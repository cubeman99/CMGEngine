#include "cmgCollisionDetector.h"
#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgPlane.h>
#include <cmgMath/types/cmgRay.h>
#include <cmgPhysics/cmgGJK.h>

CollisionDetector::CollisionDetector()
{

}

void CollisionDetector::DetectCollision(RigidBody* one, RigidBody* two, CollisionData* collisionData)
{
	collisionData->firstBody = one;
	collisionData->secondBody = two;
	collisionData->numContacts = 0;

	if (one->m_inverseMass == 0.0f && two->m_inverseMass == 0.0f)
		return;

	for (auto it1 = one->colliders_begin(); it1 != one->colliders_end(); ++it1)
	{
		for (auto it2 = two->colliders_begin(); it2 != two->colliders_end(); ++it2)
		{
			DetectCollision(*it1, *it2, collisionData);
		}
	}
}

void CollisionDetector::DetectCollision(
	Collider* a, Collider* b, CollisionData* collisionData)
{
	Simplex simplex;
	bool gjkResult = GJK::TestIntersection(a, b, &simplex);

	if (gjkResult)
	{
		EPAResult epaResult = EPA::PerformEPA(a, b, simplex);
		if (epaResult.passed)
		{
			collisionData->firstBody = a->GetBody();
			collisionData->secondBody = b->GetBody();

			collisionData->numContacts = 1;
			collisionData->contacts[0].body[0]			= a->GetBody();
			collisionData->contacts[0].body[1]			= b->GetBody();
			collisionData->contacts[0].contactNormal	= epaResult.normal;
			collisionData->contacts[0].penetration		= epaResult.depth;
			collisionData->contacts[0].contactPoint		= epaResult.contactPoint;
			collisionData->contacts[0].worldPositionA	= epaResult.contactPointA;
			collisionData->contacts[0].worldPositionB	= epaResult.contactPointB;
			collisionData->contacts[0].localPositionA	= a->GetBody()->GetWorldToBody().TransformAffine(epaResult.contactPointA);
			collisionData->contacts[0].localPositionB	= b->GetBody()->GetWorldToBody().TransformAffine(epaResult.contactPointB);
			collisionData->contacts[0].localNormal		= b->GetBody()->GetWorldToBody().Rotate(epaResult.normal);
	
		}
	}
}


void CollisionDetector::GenerateContactsEPA(
	Collider* a, Collider* b, CollisionData* collisionData, const EPAResult& epa)
{
	ColliderType typeA = a->GetType();
	ColliderType typeB = b->GetType();
	
	if (typeA == ColliderType::k_box &&
		typeB == ColliderType::k_box)
	{
		BoxCollider* boxA = (BoxCollider*) a;
		BoxCollider* boxB = (BoxCollider*) b;
		BoxCollider* boxes[2] = { boxA, boxB };

		// Identify the reference face.
		unsigned int refFaceIndex;
		BoxCollider* refFaceBox;
		unsigned int axisIndex;
		float maxDot = -FLT_MAX;
		for (unsigned int i = 0; i < 6; ++i)
		{
			axisIndex = i % 3;
			BoxCollider* box = boxes[i / 3];
			Vector3f axis = box->GetShapeToWorld().col[axisIndex].xyz;

			float dot = axis.Dot(epa.normal);
			if (dot > maxDot)
			{
				refFaceBox = box;
				refFaceIndex = i * 2;
				maxDot = dot;
			}
			if (-dot > maxDot)
			{
				refFaceBox = box;
				refFaceIndex = (i * 2) + 1;
				maxDot = -dot;
			}
		}
	}
}



