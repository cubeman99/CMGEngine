#include "cmgQuickHull.h"
#include <cmgCore/cmgAssert.h>
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgMatrix3f.h>


static float DistToLine(const Vector3f& v0, const Vector3f& v1, const Vector3f& point)
{
	Vector3f edgeDirection = (v1 - v0).Normalize();
	float distAlongEdge = point.Dot(edgeDirection);
	Vector3f edgePoint = (v0 + (distAlongEdge * edgeDirection));
	return edgePoint.DistToSqr(point);
}

static float DistToTriangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& point)
{
	Vector3f abc = (b - a).Cross(c - b).Normalize();
	return Math::Abs(point.Dot(abc) - a.Dot(abc));
}

static bool CompareConflictVertices(QuickHullVertex* a, QuickHullVertex* b)
{
	return (a->distToFace > b->distToFace);
}


//-----------------------------------------------------------------------------
// QuickHullVertex
//-----------------------------------------------------------------------------

QuickHullVertex::QuickHullVertex() :
	next(nullptr),
	prev(nullptr),
	closestFace(nullptr)
{
}

QuickHullVertex::QuickHullVertex(Vector3f position) :
	position(position),
	next(nullptr),
	prev(nullptr),
	closestFace(nullptr)
{
}


//-----------------------------------------------------------------------------
// QuickHullEdge
//-----------------------------------------------------------------------------

QuickHullEdge::QuickHullEdge() :
	next(nullptr),
	prev(nullptr),
	tail(nullptr),
	twin(nullptr),
	face(nullptr),
	merged(false)
{
}

QuickHullEdge::QuickHullEdge(QuickHullVertex* tail) :
	next(nullptr),
	prev(nullptr),
	tail(tail),
	twin(nullptr),
	face(nullptr),
	merged(false)
{
}

QuickHullConvexity QuickHullEdge::CalcConvexity(float epsilon)
{
	float rightDist = face->GetDistance(twin->face->GetCenter());
	float leftDist = twin->face->GetDistance(face->GetCenter());
	if (rightDist < -epsilon && leftDist < -epsilon)
		return QuickHullConvexity::k_convex;
	if (rightDist > epsilon && leftDist > epsilon)
		return QuickHullConvexity::k_concave;
	return QuickHullConvexity::k_coplanar;
}


//-----------------------------------------------------------------------------
// QuickHullFace
//-----------------------------------------------------------------------------
	
QuickHullFace::QuickHullFace() :
	next(nullptr),
	prev(nullptr),
	edge(nullptr),
	numEdges(0),
	removed(false),
	visible(false),
	newFace(false)
{
}

// Add edge to the end of the edges list (in counter clockwise order).
void QuickHullFace::AddEdge(QuickHullEdge* newEdge)
{
	if (edge == nullptr)
	{
		newEdge->prev = newEdge;
		newEdge->next = newEdge;
		edge = newEdge;
	}
	else
	{
		newEdge->prev = edge->prev;
		edge->prev->next = newEdge;
		newEdge->next = edge;
		edge->prev = newEdge;
	}
	newEdge->face = this;
	numEdges++;
}

float QuickHullFace::GetDistance(const Vector3f& position)
{
	return (position - edge->tail->position).Dot(normal);
}

Vector3f QuickHullFace::GetCenter()
{
	return centroid;
}

QuickHullVertex* QuickHullFace::FindConvexVertex()
{
	CMG_ASSERT(numEdges >= 3);
	Vector3f normal = (edge->next->tail->position - edge->tail->position).Cross(
		edge->prev->tail->position - edge->tail->position).Normalize();

	Matrix3f toPlane;
	toPlane.c2 = normal;
	toPlane.c1 = (Math::Abs(normal.y) < Math::Abs(normal.z) ? Vector3f::UNITY : Vector3f::UNITZ);
	toPlane.c0 = toPlane.c1.Cross(toPlane.c2).Normalize();
	toPlane.c1 = toPlane.c0.Cross(toPlane.c2).Normalize();
	toPlane.Transpose();

	// Locate a vertex that lies on the bounding box of the polygon and
	// that is not collinear with its neighbors (so e.g. the top leftmost vertex).
	QuickHullEdge* topLeft = nullptr;
	QuickHullEdge* edgePtr;
	Vector2f maxProj;
	Vector2f proj;
	unsigned int i;
	for (edgePtr = edge, i = 0; i < numEdges; edgePtr = edgePtr->next, ++i)
	{
		proj = (toPlane * edgePtr->tail->position).xy;
		if (topLeft == nullptr || 
			proj.x >= maxProj.x)
		{
			maxProj = proj;
			topLeft = edgePtr;
		}
	}

	// FIXME: this wont work if the found vertex is colinear with its two edges.

	return topLeft->tail;
}

