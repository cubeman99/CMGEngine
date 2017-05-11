#ifndef _CMG_PHYSICS_COLLIDERS_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_COLLIDER_H_

#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/cmgMathLib.h>

class RigidBody;


//-----------------------------------------------------------------------------
// ColliderType
//-----------------------------------------------------------------------------
enum class ColliderType
{
	k_unknown = -1,

	k_sphere = 0,
	k_box,
	k_plane,

	k_cylinder,
	k_cone,
	k_capsule,
	k_ellipsoid,

	k_count,
};


//-----------------------------------------------------------------------------
// Collider
//-----------------------------------------------------------------------------
class Collider
{
public:
	friend class RigidBody;

public:
	Collider(ColliderType type, const Matrix4f& offset = Matrix4f::IDENTITY);
	
	// Getters
	inline ColliderType GetType() const { return m_type; }
	inline const Matrix4f& GetShapeToBody() const { return m_shapeToBody; }
	inline const Matrix4f& GetBodyToShape() const { return m_bodyToShape; }
	inline const Matrix4f& GetShapeToWorld() const { return m_shapeToWorld; }
	inline const Matrix4f& GetWorldToShape() const { return m_worldToShape; }
	inline RigidBody* GetBody() { return m_body; }
	inline const RigidBody* GetBody() const { return m_body; }
	
	// Virtual functions
	virtual Vector3f GetCenterOfMassOffset() const { return Vector3f::ZERO; }
	virtual float GetVolume() const { return 0.0f; }
	virtual Matrix3f CalcInertiaTensor(float mass) const { return Matrix3f::IDENTITY; }
	virtual Vector3f GetSupportPoint(const Vector3f& direction) const { return Vector3f::ZERO; }


	void CalcDerivedData();


protected:

	// The type of collider (box, sphere, capsule, etc.).
	ColliderType m_type;

	// The rigid body this collider is a part of.
	RigidBody* m_body;
	
	// The offset of this shape from the body.
	// Transforms from local primitive space to body space.
	Matrix4f m_shapeToBody;

	// Transforms from body space to local primitive space.
	Matrix4f m_bodyToShape;


	Matrix4f m_shapeToWorld;
	Matrix4f m_worldToShape;
};


#endif // _CMG_PHYSICS_COLLIDERS_COLLIDER_H_