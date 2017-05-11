#include "cmgGJK.h"
#include <cmgCore/cmgAssert.h>

// http://www.cs.sjsu.edu/faculty/pollett/masters/Semesters/Spring12/josh/GJK.html
// https://www.youtube.com/watch?v=Qupqu1xe7Io


Simplex::Simplex() :
	m_numPoints(0)
{
}

void Simplex::Clear()
{
	m_numPoints = 0;
}

void Simplex::Add(const Vector3f& point)
{
	// Shift existing points up.
	for (unsigned int i = m_numPoints; i > 0; --i)
		m_points[i] = m_points[i - 1];

	// Insert new point at beginning.
	m_points[0] = point;

	++m_numPoints;
}

void Simplex::Remove(int index)
{
	// Shift points down.
	--m_numPoints;
	for (unsigned int i = index; i < m_numPoints; ++i)
		m_points[i] = m_points[i + 1];
}

bool GJK::TestIntersection(
	Collider* shapeA,
	Collider* shapeB,
	Simplex* outSimplex)
{
	Simplex simplex;
	Vector3f direction = Vector3f(1, 1, 1);

	// Add the first support point to the simplex.
	Vector3f nextPoint = GJK::GetSupportMinkowskiDiff(
		direction, shapeA, shapeB);
	simplex.Add(nextPoint);
	direction = -nextPoint; // Search towards the origin.
	bool intersecting = false;

	unsigned int maxIterations = 100; // TODO: fix infinite loops.
	unsigned int i;

	// GJK loop
	for (i = 0; i < maxIterations; ++i)
	{
		// Get the next support point.
		nextPoint = GJK::GetSupportMinkowskiDiff(
			direction, shapeA, shapeB);
		if (nextPoint.Dot(direction) < 0.0f)
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


Vector3f GJK::GetSupportMinkowskiDiff(const Vector3f& direction,
	Collider* shapeA, Collider* shapeB)
{
	return (shapeA->GetSupportPoint(direction) - shapeB->GetSupportPoint(-direction));
}


// Evolve the simplex.
bool GJK::DoSimplex(Simplex& simplex, Vector3f& direction)
{
	Vector3f a = simplex.m_points[0];
	Vector3f b = simplex.m_points[1];
	Vector3f c = simplex.m_points[2];
	Vector3f d = simplex.m_points[3];
	Vector3f ab = b - a;
	Vector3f ac = c - a;
	Vector3f ad = d - a;
	Vector3f ao = -a;
	Vector3f abc = -ab.Cross(ac);
	Vector3f acd = -ac.Cross(ad);
	Vector3f adb = -ad.Cross(ab);

	//-------------------------------------------------------------------------
	// LINE CASE
	if (simplex.m_numPoints == 2)
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
	else if (simplex.m_numPoints == 3)
	{
		if (ao.Dot(abc.Cross(ab)) > 0.0f)
		{
			simplex.Remove(2); // Remove C
			return DoSimplex(simplex, direction);

			//if (ao.Dot(ab) > 0.0f)
			//{
			//	// Closest to A
			//	direction = ao;
			//	simplex.Remove(1); // Remove B
			//}
			//else
			//{
			//	// Closest to edge AB
			//	direction = ab.Cross(ao).Cross(ab);
			//}
		}
		else if (ao.Dot(abc.Cross(-ac)) > 0.0f)
		{
			simplex.Remove(1); // Remove B
			return DoSimplex(simplex, direction);
			
			//if (ao.Dot(ac) > 0.0f)
			//{
			//	// Closest to A
			//	direction = ao;
			//	simplex.Remove(1); // Remove C
			//}
			//else
			//{
			//	// Closest to edge AC
			//	direction = ac.Cross(ao).Cross(ac);
			//}
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
	else if (simplex.m_numPoints == 4)
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