void QuickHullFace::CalcNormal()
{
	CMG_ASSERT(numEdges >= 3);
	normal = (edge->next->tail->position - edge->tail->position).Cross(
		edge->prev->tail->position - edge->tail->position).Normalize();

	Matrix3f toPlane;
	toPlane.c2 = normal;
	toPlane.c1 = (Math::Abs(normal.y) < Math::Abs(normal.z) ? Vector3f::UNITY : Vector3f::UNITZ);
	toPlane.c0 = toPlane.c1.Cross(toPlane.c2).Normalize();
	toPlane.c1 = toPlane.c0.Cross(toPlane.c2).Normalize();
	toPlane.Transpose();

	// Locate a vertex that lies on the bounding box of the polygon and
	// that is not collinear with its neighbors (so e.g. the top leftmost vertex).
	QuickHullEdge* topLeft = nullptr;
	QuickHullEdge* edgePtr;
	Vector3f maxProj;
	Vector3f proj;
	unsigned int i;
	for (edgePtr = edge, i = 0; i < numEdges; edgePtr = edgePtr->next, ++i)
	{
		proj = toPlane * edgePtr->tail->position;
		if (topLeft == nullptr || 
			proj.x >= maxProj.x)
		{
			maxProj = proj;
			topLeft = edgePtr;
		}
	}
	
	normal = (topLeft->next->tail->position - topLeft->tail->position).Cross(
		topLeft->prev->tail->position - topLeft->tail->position).Normalize();
	
	centroid = Vector3f::ZERO;
	for (edgePtr = edge, i = 0; i < numEdges; edgePtr = edgePtr->next, ++i)
		centroid += edgePtr->tail->position;
	centroid /= (float) numEdges;
}

void QuickHullFace::CreateEdges(
	unsigned int numVertices, QuickHullVertex** vertices,
	QuickHullPool<QuickHullEdge>& edgePool)
{
	// Add one edge per vertex.
	numEdges = 0;
	edge = nullptr;
	for (unsigned int i = 0; i < numVertices; ++i)
		AddEdge(edgePool.Alloc(vertices[i]));
	
	CalcNormal();
}

void QuickHullFace::MergeAdjacentFace(
	QuickHullEdge* edgeAdj,
	std::vector<QuickHullFace*>& discardedFaces)
{
	QuickHullFace* oppFace = edgeAdj->twin->face;
	CMG_ASSERT(this != oppFace);
	discardedFaces.push_back(oppFace);

	//printf("Merging face 0x%x into 0x%x\n",
		//(unsigned int) oppFace,
		//(unsigned int) this);

	QuickHullEdge* edgeOpp = edgeAdj->twin;
	edgeAdj->merged = true;
	edgeOpp->merged = true;

	// Find the seam of edges connecting these two faces.

	QuickHullEdge* edgeAdjPrev = edgeAdj->prev;
	QuickHullEdge* edgeAdjNext = edgeAdj->next;
	QuickHullEdge* edgeOppPrev = edgeOpp->prev;
	QuickHullEdge* edgeOppNext = edgeOpp->next;

	unsigned int seamSize = 1;

	while (edgeAdjPrev->twin->face == oppFace)
	{
		// Delete vertex in seam.
		
		edgeAdjPrev->merged = true;
		edgeOppNext->merged = true;
		edgeAdjPrev = edgeAdjPrev->prev;
		edgeOppNext = edgeOppNext->next;
		seamSize++;
	}
	   
	while (edgeAdjNext->twin->face == oppFace)
	{
		edgeOppPrev->merged = true;
		edgeAdjNext->merged = true;
		edgeOppPrev = edgeOppPrev->prev;
		edgeAdjNext = edgeAdjNext->next;
		seamSize++;
	}
	//printf("Seam size = %u\n", seamSize);

	numEdges += oppFace->numEdges - (2 * seamSize);
		
	QuickHullEdge* edge;

	//unsigned int i;
	//for (edge = edgeOpp, i = 0; i < oppFace->numEdges; edge = edge->next, ++i)
	//{
	//	edge->face = this;
	//}

	for (edge = edgeOppNext; edge != edgeOppPrev->next; edge = edge->next)
	{
		edge->face = this;
	}

	if (edgeAdj == this->edge)
	{
		this->edge = edgeAdjNext; 
	}
	   
	// handle the half edges at the head
	QuickHullFace* discardedFace;

	discardedFace = ConnectHalfEdges(edgeOppPrev, edgeAdjNext);
	if (discardedFace != nullptr)
	{
		discardedFaces.push_back(discardedFace);
	}

	// handle the half edges at the tail
	discardedFace = ConnectHalfEdges(edgeAdjPrev, edgeOppNext);
	if (discardedFace != nullptr)
	{
		discardedFaces.push_back(discardedFace);
	}

	// Count the number of edges.
	//unsigned int testNumEdges = numEdges + oppFace->numEdges - (2 * seamSize);

	numEdges = 0;
	edge = this->edge;
	do
	{
		edge = edge->next;
		numEdges++;
	}
	while (edge != this->edge);
	//CMG_ASSERT(numEdges == testNumEdges);
	//numEdges += oppFace->numEdges - (2 * seamSize);

	CalcNormal();
	//computeNormalAndCentroid ();
	//checkConsistency();
}

