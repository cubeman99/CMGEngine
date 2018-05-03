#include "cmgPlane.h"
#include <cmgMath/types/cmgBounds.h>
#include <cmgMath/types/cmgRay.h>
#include <cmgMath/cmgMathLib.h>


//-----------------------------------------------------------------------------
// Plane constants.
//-----------------------------------------------------------------------------

const Plane Plane::YZ(Vector3f(1.0f, 0.0f, 1.0f), 0.0f);
const Plane Plane::XZ(Vector3f(0.0f, 1.0f, 1.0f), 0.0f);
const Plane Plane::XY(Vector3f(0.0f, 0.0f, 1.0f), 0.0f);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a plane with uninitialized components.
Plane::Plane()
{
}

// Construct a plane with the given normal and 
// distance along the normal from the origin.
Plane::Plane(const Vector3f& vNormal, float distance) :
	normal(vNormal),
	distance(distance)
{
}

// Construct a plane with the given normal and 
// distance along the normal from the origin.
Plane::Plane(const Vector3f& vNormal, const Vector3f& vPosition)
{
	Set(vNormal, vPosition);
}

// Construct a plane that passes through three points, specified in
// clockwise order if it were to be looked down at from above.
Plane::Plane(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3)
{
	Set3Points(p1, p2, p3);
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Return the distance of a point to this plane.
float Plane::DistanceTo(const Vector3f& vPoint) const
{
	return (Vector3f::Dot(vPoint, normal) - distance);
}

// Return which of the plane a point is on (SIDE_FRONT, SIDE_BACK, or SIDE_ON).
SideType Plane::GetSide(const Vector3f& vPoint, float sideEpsilon) const
{
	float dist = DistanceTo(vPoint);

	if (dist >= sideEpsilon)
		return SIDE_FRONT;
	else if (dist <= -sideEpsilon)
		return SIDE_BACK;
	else
		return SIDE_ON;
}

// Return which of the plane a point is on (SIDE_FRONT or SIDE_BACK).
SideType Plane::GetPointSideExact(const Vector3f& vPoint) const
{
	return (DistanceTo(vPoint) > 0.0f ? SIDE_FRONT : SIDE_BACK);
}

// Return a point on the plane (normal * distance).
Vector3f Plane::GetPointOnPlane() const
{
	return (normal * distance);
}

// Return which side of the plane a box is on.
SideType Plane::GetBoxSide(const Bounds& bounds) const
{
	return GetBoxSide(bounds.mins, bounds.maxs);
}

// Return which side of the plane a box is on.
SideType Plane::GetBoxSide(const Vector3f& vMin, const Vector3f& vMax) const
{
	// A list of box corners.
	Vector3f vPoints[8] =
	{
		{ vMin.x, vMin.y, vMin.z },
		{ vMin.x, vMin.y, vMax.z },
		{ vMin.x, vMax.y, vMax.z },
		{ vMin.x, vMax.y, vMin.z },

		{ vMax.x, vMin.y, vMin.z },
		{ vMax.x, vMin.y, vMax.z },
		{ vMax.x, vMax.y, vMax.z },
		{ vMax.x, vMax.y, vMin.z },
	};

	// The box corners are either all on one side, or not.
	SideType firstSide = GetPointSideExact(vPoints[0]);
	for (int i = 1; i < 8; ++i)
	{
		// Does the box cross the plane?
		if (GetPointSideExact(vPoints[i]) != firstSide)
			return SIDE_ON;
	}

	// Ok, they're all on the same side, return that.
	return firstSide;
}

// Return the plane if it's sides were flipped.
Plane Plane::GetFlipped() const
{
	return Plane(-normal, -distance);
}

bool Plane::CastRay(const Ray& ray, float& distance) const
{
	return CastRay(ray, PLANE_SIDE_FRONT, distance);
}

bool Plane::CastRay(const Ray& ray, Vector3f& intersection) const
{
	return CastRay(ray, PLANE_SIDE_FRONT, intersection);
}

bool Plane::CastRay(const Ray& ray, PlaneSide side, float& distance) const
{
	float denom = normal.Dot(ray.direction);

	if ((side == PLANE_SIDE_BOTH  && Math::Abs(denom) > 0.0001f) || 
		(side == PLANE_SIDE_FRONT && denom < -0.0001f) || 
		(side == PLANE_SIDE_BACK  && denom > 0.0001f))
	{
		distance = Vector3f::Dot(GetPointOnPlane() - ray.origin, normal) / denom; 
		return (distance >= 0.0f);
	}

	return false;
}

bool Plane::CastRay(const Ray& ray, PlaneSide side, Vector3f& intersection) const
{
	float distance;
	if (!CastRay(ray, side, distance))
		return false;
	intersection = ray.GetPoint(distance);
	return true;
}

bool Plane::CastBoundedRay(const Ray& ray, PlaneSide side, float& inOutDistance) const
{
	float denom = normal.Dot(ray.direction);

	if ((side == PLANE_SIDE_BOTH  && Math::Abs(denom) > 0.0001f) || 
		(side == PLANE_SIDE_FRONT && denom < -0.0001f) || 
		(side == PLANE_SIDE_BACK  && denom > 0.0001f))
	{
		float dist = Vector3f::Dot(GetPointOnPlane() - ray.origin, normal) / denom; 

		if (dist >= 0.0f && dist < inOutDistance)
		{
			inOutDistance = dist;
			return true;
		}
	}

	return false;
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the normal vector of the plane.
Plane& Plane::SetNormal(float x, float y, float z)
{
	normal.Set(x, y, z);
	return *this;
}

// Set this plane to be on a specified point.
Plane& Plane::SetPosition(const Vector3f& vPoint)
{
	distance = Vector3f::Dot(vPoint, normal);
	return *this;
}

// Define the plane from a normal and distance along the normal.
Plane& Plane::Set(const Vector3f& vNormal, float distance)
{
	this->normal = vNormal;
	this->distance = distance;
	return *this;
}

// Define the plane with a normal at a position.
Plane& Plane::Set(const Vector3f& normal, const Vector3f& position)
{
	this->normal = normal;
	this->distance = Vector3f::Dot(position, normal);
	return *this;
}

// Define the plane from 3 points (a clockwise order yields a normal pointing towards the viewer).
Plane& Plane::Set3Points(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3)
{
	// TODO: verify the normal is in the right direction.
	normal = Vector3f::Cross(p3 - p1, p2 - p1);
	normal.Normalize();
	distance = Vector3f::Dot(p1, normal);
	return *this;
}

// Flip the side (The back is now the front and visa versa).
Plane& Plane::Flip()
{
	normal.Negate();
	distance = -distance;
	return *this;
}

