#include "cmgGJK.h"
#include <cmgCore/cmgAssert.h>

// http://www.cs.sjsu.edu/faculty/pollett/masters/Semesters/Spring12/josh/GJK.html
// https://www.youtube.com/watch?v=Qupqu1xe7Io


Simplex::Simplex() :
	numPoints(0)
{
}

void Simplex::Clear()
{
	numPoints = 0;
}

void Simplex::Add(const SupportPoint& point)
{
	// Shift existing points up.
	for (unsigned int i = numPoints; i > 0; --i)
		points[i] = points[i - 1];

	// Insert new point at beginning.
	points[0] = point;

	++numPoints;
}

void Simplex::Remove(int index)
{
	// Shift points down.
	--numPoints;
	for (unsigned int i = index; i < numPoints; ++i)
		points[i] = points[i + 1];
}

bool GJK::TestIntersection(
	Collider* shapeA,
	Collider* shapeB,
	Simplex* outSimplex)
{
	Simplex simplex;
	Vector3f direction = Vector3f(1, 1, 1);

	// Add the first support point to the simplex.
	SupportPoint nextPoint = GJK::GetSupportMinkowskiDiff(
		direction, shapeA, shapeB);
	simplex.Add(nextPoint);
	direction = -nextPoint.p; // Search towards the origin.
	bool intersecting = false;

	unsigned int maxIterations = 100; // TODO: fix infinite loops.
	unsigned int i;

	// GJK loop
	for (i = 0; i < maxIterations; ++i)
	{
		// Get the next support point.
		nextPoint = GJK::GetSupportMinkowskiDiff(
			direction, shapeA, shapeB);
		if (nextPoint.p.Dot(direction) < 0.0f)
		{
			intersecting = false;
			break;
		}
		simplex.Add(nextPoint);

		// Evolve the simplex, finding a new search direction
		// while also removing any redundant points.
		if (GJK::DoSimplex(simplex, direction))
		{
			intersecting = true;
			break;
		}
	}
	
	if (outSimplex != nullptr)
		*outSimplex = simplex;
	return intersecting;
}


SupportPoint GJK::GetSupportMinkowskiDiff(
	const Vector3f& direction,
	Collider* shapeA, Collider* shapeB)
{
	SupportPoint point;
	point.a = shapeA->GetSupportPoint(direction);
	point.p = point.a - shapeB->GetSupportPoint(-direction);
	point.next = NULL;
	return point;
}


// Evolve the simplex.
bool GJK::DoSimplex(Simplex& simplex, Vector3f& direction)
{
	Vector3f a = simplex.points[0].p;
	Vector3f b = simplex.points[1].p;
	Vector3f c = simplex.points[2].p;
	Vector3f d = simplex.points[3].p;
	Vector3f ab = b - a;
	Vector3f ac = c - a;
	Vector3f ad = d - a;
	Vector3f ao = -a;
	Vector3f abc = -ab.Cross(ac);
	Vector3f acd = -ac.Cross(ad);
	Vector3f adb = -ad.Cross(ab);

	//-------------------------------------------------------------------------
	// LINE CASE
	if (simplex.numPoints == 2)
	{
		if (ao.Dot(ab) > 0.0f)
		{
			// Direction toward the origin that's perpendicular to
			// the line from A to B.
			direction = ab.Cross(ao).Cross(ab);
		}
		else
		{
			direction = ao;
			simplex.Remove(1); // Remove B
		}
	}

	//-------------------------------------------------------------------------
	// TRIANGLE CASE
	else if (simplex.numPoints == 3)
	{
		if (ao.Dot(abc.Cross(ab)) > 0.0f)
		{
			simplex.Remove(2); // Remove C
			return DoSimplex(simplex, direction);
		}
		else if (ao.Dot(abc.Cross(-ac)) > 0.0f)
		{
			simplex.Remove(1); // Remove B
			return DoSimplex(simplex, direction);
		}
		else
		{
			// Check if the origin is above or below the triangle.
			if (ao.Dot(abc) > 0.0f)
			{
				// Closest to above triangle.
				direction = abc;
			}
			else
			{
				// Closest to below triangle.
				direction = -abc;
			}
		}
	}

	//-------------------------------------------------------------------------
	// TETRAHEDRON CASE
	else if (simplex.numPoints == 4)
	{
		if (ao.Dot(acd) > 0.0f)
		{
			simplex.Remove(1); // Remove B
			return DoSimplex(simplex, direction);
		}
		else if (ao.Dot(adb) > 0.0f)
		{
			simplex.Remove(2); // Remove C
			return DoSimplex(simplex, direction);
		}
		else if (ao.Dot(abc) > 0.0f)
		{
			simplex.Remove(3); // Remove D
			return DoSimplex(simplex, direction);
		}
		else
		{
			// The simplex contains the origin!
			return true;
		}
	}

	else
	{
		CMG_ASSERT_MSG(false, "Invalid number of points in simplex");
	}

	return false;
}