QuickHullFace* QuickHullFace::ConnectHalfEdges(
	QuickHullEdge* edgePrev, QuickHullEdge* edge)
{
	QuickHullFace* discardedFace = nullptr;

	//if (edgePrev->twin->face == edge->twin->face)
	if (false)
	{
		// Then there is a redundant edge that we can get rid of

		QuickHullFace* oppFace = edge->twin->face;
		QuickHullEdge* edgeOpp;

		numEdges--;

		if (edgePrev == this->edge)
		{
			this->edge = edge; 
		}
		if (oppFace->numEdges == 3)
		{
			// then we can get rid of the opposite face altogether
			edgeOpp = edge->twin->prev->twin;

			//oppFace.mark = DELETED;
			discardedFace = oppFace;
		}
		else
		{
			edgeOpp = edge->twin->next;

			if (oppFace->edge == edgeOpp->prev)
			{
				oppFace->edge = edgeOpp; 
			}
			edgeOpp->prev = edgeOpp->prev->prev;
			edgeOpp->prev->next = edgeOpp;
			oppFace->numEdges--;
		}
		edge->prev = edgePrev->prev;
		edge->prev->next = edge;

		edge->twin = edgeOpp;
		edgeOpp->twin = edge;

		// oppFace was modified, so need to recompute
		oppFace->CalcNormal();
	}
	else
	{
		edgePrev->next = edge;
		edge->prev = edgePrev;
	}
	return discardedFace;
}


//-----------------------------------------------------------------------------
// QuickHull
//-----------------------------------------------------------------------------

QuickHull::QuickHull()
{
}

void QuickHull::Clear()
{
	m_horizon.clear();
	m_newFaces.Clear();
	m_unclaimed.Clear();

	m_mergedEdges.clear();
	m_mergedFace = nullptr;

	m_conflictVertices.Clear();
	m_hullVertices.Clear();
	m_hullFaces.Clear();

	m_poolVertices.FreeMemory();
	m_poolEdges.FreeMemory();
	m_poolFaces.FreeMemory();
}

void QuickHull::AllocateMemory(unsigned int numPoints)
{
	m_poolVertices.AllocateMemory(numPoints);
	m_poolEdges.AllocateMemory(((3 * numPoints) - 6) * 4);
	m_poolFaces.AllocateMemory(((2 * numPoints) - 4) * 2);
	m_horizon.reserve((3 * numPoints) - 6);
	//m_newFaces.reserve((3 * numPoints) - 6);
}

QuickHullFace* QuickHull::CreateFace(unsigned int numVertices, QuickHullVertex** vertices)
{
	CMG_ASSERT(numVertices >= 3);
	QuickHullFace* face = m_poolFaces.Alloc();
	m_hullFaces.Push(face);
	face->CreateEdges(numVertices, vertices, m_poolEdges);
	return face;
}

void QuickHull::LinkTwinEdges()
{
	QuickHullFace *faceA, *faceB;
	QuickHullEdge *edgeA, *edgeB;
	unsigned int i, j;

	// Link edges for each pair of face.
	for (faceA = m_hullFaces.Head(); faceA; faceA = faceA->next)
	{
		for (edgeA = faceA->edge, i = 0;
			i < faceA->numEdges; edgeA = edgeA->next, ++i)
		{
			if (edgeA->twin != nullptr)
				continue;

			for (faceB = faceA->next; faceB && !edgeA->twin; faceB = faceB->next)
			{
				for (edgeB = faceB->edge, j = 0;
					j < faceB->numEdges && !edgeA->twin;
					edgeB = edgeB->next, ++j)
				{
					if (edgeA->tail == edgeB->next->tail &&
						edgeB->tail == edgeA->next->tail)
					{
						LinkEdgeTwins(edgeA, edgeB);
					}
				}
			}
		}
	}
}

void QuickHull::DoQuickHull(unsigned int numPoints, const Vector3f* points)
{
	Clear();

	//  Build the initial hull.
	if (!BuildInitialHull(numPoints, points))
		return;

	QuickHullVertex* vertex = NextConflictVertex();
	while (vertex != nullptr)
	{
		AddVertexToHull(vertex);
		vertex = NextConflictVertex();
	}

	// Merge all coplanar/concave faces.
	while (MergeFaces())
		;
}

