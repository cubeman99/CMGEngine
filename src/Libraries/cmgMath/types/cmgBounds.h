#ifndef _BOUNDS_H_
#define _BOUNDS_H_

#include <cmgMath/types/cmgVector3f.h>

struct Ray;


// Axis-aligned, 3D floating-point rectangle
// stored with min and max points.
struct Bounds
{
	Vector3f mins, maxs;

	// Constructors.
	Bounds();
	Bounds(const Vector3f& mins, const Vector3f& maxs);

	// Accessors.
	inline float	GetVolume()		const { return (maxs.x - mins.x) * (maxs.y - mins.y) * (maxs.z - mins.z); }
	inline Vector3f GetCenter()		const { return (mins + maxs) * 0.5f; }
	inline Vector3f GetSize()		const { return (maxs - mins); }
	inline Vector3f GetExtents()	const { return (maxs - mins) * 0.5f; }
	bool			Contains(const Vector3f& point) const;
	bool			Intersects(const Bounds& bounds) const;
	bool			IntersectsRay(const Ray& ray) const;
	bool			CastRay(const Ray& ray, float& distance) const;
	bool			CastRay(const Ray& ray, Vector3f& intersection) const;
	float			GetSqrDistance(const Vector3f& point) const;

	// Mutators.
	Bounds& SetMinMax(const Vector3f& mins, const Vector3f& maxs);
	Bounds& SetCenterSize(const Vector3f& center, const Vector3f& size);
	Bounds& SetAsPoint(const Vector3f& point);
	Bounds& Expand(float amount);
	Bounds& Expand(const Vector3f& amount);
	Bounds& Translate(const Vector3f& amount);
	Bounds& Encapsulate(const Vector3f& point);
	Bounds& Combine(const Bounds& bounds);
	
private:
	// Private static methods.
	static bool CastRayAtBox(const Vector3f& boxMin, const Vector3f& boxMax,
		const Vector3f& rayOrigin, const Vector3f& rayDir, Vector3f& intersection);
};


#endif // _BOUNDS_H_