#ifndef _CMG_PHYSICS_COLLIDERS_CYLINDER_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_CYLINDER_COLLIDER_H_

#include <cmgPhysics/colliders/cmgCollider.h>


class CylinderCollider : public Collider
{
public:
	CylinderCollider(float radius, float halfHeight, const Matrix4f& offset = Matrix4f::IDENTITY);
	
	// Collider implementations
	float GetVolume() const override;
	Matrix3f CalcInertiaTensor(float mass) const override;
	Vector3f GetSupportPoint(const Vector3f& direction) const override;

	inline float GetRadius() const { return m_radius; }
	inline float GetHalfHeight() const { return m_halfHeight; }
	inline float GetHeight() const { return (m_halfHeight * 2.0f); }

private:

	float m_radius;
	float m_halfHeight;
};


#endif // _CMG_PHYSICS_COLLIDERS_CYLINDER_COLLIDER_H_