#ifndef _PLANE_H_
#define _PLANE_H_

#include <cmgMath/types/cmgVector3f.h>

struct Bounds;
struct Ray;

typedef int SideType;


// Used to represent sides of things like planes.
#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2

#define PLANE_SIDE_EPSILON	0.01f


enum PlaneSide
{
	PLANE_SIDE_FRONT = 0,
	PLANE_SIDE_BACK  = 1,
	PLANE_SIDE_BOTH  = 2,
};


// 3D floating-point plane in a normal-distance equation
// format [ax + by + cz = d], in which (a, b, c) specify
// the normal, and d is the distance.
struct Plane
{
	Vector3f normal;	// Normal vector of the plane.
	float  distance;	// Distance along the normal from the origin to the plane.
	

	// Constants.
	static const Plane YZ; // The YZ plane at the origin (nomal is in +X direction).
	static const Plane XZ; // The XZ plane at the origin (nomal is in +Y direction).
	static const Plane XY; // The XY plane at the origin (nomal is in +Z direction).

	// Constructors.
	Plane();
	Plane(const Vector3f& vNormal, float distance);
	Plane(const Vector3f& vNormal, const Vector3f& vPosition);
	Plane(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3);

	// Accessors.
	float		DistanceTo(const Vector3f& point) const;
	SideType	GetSide(const Vector3f& vPoint, float sideEpsilon = PLANE_SIDE_EPSILON) const;
	SideType	GetPointSideExact(const Vector3f& vPoint) const;
	Vector3f	GetPointOnPlane() const;
	SideType	GetBoxSide(const Bounds& box) const;
	SideType	GetBoxSide(const Vector3f& vMin, const Vector3f& vMax) const;
	Plane		GetFlipped() const;
	bool		CastRay(const Ray& ray, float& distance) const;
	bool		CastRay(const Ray& ray, Vector3f& intersection) const;
	bool		CastRay(const Ray& ray, PlaneSide side, float& distance) const;
	bool		CastRay(const Ray& ray, PlaneSide side, Vector3f& intersection) const;

	// Mutators.
	Plane& SetNormal(float x, float y, float z);
	Plane& SetPosition(const Vector3f& position);
	Plane& Set(const Vector3f& vNormal, float distance);
	Plane& Set(const Vector3f& vNormal, const Vector3f& vPosition);
	Plane& Set3Points(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3);
	Plane& Flip();
};


#endif // _PLANE_H_