bool QuickHull::BuildInitialHull(unsigned int numPoints, const Vector3f* points)
{
	if (numPoints < 4)
		return false;

	QuickHullVertex* vertex;
	
	// Allocate storage for vertices, half-edges, and faces.
	AllocateMemory(numPoints);

	// Add all points to the list of conflict vertices.
	m_unclaimed.Clear();
	for (unsigned int i = 0; i < numPoints; ++i)
	{
		vertex = m_poolVertices.Alloc();
		vertex->position = points[i];
		m_unclaimed.Push(vertex);
	}

	// Find the 4 initial hull vertices.
	QuickHullVertex* initialVertices[4];
	if (!FindInitialHullVertices(initialVertices))
		return false;
	
	// Create the a tetrahedron hull from those 4 vertices.
	CreateTetrahedronHull(initialVertices);
	
	// Assign other conflict vertices to their nearest face's conflict list.
	QuickHullFace *face;
	QuickHullVertex* nextVertex;
	float dist;
	for (vertex = m_unclaimed.Head();
		vertex != nullptr; vertex = nextVertex)
	{
		nextVertex = vertex->next;

		// Find the face closest to this vertex.
		vertex->closestFace = nullptr;
		vertex->distToFace = m_tolerance;
		for (face = m_hullFaces.Head(); face != nullptr; face = face->next)
		{
			dist = face->GetDistance(vertex->position);
			if (dist > vertex->distToFace)
			{
				vertex->closestFace = face;
				vertex->distToFace = dist;
			}
		}

		// Add the vertex to the face's conflict list,
		// or remove it if it is inside the hull.
		if (vertex->closestFace != nullptr)
		{
			vertex->closestFace->conflictList.push_front(vertex);
			m_conflictVertices.InsertSorted(vertex, CompareConflictVertices);
		}
		else
		{
			m_poolVertices.Free(vertex);
		}
	}
	
	return true;
}

bool QuickHull::FindInitialHullVertices(
	QuickHullVertex** initialVertices)
{
	QuickHullVertex* extremeVertices[6] = { 0 };
	QuickHullVertex* vertex;
	float maxDist, dist;

	// Identify the extreme vertices along each cardinal axis.
	for (unsigned int i = 0; i < 6; ++i)
		extremeVertices[i] = m_unclaimed.Head();
	for (vertex = m_unclaimed.Head()->next;
		vertex != nullptr; vertex = vertex->next)
	{
		for (unsigned int axis = 0; axis < 3; ++axis)
		{
			if (vertex->position[axis] < extremeVertices[axis]->position[axis])
				extremeVertices[axis] = vertex;
			if (vertex->position[axis] > extremeVertices[axis + 3]->position[axis])
				extremeVertices[axis + 3] = vertex;
		}
	}

	Vector3f mins, maxs;
	mins.x = extremeVertices[0]->position.x;
	mins.y = extremeVertices[1]->position.y;
	mins.z = extremeVertices[2]->position.z;
	maxs.x = extremeVertices[3]->position.x;
	maxs.y = extremeVertices[4]->position.y;
	maxs.z = extremeVertices[5]->position.z;

	m_tolerance = 3.0f * FLT_EPSILON *
		(Math::Max(Math::Abs(maxs.x), Math::Abs(mins.x)) +
		 Math::Max(Math::Abs(maxs.y), Math::Abs(mins.y)) +
		 Math::Max(Math::Abs(maxs.z), Math::Abs(mins.z)));

	// From those 6 points, choose the pair which is furthest apart.
	maxDist = -FLT_MAX;
	for (unsigned int i = 0; i < 6; ++i)
	{
		for (unsigned int j = i + 1; j < 6; ++j)
		{
			dist = extremeVertices[i]->position.DistToSqr(
				extremeVertices[j]->position);
			if (dist > maxDist)
			{
				maxDist = dist;
				initialVertices[0] = extremeVertices[i];
				initialVertices[1] = extremeVertices[j];
			}
		}
	}
	m_unclaimed.Remove(initialVertices[0]);
	m_unclaimed.Remove(initialVertices[1]);
	
	// Find the point furthest from the line between these two extreme points.
	maxDist = -FLT_MAX;
	for (vertex = m_unclaimed.Head();
		vertex != nullptr; vertex = vertex->next)
	{
		dist = DistToLine(
			initialVertices[0]->position,
			initialVertices[1]->position,
			vertex->position);
		if (dist > maxDist)
		{
			initialVertices[2] = vertex;
			maxDist = dist;
		}
	}
	m_unclaimed.Remove(initialVertices[2]);
	
	// Find the point furthest from the triangle between the previous 3 points.
	maxDist = -FLT_MAX;
	for (vertex = m_unclaimed.Head();
		vertex != nullptr; vertex = vertex->next)
	{
		dist = DistToTriangle(
			initialVertices[0]->position,
			initialVertices[1]->position,
			initialVertices[2]->position,
			vertex->position);
		if (dist > maxDist)
		{
			maxDist = dist;
			initialVertices[3] = vertex;
		}
	}
	m_unclaimed.Remove(initialVertices[3]);
	return true;
}

