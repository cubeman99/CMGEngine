#ifndef _CMG_PHYSICS_COLLIDERS_CONE_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_CONE_COLLIDER_H_

#include <cmgPhysics/colliders/cmgCollider.h>


class ConeCollider : public Collider
{
public:
	ConeCollider(float radius, float height, const Matrix4f& offset = Matrix4f::IDENTITY);
	
	// Collider implementations
	float GetVolume() const override;
	virtual Vector3f GetCenterOfMassOffset() const;
	Matrix3f CalcInertiaTensor(float mass) const override;
	Vector3f GetSupportPoint(const Vector3f& direction) const override;

	inline float GetRadius() const { return m_radius; }
	inline float GetHeight() const { return m_height; }

private:

	float m_radius;
	float m_height;

	float m_sideLength;
};


#endif // _CMG_PHYSICS_COLLIDERS_CONE_COLLIDER_H_