#ifndef _CMG_RAY_H_
#define _CMG_RAY_H_

#include <cmgMath/types/cmgVector3f.h>


// 3D ray with a direction and origin.
struct Ray
{
	Vector3f direction;
	Vector3f origin;
	
	// Constants
	static const Ray ORIGINX; // A ray at the origin pointing in the +X direction.
	static const Ray ORIGINY; // A ray at the origin pointing in the +Y direction.
	static const Ray ORIGINZ; // A ray at the origin pointing in the +Z direction.

	// Constructors
	Ray();
	Ray(const Vector3f& origin, const Vector3f& direction);

	// Getters
	Vector3f GetPoint(float distance) const;

	// Setters
	Ray& SetDirection(float x, float y, float z);
	Ray& SetOrigin(float x, float y, float z);
	Ray& Set(const Vector3f& origin, const Vector3f& direction);
};


#endif // _CMG_RAY_H_