void QuickHull::CreateTetrahedronHull(QuickHullVertex** vertices)
{
	QuickHullVertex *a, *b, *c, *d;
	QuickHullVertex* faceVertices[3];
	Vector3f normal;

	// Create the 4 faces, making sure they are in CCW order.
	for (unsigned int i = 0; i < 4; ++i)
	{
		// Add the 4 vertices to the hull.
		m_hullVertices.Push(vertices[i]);

		// Get the 3 vertices on this face, and the one below it.
		a = vertices[(i + 0) % 4];
		b = vertices[(i + 1) % 4];
		c = vertices[(i + 2) % 4];
		d = vertices[(i + 3) % 4];

		// Compute the face normal direction.
		normal = (b->position - a->position).
			Cross(c->position - a->position);

		// Define the list of face vertices.
		// Make sure it is in CCW order compared to D.
		if (normal.Dot(a->position - d->position) > 0.0f)
		{
			faceVertices[0] = a;
			faceVertices[1] = b;
			faceVertices[2] = c;
		}
		else
		{
			// Reverse winding order and flip normal.
			faceVertices[0] = a;
			faceVertices[1] = c;
			faceVertices[2] = b;
		}

		// Create the face from the 3 vertices.
		CreateFace(3, faceVertices);
	}

	// Link up twin edges for the initial hull.
	LinkTwinEdges();
}

QuickHullVertex* QuickHull::NextConflictVertex()
{
	if (m_conflictVertices.Empty())
		return nullptr;

	// The conflict list is sorted by distance to
	// closest face, so we can just pop its front.
	QuickHullVertex* vertex = m_conflictVertices.Pop();
	vertex->closestFace->conflictList.remove(vertex);
	m_hullVertices.Push(vertex);
	CMG_ASSERT(!vertex->closestFace->removed);
	return vertex;
}

void QuickHull::AddVertexToHull(QuickHullVertex* vertex)
{
	ValidateEdges();

	BuildHorizon(m_horizon, vertex);

	BuildNewFaces(m_newFaces, m_horizon, vertex);
	ValidateEdges();
	//MergeFaces(m_newFaces);
	
	ResolveOrphans(m_newFaces);

	// Add new faces to the hull.
	while (!m_newFaces.Empty())
	{
		QuickHullFace* face = m_newFaces.Pop();
		m_hullFaces.Push(face);
		face->newFace = false;
	}
}

void QuickHull::SearchHorizon(
	std::vector<QuickHullEdge*>& horizon,
	QuickHullVertex* viewPoint,
	QuickHullEdge* firstEdge)
{
	QuickHullFace* face = firstEdge->face;
	face->visible = true;

	// Iterate edges and cross over to neighboring faces.
	QuickHullEdge* edge = firstEdge;
	QuickHullFace* neighboringFace;
	do
	{
		neighboringFace = edge->twin->face;

		// Check if the neighboring face is not visible.
		if (!neighboringFace->visible)
		{
			// Determine face visibility.
			if (neighboringFace->GetDistance(viewPoint->position) > m_tolerance)
			{
				// Continue the search on the neighboring face.
				SearchHorizon(horizon, viewPoint, edge->twin->next);
			}
			else
			{
				// This edge is part of the horizon because it
				// connects a visible and non-visible face.
				horizon.push_back(edge->twin);
			}
		}

		edge = edge->next;
	}
	while (edge != firstEdge);
	
	// Unclaim all this face's conflict points.
	DeleteFacePoints(face);

	// Remove this face from the hull.
	m_hullFaces.Remove(face);
	m_poolFaces.Free(face);

	// Free all its half-edges.
	unsigned int i;
	QuickHullEdge* nextEdge;
	for (edge = face->edge, i = 0; i < face->numEdges; edge = nextEdge, ++i)
	{
		nextEdge = edge->next;
		m_poolEdges.Free(edge);
	}
}

void QuickHull::BuildHorizon(
	std::vector<QuickHullEdge*>& horizon,
	QuickHullVertex* viewPoint)
{
	m_unclaimed.Clear();
	horizon.clear();

	// Perform a depth first search starting from the view point's closest face.
	// This will create the list of horizon edges in CCW order.
	viewPoint->closestFace->visible = true;
	SearchHorizon(horizon, viewPoint,
		viewPoint->closestFace->edge);
}