// http://hacktank.net/blog/?p=119


Polytope::Polytope() :
	m_numFaces(0),
	m_faces(nullptr),
	m_numPoints(0),
	m_points(nullptr)
{

}

Polytope::Polytope(const Simplex& simplex)
{
	CMG_ASSERT(simplex.numPoints == 4);
	
	m_points = nullptr;
	m_faces = nullptr;
	m_numPoints = 4;
	m_numFaces = 4;

	SupportPoint* polyPoints[4];
	SupportPoint a, b, c, d;

	// Add the support points to the polytope.
	for (unsigned int i = 0; i < 4; ++i)
	{
		polyPoints[i] = new SupportPoint(simplex.points[i]);
		if (i == 0)
			m_points = polyPoints[i];
		else
			polyPoints[i - 1]->next = polyPoints[i];
		if (i == 3)
			polyPoints[i]->next = nullptr;
	}

	// Add the faces, making sure they are in CCW order.
	for (unsigned int i = 0; i < 4; ++i)
	{
		// Get the 3 points on this face, and the one below it.
		a = simplex.points[(i + 0) % 4];
		b = simplex.points[(i + 1) % 4];
		c = simplex.points[(i + 2) % 4];
		d = simplex.points[(i + 3) % 4];

		// Create face ABC, make sure it is in CCW order compared to D.
		PolytopeFace* face = new PolytopeFace();
		face->next = m_faces;
		m_faces = face;

		Vector3f normal = (b.p - a.p).Cross(c.p - a.p).Normalize();

		if (normal.Dot(a.p - d.p) > 0.0f)
		{
			face->p0 = polyPoints[(i + 0) % 4];
			face->p1 = polyPoints[(i + 1) % 4];
			face->p2 = polyPoints[(i + 2) % 4];
			face->normal = normal;
		}
		else
		{
			// Reverse winding order and flip normal.
			face->p0 = polyPoints[(i + 0) % 4];
			face->p1 = polyPoints[(i + 2) % 4];
			face->p2 = polyPoints[(i + 1) % 4];
			face->normal = -normal;
		}
	}
}

Polytope::~Polytope()
{
	// Free linked lists.
	for (PolytopeFace* face = m_faces; face; )
	{
		PolytopeFace* nextFace = face->next;
		delete face;
		face = nextFace;
	}
	for (SupportPoint* point = m_points; point; )
	{
		SupportPoint* nextPoint = point->next;
		delete point;
		point = nextPoint;
	}
}

void Polytope::AddPoint(const SupportPoint& newPoint)
{
	// Allocate storage for the point.
	SupportPoint* point = new SupportPoint(newPoint);
	point->next = m_points;
	m_points = point;
	
	// Remove all faces that this point can "see"
	// This is just like backface culling.
	PolytopeFace* facePrev = nullptr;
	for (PolytopeFace* face = m_faces; face; )
	{
		PolytopeFace* nextFace = face->next;

		if (face->normal.Dot(point->p - face->p0->p) > 0.0f)
		{
			// Add the edges from the face.
			AddEdge(face->p0, face->p1);
			AddEdge(face->p1, face->p2);
			AddEdge(face->p2, face->p0);

			// Remove the face.
			RemoveFace(face, facePrev);
		}
		else
			facePrev = face;

		face = nextFace;
	}
	
	// Create new triangles from the edges in the edge list.
	for (auto it = m_edges.begin(); it != m_edges.end(); ++it)
	{
		PolytopeFace* newFace = new PolytopeFace(point, it->p0, it->p1);
		newFace->next = m_faces;
		m_faces = newFace;
	}
 
	m_edges.clear();

	++m_numPoints;
}

