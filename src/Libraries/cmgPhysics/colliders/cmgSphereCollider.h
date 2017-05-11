#ifndef _CMG_PHYSICS_COLLIDERS_SPHERE_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_SPHERE_COLLIDER_H_

#include <cmgPhysics/colliders/cmgCollider.h>


class SphereCollider : public Collider
{
public:
	SphereCollider(float radius, const Matrix4f& offset = Matrix4f::IDENTITY);
	
	// Collider implementations
	float GetVolume() const override;
	Matrix3f CalcInertiaTensor(float mass) const override;
	Vector3f GetSupportPoint(const Vector3f& direction) const override;

	inline float GetRadius() const { return m_radius; }

private:

	float m_radius;
};


#endif // _CMG_PHYSICS_COLLIDERS_SPHERE_COLLIDER_H_