void QuickHull::BuildNewFaces(
	QuickHullList<QuickHullFace>& newFaces,
	std::vector<QuickHullEdge*>& horizon,
	QuickHullVertex* viewPoint)
{
	QuickHullFace* face;
	QuickHullFace* nextFace;
	QuickHullVertex* faceVertices[3];
	faceVertices[2] = viewPoint;
	newFaces.Clear();

	// Create one face per horizon edge.
	for (unsigned int i = 0; i < m_horizon.size(); ++i)
	{
		faceVertices[0] = m_horizon[(i + m_horizon.size() - 1) % m_horizon.size()]->tail;
		faceVertices[1] = m_horizon[i]->tail;
		face = CreateFace(3, faceVertices);
		LinkEdgeTwins(face->edge, m_horizon[i]);
		m_hullFaces.Remove(face);
		newFaces.Push(face);
		face->newFace = true;
	}

	// Link edges between new faces.
	for (face = m_newFaces.Head(); face != nullptr; face = face->next)
	{
		nextFace = face->next;
		if (nextFace == nullptr)
			nextFace = m_newFaces.Head();
		LinkEdgeTwins(face->edge->next->next, nextFace->edge->next);
	}
}

bool QuickHull::MergeFaces()
{
	QuickHullFace* face;

	m_mergedEdges.clear();
	m_mergedEdges.clear();

	for (face = m_hullFaces.Head(); face != nullptr; face = face->next)
	{
		if (MergeAdjacentFaces(face))
		{
			ValidateFace(face);
			ValidateEdges();
			return true;
		}
	}

	return false;
}

bool QuickHull::MergeFaces(
	QuickHullList<QuickHullFace>& newFaces)
{
	QuickHullFace* face;
	//QuickHullEdge* edge;
	//unsigned int j;

	// First merge pass: merge faces which are non-convex
	// as determined by the larger face.
	for (face = newFaces.Head(); face != nullptr; face = face->next)
	{
		//if (face.mark == Face.VISIBLE)
		{
			while (MergeAdjacentFaces(face))
			{
				ValidateFace(face);
				ValidateEdges();
				return true;
			}
			ValidateFace(face);
			ValidateEdges();
		}
	}

	return false;

	//// Second merge pass: merge faces which are non-convex
	//// with respect to either face.
	//for (face = newFaces.first(); face!=null; face=face.next)
	//{
	//	if (face.mark == Face.NON_CONVEX)
	//	{
	//		face.mark = Face.VISIBLE;
	//		while (doAdjacentMerge(face, NONCONVEX))
	//			;
	//	}
	//}


	/*
	for (auto it = newFaces.begin(); it != newFaces.end(); ++it)
	{
		face = *it;
		if (face->removed)
			continue;

		for (edge = face->edge, j = 0; j < face->numEdges; )
		{
			if (edge->CalcConvexity(epsilon) != QuickHullConvexity::k_convex)
			{
				MergeFace(edge);
				edge = face->edge;
				j = 0;
			}
			else
			{
				edge = edge->next;
				++j;
			}
		}
	}

	// Remove removed faces from new faces list.
	for (j = 0; j < newFaces.size(); ++j)
	{
		if (newFaces[j]->removed)
		{
			newFaces.erase(newFaces.begin() + j);
			j--;
		}
	}
	*/
}

static float oppFaceDistance(QuickHullEdge* edge)
{
	return edge->face->GetDistance(edge->twin->face->GetCenter());
}

bool QuickHull::MergeAdjacentFaces(QuickHullFace* face)
{
	QuickHullEdge* edge;
	unsigned int i;

	for (edge = face->edge, i = 0;
		i < face->numEdges; edge = edge->next, ++i)
	{
		QuickHullFace* oppFace = edge->twin->face;
		bool merge = false;
		
		if (oppFaceDistance(edge) > -m_tolerance ||
			oppFaceDistance(edge->twin) > -m_tolerance)
		{
			merge = true;
		}

		if (merge)
		{
			m_mergedEdges.push_back(edge);
			MergeFace(edge);
			return true;
		}
	}

	return false;
}

