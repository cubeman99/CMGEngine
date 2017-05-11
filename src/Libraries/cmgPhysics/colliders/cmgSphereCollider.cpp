#include "cmgSphereCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

SphereCollider::SphereCollider(float radius, const Matrix4f& offset) :
	Collider(ColliderType::k_sphere, offset),
	m_radius(radius)
{

}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float SphereCollider::GetVolume() const
{
	return ((4.0f / 3.0f) * Math::PI * m_radius * m_radius * m_radius);
}

Vector3f SphereCollider::GetSupportPoint(const Vector3f& direction) const
{
	return m_shapeToWorld.c3.xyz +
		(direction * (m_radius / direction.Length()));
}