void CollisionDetector::DetectCollision(CollisionPrimitive* one, CollisionPrimitive* two, CollisionData* collisionData)
{
	CollisionPrimitiveType::value_type typeOne = one->GetType();
	CollisionPrimitiveType::value_type typeTwo = two->GetType();

	// Call the correct collision detection function for the pair of primitives.
	if (typeOne == CollisionPrimitiveType::k_sphere &&
		typeTwo == CollisionPrimitiveType::k_sphere)
	{
		CollideSphereAndSphere(
			*((CollisionSphere*) one),
			*((CollisionSphere*) two),
			collisionData);
	}
	else if (typeOne == CollisionPrimitiveType::k_box &&
			typeTwo == CollisionPrimitiveType::k_sphere)
	{
		CollideBoxAndSphere(
			*((CollisionBox*) one),
			*((CollisionSphere*) two),
			collisionData);
	}
	else if (typeOne == CollisionPrimitiveType::k_sphere &&
			typeTwo == CollisionPrimitiveType::k_box)
	{
		CollideBoxAndSphere(
			*((CollisionBox*) two),
			*((CollisionSphere*) one),
			collisionData);
	}
	else if (typeOne == CollisionPrimitiveType::k_box &&
			typeTwo == CollisionPrimitiveType::k_box)
	{
		CollideBoxAndBox(
			*((CollisionBox*) one),
			*((CollisionBox*) two),
			collisionData);
	}
	else
	{
		// TODO: Support other primitives.
	}
}


unsigned int CollisionDetector::CollideSphereAndSphere(
	const CollisionSphere& one,
	const CollisionSphere& two,
	CollisionData* collisionData)
{
	Vector3f positionOne = one.body->m_position;
	Vector3f positionTwo = two.body->m_position;

	Vector3f oneToTwo = positionTwo - positionOne;
	float distSqr = oneToTwo.LengthSquared();
	float radiusSum = one.radius + two.radius;

	// Check if they are colliding.
	if (distSqr > radiusSum * radiusSum)
		return 0;

	float dist = Math::Sqrt(distSqr);

	// Create the contact
	Contact* contact = collisionData->AddContact();
	if (dist > 0.00001f)
		contact->contactNormal = -oneToTwo / dist;
	else
		contact->contactNormal = Vector3f::UNITX;
	contact->contactPoint = positionOne + (oneToTwo * 0.5f);
	contact->penetration = radiusSum - dist;
	contact->body[0] = one.body;
	contact->body[1] = two.body;
	return 1;
}


unsigned int CollisionDetector::CollideBoxAndSphere(
	const CollisionBox& box,
	const CollisionSphere& sphere,
	CollisionData* collisionData)
{
	// Convert sphere center point to box coordinate space.
	Vector3f center = box.TransformWorldToLocal(sphere.GetWorldPosition());
	
	// Clamp the center position to the box.
	Vector3f closestPoint;
	closestPoint.x = Math::Clamp(center.x, -box.halfSize.x, box.halfSize.x);
	closestPoint.y = Math::Clamp(center.y, -box.halfSize.y, box.halfSize.y);
	closestPoint.z = Math::Clamp(center.z, -box.halfSize.z, box.halfSize.z);

	float distSqr = center.DistToSqr(closestPoint);
	if (distSqr > sphere.radius * sphere.radius)
		return 0;
	
	float dist = Math::Sqrt(distSqr);

	Vector3f closestPointWorld = box.TransformLocalToWold(closestPoint);

	// Create the contact.
	Contact* contact = collisionData->AddContact();
	contact->contactNormal = closestPointWorld - sphere.GetWorldPosition();
	contact->contactNormal.Normalize();
	contact->contactPoint = closestPointWorld;
	contact->penetration = sphere.radius - dist;
	contact->body[0] = box.body;
	contact->body[1] = sphere.body;
	return 1;
}