void QuickHull::MergeFace(QuickHullEdge* edgeAdj)
{
	QuickHullFace* adjFace = edgeAdj->face;
	QuickHullFace* oppFace = edgeAdj->twin->face;
	CMG_ASSERT(adjFace != oppFace);
	DiscardFace(oppFace, adjFace);
	m_mergedFace = adjFace;

	//printf("Merging face 0x%x into 0x%x\n",
	//	(unsigned int) oppFace,
	//	(unsigned int) this);

	QuickHullEdge* edgeOpp = edgeAdj->twin;
	edgeAdj->merged = true;
	edgeOpp->merged = true;

	// Find the seam of edges connecting these two faces.

	QuickHullEdge* edgeAdjPrev = edgeAdj->prev;
	QuickHullEdge* edgeAdjNext = edgeAdj->next;
	QuickHullEdge* edgeOppPrev = edgeOpp->prev;
	QuickHullEdge* edgeOppNext = edgeOpp->next;

	unsigned int seamSize = 1;

	while (edgeAdjPrev->twin->face == oppFace)
	{
		edgeAdjPrev->merged = true;
		edgeOppNext->merged = true;
		edgeAdjPrev = edgeAdjPrev->prev;
		edgeOppNext = edgeOppNext->next;
		seamSize++;
	}
	   
	while (edgeAdjNext->twin->face == oppFace)
	{
		edgeOppPrev->merged = true;
		edgeAdjNext->merged = true;
		edgeOppPrev = edgeOppPrev->prev;
		edgeAdjNext = edgeAdjNext->next;
		seamSize++;
	}
	QuickHullEdge* seamFirst = edgeAdjPrev->next;
	QuickHullEdge* seamLast = edgeAdjNext->prev;


	//printf("Seam size = %u\n", seamSize);

	adjFace->numEdges += oppFace->numEdges - (2 * seamSize);
		
	// Assign the merged edges to the absorbing face.
	QuickHullEdge* edge;
	for (edge = edgeOppNext; edge != edgeOppPrev->next; edge = edge->next)
		edge->face = adjFace;

	// Make sure we don't invalidate the face's first edge pointer.
	if (adjFace->edge == edgeAdj)
		adjFace->edge = edgeAdjNext; 
	   
	// Connect the two faces by the ends of the seam .
	ConnectHalfEdges(edgeOppPrev, edgeAdjNext);
	ConnectHalfEdges(edgeAdjPrev, edgeOppNext);

	// Count the number of edges.
	//unsigned int testNumEdges = numEdges + oppFace->numEdges - (2 * seamSize);
	adjFace->numEdges = 0;
	edge = adjFace->edge;
	do
	{
		edge = edge->next;
		adjFace->numEdges++;
	}
	while (edge != adjFace->edge);
	//CMG_ASSERT(numEdges == testNumEdges);
	//numEdges += oppFace->numEdges - (2 * seamSize);

	// Discard half-edges and vertices in the seam.
	edge = seamFirst;
	QuickHullEdge* nextEdge;
	while (true)
	{
		nextEdge = edge->next;

		// Discard vertices in between seam edges.
		if (edge != seamFirst)
			DiscardVertex(edge->tail);

		// Discard seam edges.
		m_poolEdges.Free(edge);
		m_poolEdges.Free(edge->twin);

		if (edge == seamLast)
			break;
		edge = nextEdge;
	}

	//for (edge = seamFirst; edge != edgeAdjNext; edge = nextEdge)
	//{
	//	nextEdge = edge->next;

	//	// Discard vertices in between seam edges.
	//	if (edge != edgeAdjPrev->next)
	//		DiscardVertex(edge->tail);

	//	// Discard seam edges.
	//	//m_poolEdges.Free(edge);
	//	//m_poolEdges.Free(edge->twin);
	//}

	adjFace->CalcNormal();
	ValidateFace(adjFace);
}

void QuickHull::ConnectHalfEdges(QuickHullEdge* edgePrev, QuickHullEdge* edgeNext)
{
	QuickHullFace* adjFace = edgePrev->face;
	QuickHullFace* discardedFace = nullptr;

	//if (edgePrev->twin->face == edge->twin->face)
	if (false)
	{
		// Then there is a redundant edge that we can get rid of

		QuickHullFace* oppFace = edgeNext->twin->face;
		QuickHullEdge* edgeOpp;

		//numEdges--;

		if (adjFace->edge == edgePrev)
		{
			adjFace->edge = edgeNext; 
		}
		if (oppFace->numEdges == 3)
		{
			// then we can get rid of the opposite face altogether
			edgeOpp = edgeNext->twin->prev->twin;
			DiscardFace(oppFace);
		}
		else
		{
			edgeOpp = edgeNext->twin->next;

			if (oppFace->edge == edgeOpp->prev)
			{
				oppFace->edge = edgeOpp; 
			}
			edgeOpp->prev = edgeOpp->prev->prev;
			edgeOpp->prev->next = edgeOpp;
			oppFace->numEdges--;
		}
		edgeNext->prev = edgePrev->prev;
		edgeNext->prev->next = edgeNext;

		edgeNext->twin = edgeOpp;
		edgeOpp->twin = edgeNext;

		// oppFace was modified, so need to recompute
		oppFace->CalcNormal();
	}
	else
	{
		edgePrev->next = edgeNext;
		edgeNext->prev = edgePrev;
	}
}

