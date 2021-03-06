#ifndef _CMG_PHYSICS_COLLIDERS_BOX_COLLIDER_H_
#define _CMG_PHYSICS_COLLIDERS_BOX_COLLIDER_H_

#include <cmgPhysics/colliders/cmgCollider.h>


class BoxCollider : public Collider
{
public:
	BoxCollider(const Vector3f& halfSize, const Matrix4f& offset = Matrix4f::IDENTITY);
	

	// Collider implementations
	float GetVolume() const override;
	Matrix3f CalcInertiaTensor(float mass) const override;
	Vector3f GetSupportPoint(const Vector3f& direction) const override;

	inline const Vector3f& GetHalfSize() const { return m_halfSize; }
	
	bool CastBoundedRay(const Ray& ray, float& inOutDistance, Vector3f& outNormal) const override;

private:

	Vector3f m_halfSize;
};


#endif // _CMG_PHYSICS_COLLIDERS_BOX_COLLIDER_H_