void Polytope::RemoveFace(PolytopeFace* face, PolytopeFace* facePrev)
{
	if (facePrev == nullptr)
		m_faces = face->next;
	else
		facePrev->next = face->next;
	delete face;
	--m_numFaces;
}

void Polytope::AddEdge(SupportPoint* p0, SupportPoint* p1)
{
	for (auto it = m_edges.begin(); it != m_edges.end(); ++it)
	{
		if (it->p0 == p1 && it->p1 == p0)
		{
			// Opposite edge found, remove it and do not add new one
			m_edges.erase(it);
			return;
		}
	}
	m_edges.emplace_back(p0, p1);
}

PolytopeFace* Polytope::FindFaceClosestOrigin()
{
	PolytopeFace* closestFace = nullptr;
	
	for (PolytopeFace* face = m_faces; face; face = face->next)
	{
		// Face normals point away from the origin.
		face->distToOrigin = face->normal.Dot(face->p0->p);
		if (closestFace == nullptr ||
			face->distToOrigin < closestFace->distToOrigin)
		{
			closestFace = face;
		}
	}

	return closestFace;
}


void EPA::Barycentric(
	const Vector3f& p, const Vector3f& a,
	const Vector3f& b, const Vector3f& c,
	float* u, float* v, float* w)
{
     // code from Crister Erickson's Real-Time Collision Detection
     Vector3f v0 = b - a,v1 = c - a,v2 = p - a;
     float d00 = v0.Dot(v0);
     float d01 = v0.Dot(v1);
     float d11 = v1.Dot(v1);
     float d20 = v2.Dot(v0);
     float d21 = v2.Dot(v1);
     float denom = d00 * d11 - d01 * d01;
     *v = (d11 * d20 - d01 * d21) / denom;
     *w = (d00 * d21 - d01 * d20) / denom;
     *u = 1.0f - *v - *w;
}



// EPA References:
// http://hacktank.net/blog/?p=119
// http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/

EPAResult EPA::PerformEPA(Collider* shapeA, Collider* shapeB, const Simplex& gjkTerminationSimplex)
{
	// The tolerance should be something positive close to zero (ex. 0.00001)
	const float EPA_TOLERANCE = 0.0001f;

	// Create the polytop from the termination simplex.
	Polytope polytope(gjkTerminationSimplex);

	EPAResult result;

	unsigned int maxIterations = 100;
	unsigned int i;

	for (i = 0; i < maxIterations; ++i)
	{
		// Find the closest face to the origin on the Minkowski Difference.
		PolytopeFace* face = polytope.FindFaceClosestOrigin();

		// Obtain a new support point in the direction of the face normal.
		SupportPoint point = GJK::GetSupportMinkowskiDiff(
			face->normal, shapeA, shapeB);

		// check the distance from the origin to the edge against the
		// distance p is along e.normal
		float dist = point.p.Dot(face->normal);
		if (dist - face->distToOrigin < EPA_TOLERANCE)
		{
			// If the difference is less than the tolerance then we can
			// assume that we cannot expand the simplex any further and
			// we have our solution.

			float baryCoords[3];
			EPA::Barycentric(face->normal * face->distToOrigin,
				face->p0->p, face->p1->p, face->p2->p,
				baryCoords + 0, baryCoords + 1, baryCoords + 2);
			result.contactPoint =
				(face->p0->a * baryCoords[0] +
				face->p1->a * baryCoords[1] +
				face->p2->a * baryCoords[2]);
			result.passed = true;
			result.normal = -face->normal;
			result.depth = face->distToOrigin;
			return result;
		}
		else
		{
			// We haven't reached the edge of the Minkowski Difference
			// so continue expanding by adding the new point to the polytope.
			// This function maintains convexity.
			polytope.AddPoint(point);
		}
	}

	result.passed = false;
	return result;
}

