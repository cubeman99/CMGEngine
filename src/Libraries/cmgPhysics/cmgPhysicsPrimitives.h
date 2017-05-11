#ifndef _CMG_PHYSICS_PRIMITIVES_H_
#define _CMG_PHYSICS_PRIMITIVES_H_

#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgQuaternion.h>
#include <cmgMath/types/cmgPlane.h>
#include <cmgCore/containers/cmgArray.h>
#include <cfloat>

class RigidBody;


struct CollisionPrimitiveType
{
	enum
	{
		k_unknown = -1,

		k_sphere = 0,
		k_box,
		k_plane,

		k_cylinder,
		k_capsule,
		k_polyhedra,

		k_count,
	};

	typedef int value_type;
};


//-----------------------------------------------------------------------------
// CollisionPrimitive
//-----------------------------------------------------------------------------
class CollisionPrimitive
{
public:
	typedef CollisionPrimitiveType::value_type primitive_type;

public:
	CollisionPrimitive();

	// Virtual mathods

	virtual primitive_type GetType() const = 0;
	virtual float GetVolume() const { return 0.0f; }
	virtual void CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const {}
	virtual Vector3f GetSupport(const Vector3f& direction) const { return Vector3f::ZERO; }

	// Coordinate space methods

	Vector3f GetWorldPosition() const;
	Vector3f TransformLocalToWold(const Vector3f& localPoint) const;
	Vector3f RotateLocalToWold(const Vector3f& localDirection) const;
	Vector3f TransformWorldToLocal(const Vector3f& localPoint) const;
	Vector3f RotateWorldToLocal(const Vector3f& localDirection) const;
	inline const Matrix4f& GetBodyToShape() const { return bodyToShape; }

public:

	// The body this primitive is a part of.
	RigidBody* body;

	// The offset of this primitive from the body.
	// Transforms from body space to local primitive space.
	Matrix4f bodyToShape;
};


//-----------------------------------------------------------------------------
// CollisionSphere
//-----------------------------------------------------------------------------
class CollisionSphere : public CollisionPrimitive
{
public:
	CollisionSphere() :
		CollisionPrimitive()
	{}
	CollisionSphere(float radius) :
		CollisionPrimitive(),
		radius(radius)
	{}
	
	inline primitive_type GetType() const override
		{ return CollisionPrimitiveType::k_sphere; }
	
	float GetVolume() const override;
	void CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const override;

public:
	float radius;
};


//-----------------------------------------------------------------------------
// CollisionBox
//-----------------------------------------------------------------------------
class CollisionBox : public CollisionPrimitive
{
public:
	inline primitive_type GetType() const override
		{ return CollisionPrimitiveType::k_box; }
	
	CollisionBox() :
		CollisionPrimitive()
	{}
	CollisionBox(const Vector3f& halfSize) :
		CollisionPrimitive(),
		halfSize(halfSize)
	{}
	
	Plane GetPlane(int axisIndex, bool negate) const;
	Vector3f GetAxis(int axisIndex) const;
	Vector3f GetVertex(unsigned int index) const;
	void GetEdge(unsigned int edgeIndex, Vector3f& outEndPoint1, Vector3f& outEndPoint2) const;
	void ProjectOntoAxis(const Vector3f& axis, float& outMin, float& outMax) const;

	float GetVolume() const override;
	void CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const override;
	Vector3f GetSupport(const Vector3f& direction) const override;

public:
	Vector3f halfSize;
};


//-----------------------------------------------------------------------------
// CollisionPlane
//-----------------------------------------------------------------------------
class CollisionPlane : public CollisionPrimitive
{
public:
	CollisionPlane() :
		CollisionPrimitive()
	{}
	CollisionPlane(const Vector3f& normal, float distance) :
		CollisionPrimitive(),
		normal(normal), distance(distance)
	{}
	CollisionPlane(const Plane& plane) :
		CollisionPrimitive(),
		normal(plane.normal), distance(plane.distance)
	{}

	float GetVolume() const override;
	void CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const override;

public:
	Vector3f normal;
	float distance;
};


class CollisionCylinder : public CollisionPrimitive
{
public:
	float radius;
	float height;
};


class CollisionCapsule : public CollisionPrimitive
{
public:
	float radius;
	float height;
};

//-----------------------------------------------------------------------------
// CollisionPolyhedron
//-----------------------------------------------------------------------------
class CollisionPolyhedron : public CollisionPrimitive
{
public:
	CollisionPolyhedron() :
		CollisionPrimitive()
	{}

	float GetVolume() const override;
	void CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const override;

public:
	// List of vertices
	// Each vertex has a list of connected vertices
	//std::vector<Vector3f> vertices;
	//std::vector<std::vector<int>> vertexConnections;
};

#endif // _CMG_PHYSICS_PRIMITIVES_H_