unsigned int CollisionDetector::CollideBoxAndBox(
	const CollisionBox& one,
	const CollisionBox& two,
	CollisionData* collisionData)
{
	Vector3f axes[15];

	//-----------------------------------------------------------------------------
	// Create a list of axes to perform the SAT on.

	// Face axes from object one.
	axes[0] = one.GetAxis(0);
	axes[1] = one.GetAxis(1);
	axes[2] = one.GetAxis(2);

	// Face axes from object two.
	axes[3] = two.GetAxis(0);
	axes[4] = two.GetAxis(1);
	axes[5] = two.GetAxis(2);

	// Edge-edge axes.
	axes[6] = one.GetAxis(0).Cross(two.GetAxis(0));
	axes[7] = one.GetAxis(0).Cross(two.GetAxis(1));
	axes[8] = one.GetAxis(0).Cross(two.GetAxis(2));
	axes[9] = one.GetAxis(1).Cross(two.GetAxis(0));
	axes[10] = one.GetAxis(1).Cross(two.GetAxis(1));
	axes[11] = one.GetAxis(1).Cross(two.GetAxis(2));
	axes[12] = one.GetAxis(2).Cross(two.GetAxis(0));
	axes[13] = one.GetAxis(2).Cross(two.GetAxis(1));
	axes[14] = one.GetAxis(2).Cross(two.GetAxis(2));

	//-----------------------------------------------------------------------------
	// Perform the Seperating Axis Test for each axis.

	float smallestOverlap = FLT_MAX;
	unsigned int smallestOverlapAxis;
	float minOne, maxOne, minTwo, maxTwo;
	Vector3f axis;

	for (unsigned int i = 0; i < 15; ++i)
	{
		if (axes[i].LengthSquared() < 0.001f)
			continue;
		axes[i].Normalize();
		axis = axes[i];

		// Project each box onto the axis and determine the overlap.
		one.ProjectOntoAxis(axis, minOne, maxOne);
		two.ProjectOntoAxis(axis, minTwo, maxTwo);
		float overlap = Math::Min(maxOne - minTwo, maxTwo - minOne);
		
		// If there is no overlap, then the two boxes are not colliding.
		if (overlap < 0.0f)
			return 0;

		// Else, keep track of the smallest overlap.
		if (overlap < smallestOverlap)
		{
			smallestOverlap = overlap;
			smallestOverlapAxis = i;
		}
	}

	axis = axes[smallestOverlapAxis];

	//-----------------------------------------------------------------------------
	// The SAT detected inter-penetration on an axis.
	// Handle the two collision cases of Face-Vertex and Edge-Edge.

	// Face-vertex contact.
	if (smallestOverlapAxis < 6)
	{
		/*
		// Determine which face is in contact.
		if (axis.Dot(toCenter) > 0.0f)
			axis = -axis;

		const CollisionBox* otherBox = &two;
		if (smallestOverlapAxis >= 3)
			otherBox = &one;

		// Which vertex is in contact?
		Vector3f vertex = otherBox->halfSize;
		if (smallestOverlapAxis >= 3)
			vertex = -vertex;
		if (otherBox->GetAxis(0).Dot(axis) < 0.0f) vertex.x = -vertex.x;
		if (otherBox->GetAxis(1).Dot(axis) < 0.0f) vertex.y = -vertex.y;
		if (otherBox->GetAxis(2).Dot(axis) < 0.0f) vertex.z = -vertex.z;
		
		// Transform the vertex from body space to world space.
		vertex = otherBox->offset.TransformAffine(vertex);
		vertex = otherBox->body->m_bodyToWorld.TransformAffine(vertex);

		ContactData* contact = &collisionData->contacts[collisionData->numContacts].contactData;
		contact->contactNormal = axis;
		contact->contactPoint = vertex;
		contact->penetration = smallestOverlap;
		collisionData->numContacts++;
		
		if (smallestOverlapAxis >= 3)
		{
			RigidBody* temp = collisionData->firstBody;
			collisionData->firstBody = collisionData->secondBody;
			collisionData->secondBody = temp;
		}*/
		
		//-----------------------------------------------------------------------------
		// 1. Identify the reference face.

		const CollisionBox* refBox = (smallestOverlapAxis < 3 ? &one : &two);
		const CollisionBox* incBox = (smallestOverlapAxis < 3 ? &two : &one);

		Vector3f incPosToRefPos =
			incBox->TransformLocalToWold(Vector3f::ZERO) -
			refBox->TransformLocalToWold(Vector3f::ZERO);
		
		unsigned int refFaceIndex = (smallestOverlapAxis % 3) * 2;
		Vector3f refFaceNormal = axis;
		if (refFaceNormal.Dot(incPosToRefPos) < 0.0f)
		{
			refFaceNormal = -refFaceNormal;
			refFaceIndex += 1;
		}

		//-----------------------------------------------------------------------------
		// 2. Find the most anti-parallel face on the other shape (this defines the incident face)

		unsigned int incFaceIndex = 0;
		Vector3f incFaceNormal;
		float minDot = FLT_MAX;
		for (unsigned int i = 0; i < 3; i++)
		{
			Vector3f faceAxis = incBox->GetAxis(i);
			float dot = faceAxis.Dot(refFaceNormal);
			if (dot < minDot)
			{
				minDot = dot;
				incFaceIndex = i * 2;
				incFaceNormal = faceAxis;
			}
			if (-dot < minDot)
			{
				minDot = -dot;
				incFaceIndex = (i * 2) + 1;
				incFaceNormal = -faceAxis;
			}
		}

		//-----------------------------------------------------------------------------
		// 3. Clip incident face against the side planes of reference face

		// Create a list of the incident face vertices.
		std::vector<Vector3f> vertices;
		unsigned int a1 = ((incFaceIndex / 2) + 1) % 3;
		unsigned int a2 = (a1 + 1) % 3;
		Vector3f vertex = incBox->halfSize;
		if (incFaceIndex % 2 == 1)
			vertex = -vertex;
		vertices.push_back(vertex);
		vertex[a1] = -vertex[a1];
		vertices.push_back(vertex);
		vertex[a2] = -vertex[a2];
		vertices.push_back(vertex);
		vertex[a1] = -vertex[a1];
		vertices.push_back(vertex);
		
		// Put vertices into world space.
		for (unsigned int i = 0; i < vertices.size(); i++)
			vertices[i] = incBox->TransformLocalToWold(vertices[i]);
		
		// Perform Sutherland Hodgeman clipping of the vertices
		// against the perpendicular reference face planes.
		// https://gamedevelopment.tutsplus.com/tutorials/understanding-sutherland-hodgman-clipping-for-physics-engines--gamedev-11917
		for (unsigned int i = 0; i < 4; i++)
		{
			unsigned int clipPlaneAxis = ((refFaceIndex / 2) + 1 + (i / 2)) % 3;
			Plane clipPlane = refBox->GetPlane(clipPlaneAxis, i % 2 == 1);
			
			std::vector<Vector3f> newVertices;

			for (unsigned int j = 0; j < vertices.size(); j++)
			{
				Vector3f startPoint = vertices[j];
				Vector3f endPoint = vertices[(j + 1) % vertices.size()];
				Vector3f intersection;
				bool startPointInBack = (clipPlane.GetSide(startPoint) == SIDE_BACK);
				bool endPointInBack = (clipPlane.GetSide(endPoint) == SIDE_BACK);

				Ray ray(startPoint, Vector3f::Normalize(endPoint - startPoint));
				clipPlane.CastRay(ray, PLANE_SIDE_BOTH, intersection);
				
				if (startPointInBack && endPointInBack)
				{
					newVertices.push_back(endPoint);
				}
				else if (startPointInBack)
				{
					newVertices.push_back(intersection);
				}
				else if (endPointInBack)
				{
					newVertices.push_back(intersection);
					newVertices.push_back(endPoint);
				}
			}

			vertices = newVertices;
		}

		//-----------------------------------------------------------------------------
		// 4. Keep all vertices below reference face

		Plane refFacePlane = refBox->GetPlane(refFaceIndex / 2, (refFaceIndex % 2 == 1));
		unsigned int numContacts = 0;;

		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			// Discard any vertices above the reference face (not penetrating).
			if (refFacePlane.GetSide(vertices[i]) == SIDE_FRONT)
				continue;

			// Add this vertex as a contact in the collision.
			Contact* contact = collisionData->AddContact();
			contact->contactNormal = refFaceNormal;
			contact->contactPoint = vertices[i];
			contact->penetration = smallestOverlap;
			contact->contactType = ContactType::k_vertex_face;
			contact->features.faceIndex[0] = incFaceIndex;
			contact->features.faceIndex[1] = refFaceIndex;
			contact->body[0] = incBox->body;
			contact->body[1] = refBox->body;
			numContacts++;
		}
		
		// Make sure the incident body is the first one in the collision.
		if (smallestOverlapAxis < 3)
		{
			RigidBody* temp = collisionData->firstBody;
			collisionData->firstBody = collisionData->secondBody;
			collisionData->secondBody = temp;
		}

		return numContacts;
	}
	else // Edge-Edge contact
	{
		axis = axes[smallestOverlapAxis];
		axis.Normalize();

		// Check which object is further along the axis than the other,
		// and make sure the axis is facing toward object one.
		one.ProjectOntoAxis(axis, minOne, maxOne);
		two.ProjectOntoAxis(axis, minTwo, maxTwo);
		float centerOne = (minOne + maxOne) * 0.5f;
		float centerTwo = (minTwo + maxTwo) * 0.5f;
		if (centerOne < centerTwo)
			axis = -axis;

		Vector3f pointOnEdgeOne = one.halfSize;
		Vector3f pointOnEdgeTwo = two.halfSize;
		unsigned int oneEdgeVertex1 = 0;
		unsigned int oneEdgeVertex2 = 0;
		unsigned int twoEdgeVertex1 = 0;
		unsigned int twoEdgeVertex2 = 0;
		unsigned int oneAxisIndex = (smallestOverlapAxis - 6) / 3;
		unsigned int twoAxisIndex = (smallestOverlapAxis - 6) % 3;

		// Find the two edges that are colliding.
		for (unsigned int i = 0; i < 3; i++)
		{
			if (i == oneAxisIndex)
			{
				pointOnEdgeOne[i] = 0;
				oneEdgeVertex2 |= 1 << i;
			}
			else if (one.GetAxis(i).Dot(axis) > 0)
			{
				pointOnEdgeOne[i] = -pointOnEdgeOne[i];
				oneEdgeVertex1 |= 1 << i;
				oneEdgeVertex2 |= 1 << i;
			}
			
			if (i == twoAxisIndex)
			{
				pointOnEdgeTwo[i] = 0;
				twoEdgeVertex2 |= 1 << i;
			}
			else if (two.GetAxis(i).Dot(axis) < 0)
			{
				pointOnEdgeTwo[i] = -pointOnEdgeTwo[i];
				twoEdgeVertex1 |= 1 << i;
				twoEdgeVertex2 |= 1 << i;
			}
		}
		
		// Transform points from body space to world space.
		pointOnEdgeOne = one.TransformLocalToWold(pointOnEdgeOne);
		pointOnEdgeTwo = two.TransformLocalToWold(pointOnEdgeTwo);

		// Determine the contact point (the closest point between the two edges).
		Vector3f axisOne = one.GetAxis(oneAxisIndex);
		Vector3f axisTwo = two.GetAxis(twoAxisIndex);
		Vector3f contactPoint = GetEdgeContactPoint(
			axisOne, axisTwo, pointOnEdgeOne, pointOnEdgeTwo);

		// Create the contact.
		Contact* contact = collisionData->AddContact();
		contact->contactNormal = axis;
		contact->contactPoint = contactPoint;
		contact->penetration = smallestOverlap;
		contact->contactType = ContactType::k_edge_edge;
		contact->features.edges[0].vertexIndex[0] = oneEdgeVertex1;
		contact->features.edges[0].vertexIndex[1] = oneEdgeVertex2;
		contact->features.edges[1].vertexIndex[0] = twoEdgeVertex1;
		contact->features.edges[1].vertexIndex[1] = twoEdgeVertex2;
		contact->body[0] = one.body;
		contact->body[1] = two.body;
		return 1;
	}

	return 0;
}



