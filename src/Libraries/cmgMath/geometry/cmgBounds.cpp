#include "cmgBounds.h"
#include <cmgMath/geometry/cmgRay.h>
#include <cmgMath/cmgMathLib.h>


// ================== CONSTRUCTORS ================== //

// Construct a bounding-box with uninitialized components.
Bounds::Bounds()
{
}

// Creates new Bounds with a given min and max points.
Bounds::Bounds(const Vector3f& mins, const Vector3f& maxs) :
	mins(mins), maxs(maxs)
{
}


// =================== ACCESSORS =================== //

// Is point contained in the bounding-box?
bool Bounds::Contains(const Vector3f& point) const
{
	return (point.x >= mins.x && point.x < maxs.x &&
			point.y >= mins.y && point.y < maxs.y &&
			point.z >= mins.z && point.z < maxs.z);
}

// Does another bounding-box intersect with this bounding-box?
bool Bounds::Intersects(const Bounds& bounds) const
{
	return !(bounds.mins.x - maxs.x >= 0.0f || mins.x - bounds.maxs.x >= 0.0f ||
				bounds.mins.y - maxs.y >= 0.0f || mins.y - bounds.maxs.y >= 0.0f ||
				bounds.mins.z - maxs.y >= 0.0f || mins.z - bounds.maxs.z >= 0.0f);
}

// Does ray intersect this bounding-box?
bool Bounds::IntersectsRay(const Ray& ray) const
{
	return CastRayAtBox(mins, maxs, ray.origin, ray.direction, Vector3f());
}

// Cast a ray at this bounding-box, and output the distance
// from the ray to the point of intersection.
bool Bounds::CastRay(const Ray& ray, float& distance) const
{
	Vector3f intersection;
	if (!CastRay(ray, intersection))
		return false;
	distance = Vector3f::Dot(intersection - ray.origin, ray.direction);
	return true;
}

// Cast a ray at this bounding-box, and output the distance
// from the ray to the point of intersection.
bool Bounds::CastRay(const Ray& ray, Vector3f& intersection) const
{
	return CastRayAtBox(mins, maxs, ray.origin, ray.direction, intersection);
}


// The smallest distance between the point and this bounding box.
float Bounds::DistToPoint(const Vector3f& point) const
{
	return ClampPoint(point).DistTo(point);
}

// The smallest squared distance between the point and this bounding box.
float Bounds::DistSqrToPoint(const Vector3f& point) const
{
	return ClampPoint(point).DistToSqr(point);
}

Vector3f Bounds::ClampPoint(const Vector3f& point) const
{
	return Vector3f(
		Math::Clamp(point.x, mins.x, maxs.x),
		Math::Clamp(point.y, mins.y, maxs.y),
		Math::Clamp(point.z, mins.z, maxs.z));
}



// ==================== MUTATORS ==================== //

Bounds& Bounds::SetMinMax(const Vector3f& mins, const Vector3f& maxs)
{
	this->mins = mins;
	this->maxs = maxs;
	return *this;
}

// Creates new Bounds with a given center and total size. Bound extents will be half the given size.
Bounds& Bounds::SetCenterSize(const Vector3f& center, const Vector3f& size)
{
	mins = center - (size * 0.5f);
	maxs = center + (size * 0.5f);
	return *this;
}

// Set the bounds to contain a single point with no size.
Bounds& Bounds::SetAsPoint(const Vector3f& point)
{
	mins = point;
	maxs = point;
	return *this;
}

// Expand the bounds by increasing its size by amount along each side.
Bounds& Bounds::Expand(float amount)
{
	mins.x -= amount;
	mins.y -= amount;
	mins.z -= amount;
	maxs.x += amount;
	maxs.y += amount;
	maxs.z += amount;
	return *this;
}

// Expand the bounds by increasing its size by amount along each side.
Bounds& Bounds::Expand(const Vector3f& amount)
{
	mins -= amount;
	maxs += amount;
	return *this;
}