void QuickHull::ResolveOrphans(
		QuickHullList<QuickHullFace>& newFaces)
{
	QuickHullVertex* vertex;
	QuickHullFace* face;
	float dist;

	// Assign unclaimed points to their closest face.
	while (!m_unclaimed.Empty())
	{
		vertex = m_unclaimed.Pop();

		// Find the new face closest to this vertex.
		vertex->closestFace = nullptr;
		vertex->distToFace = m_tolerance;
		for (face = newFaces.Head(); face != nullptr; face = face->next)
		{
			dist = face->GetDistance(vertex->position);
			if (dist > vertex->distToFace)
			{
				vertex->closestFace = face;
				vertex->distToFace = dist;
			}
		}

		// Add the vertex to the face's conflict list,
		// or remove it if it is inside the hull.
		if (vertex->closestFace != nullptr)
		{
			vertex->closestFace->conflictList.push_front(vertex);
			m_conflictVertices.InsertSorted(vertex, CompareConflictVertices);
		}
		else
		{
			m_poolVertices.Free(vertex);
		}
	}
}

void QuickHull::DeleteFacePoints(QuickHullFace* face, QuickHullFace* absorbingFace)
{
	QuickHullVertex* vertex;

	for (auto it = face->conflictList.begin();
		it != face->conflictList.end(); ++it)
	{
		vertex = *it;

		if (absorbingFace == nullptr)
		{
			m_conflictVertices.Remove(vertex);
			m_unclaimed.Push(vertex);
		}
		else
		{
			vertex->distToFace = absorbingFace->GetDistance(vertex->position);

			if (vertex->distToFace > m_tolerance)
			{
				vertex->closestFace = absorbingFace;
				absorbingFace->conflictList.push_back(vertex);
			}
			else
			{
				m_conflictVertices.Remove(vertex);
				m_unclaimed.Push(vertex);
			}

		}
	}
	face->conflictList.clear();
}

void QuickHull::LinkEdgeTwins(QuickHullEdge* edgeA, QuickHullEdge* edgeB)
{
	edgeA->twin = edgeB;
	edgeB->twin = edgeA;
}

void QuickHull::DiscardFace(QuickHullFace* face, QuickHullFace* absorbingFace)
{
	// Remove the face from the hull.
	if (face->newFace)
		m_newFaces.Remove(face);
	else
		m_hullFaces.Remove(face);
	m_poolFaces.Free(face);

	// Handle the orphaned conflict points.
	DeleteFacePoints(face, absorbingFace);
}

void QuickHull::DiscardVertex(QuickHullVertex* vertex)
{
	// Remove the vertex from the hull.
	m_hullVertices.Remove(vertex);
	m_poolVertices.Free(vertex);
}


void QuickHull::PrintFaceInfo(QuickHullFace* face)
{
	QuickHullEdge* edge;
	unsigned int i;

	printf("Face %d:\n", GetFaceNumber(face));
	for (edge = face->edge, i = 0;
		i < face->numEdges; edge = edge->next, ++i)
	{
		printf("%d.%d, ", GetEdgeNumber(edge), GetFaceNumber(edge->face));
	}
	printf("\n");
	for (edge = face->edge->prev, i = 0;
		i < face->numEdges; edge = edge->prev, ++i)
	{
		printf("%d.%d, ", GetEdgeNumber(edge), GetFaceNumber(edge->face));
	}
	printf("\n");
}

int QuickHull::GetEdgeNumber(QuickHullEdge* edge)
{
	return (int) (edge - m_poolEdges.GetData());
}

int QuickHull::GetFaceNumber(QuickHullFace* face)
{
	return (int) (face - m_poolFaces.GetData());
}

void QuickHull::ValidateFace(QuickHullFace* face)
{
	QuickHullEdge* edge;
	unsigned int i;

	for (edge = face->edge, i = 0; i < face->numEdges; edge = edge->next, ++i)
	{
		CMG_ASSERT(edge->twin != edge);
		CMG_ASSERT(edge->twin->twin == edge);
		CMG_ASSERT(edge->next->prev == edge);
		CMG_ASSERT(edge->prev->next == edge);
		CMG_ASSERT(edge->twin != edge->next);
		CMG_ASSERT(edge->twin != edge->prev);
		CMG_ASSERT(edge->face == face);
		CMG_ASSERT(edge->face == edge->next->face);
		CMG_ASSERT(edge->face == edge->prev->face);
		CMG_ASSERT(edge->twin->face != edge->face); // peninsula test
		CMG_ASSERT(!edge->merged);
		CMG_ASSERT(!edge->twin->merged);
	}

	CMG_ASSERT(edge == face->edge);
}

void QuickHull::ValidateEdges()
{
	QuickHullFace* face;

	for (face = m_hullFaces.Head(); face; face = face->next)
	{
		ValidateFace(face);
	}
}