Vector3f CollisionDetector::GetEdgeContactPoint(
	const Vector3f& axisOne,
	const Vector3f& axisTwo,
	const Vector3f& pointOnEdgeOne,
	const Vector3f& pointOnEdgeTwo)
{	
	// The vector between the test points on each edge.
	Vector3f toSt = pointOnEdgeOne - pointOnEdgeTwo;
				
	// How much of those vectors are in the direction of each edge?
	float dpStaOne = axisOne.Dot(toSt);
	float dpStaTwo = axisTwo.Dot(toSt);

	// Work out how far along each edge is the closest point.
	float smOne = axisOne.LengthSquared();
	float smTwo = axisTwo.LengthSquared();
	float dotProductEdges = axisTwo.Dot(axisOne);
	float denom = smOne * smTwo - dotProductEdges * dotProductEdges;
	CMG_ASSERT(denom != 0.0f);
	float a = (dotProductEdges * dpStaTwo - smTwo * dpStaOne) / denom;
	float b = (smOne * dpStaTwo - dotProductEdges * dpStaOne) / denom;

	// Use a point midway between the two nearest points.
	Vector3f nearestPointOnOne = pointOnEdgeOne + axisOne * a;
	Vector3f nearestPointOnTwo = pointOnEdgeTwo + axisTwo * b;
	return (nearestPointOnOne + nearestPointOnTwo) * 0.5f;
}