// Translates the position of the bounding-box by amount.
Bounds& Bounds::Translate(const Vector3f& amount)
{
	mins += amount;
	maxs += amount;
	return *this;
}

// Grows the Bounds to include the point.
Bounds& Bounds::Encapsulate(const Vector3f& point)
{
	if (point.x < mins.x)
		mins.x = point.x;
	else if (point.x > maxs.x)
		maxs.x = point.x;

	if (point.y < mins.y)
		mins.y = point.y;
	else if (point.y > maxs.y)
		maxs.y = point.y;

	if (point.z < mins.z)
		mins.z = point.z;
	else if (point.z > maxs.z)
		maxs.z = point.z;

	return *this;
}

// Combine this bounds with another (this is a union operation).
Bounds& Bounds::Combine(const Bounds& bounds)
{
	Encapsulate(bounds.mins);
	Encapsulate(bounds.maxs);
	return *this;
}

Bounds Bounds::Union(const Bounds & left, const Bounds & right)
{
	Bounds result = left;
	return result.Combine(right);
}

bool Bounds::Intersect(Bounds& outIntersection, const Bounds & left, const Bounds & right)
{
	if (left.Intersects(right))
	{
		for (uint32 axis = 0; axis < 3; axis++)
		{
			outIntersection.mins[axis] = Math::Max(
				left.mins[axis], right.mins[axis]);
			outIntersection.maxs[axis] = Math::Min(
				left.maxs[axis], right.maxs[axis]);
		}
		return true;
	}
	return false;
}



//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Purpose: Fast Ray-Box intersection.
//-----------------------------------------------------------------------------
// By Andrew Woo, from "Graphics Gems", Academic Press, 1990
// http://tog.acm.org/resources/GraphicsGems/gems/RayBox.c
//-----------------------------------------------------------------------------
bool Bounds::CastRayAtBox(const Vector3f& boxMin, const Vector3f& boxMax,
		const Vector3f& rayOrigin, const Vector3f& rayDir, Vector3f& hit)
{
	#define NUMDIM	3
	#define RIGHT	0
	#define LEFT	1
	#define MIDDLE	2

	bool	isInside = true;
	int		whichPlane;
	char	quadrant[NUMDIM];
	float	maxT[NUMDIM];
	float	candidatePlane[NUMDIM];
	int	i;

	// Find candidate planes; this loop can be avoided if
   	// rays cast all from the eye (assume perpsective view).
	for (i = 0; i < NUMDIM; i++)
	{
		if (rayOrigin[i] < boxMin[i])
		{
			quadrant[i]       = LEFT;
			candidatePlane[i] = boxMin[i];
			isInside          = false;
		}
		else if (rayOrigin[i] > boxMax[i])
		{
			quadrant[i]       = RIGHT;
			candidatePlane[i] = boxMax[i];
			isInside          = false;
		}
		else
		{
			quadrant[i] = MIDDLE;
		}
	}

	// Ray rayOrigin inside bounding box.
	if (isInside)
	{
		hit = rayOrigin;
		return true;
	}

	// Calculate T distances to candidate planes.
	for (i = 0; i < NUMDIM; i++)
	{
		if (quadrant[i] != MIDDLE && rayDir[i] != 0.0f)
			maxT[i] = (candidatePlane[i] - rayOrigin[i]) / rayDir[i];
		else
			maxT[i] = -1.0f;
	}

	// Get largest of the maxT's for final choice of intersection.
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
	{
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	// Check final candidate actually inside box.
	if (maxT[whichPlane] < 0.0f)
		return false;

	for (i = 0; i < NUMDIM; i++)
	{
		if (whichPlane != i)
		{
			hit[i] = rayOrigin[i] + (maxT[whichPlane] * rayDir[i]);
			if (hit[i] < boxMin[i] || hit[i] > boxMax[i])
				return false;
		}
		else
		{
			hit[i] = candidatePlane[i];
		}
	}

	return true; // ray hits box.
}

