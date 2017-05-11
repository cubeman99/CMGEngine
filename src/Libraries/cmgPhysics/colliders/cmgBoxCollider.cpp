#include "cmgBoxCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

BoxCollider::BoxCollider(const Vector3f& halfSize, const Matrix4f& offset) :
	Collider(ColliderType::k_box, offset),
	m_halfSize(halfSize)
{

}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float BoxCollider::GetVolume() const
{
	return (8.0f * m_halfSize.x * m_halfSize.y * m_halfSize.z);
}

Vector3f BoxCollider::GetSupportPoint(const Vector3f& direction) const
{
	Vector3f point = m_shapeToWorld.c3.xyz;
	point += m_shapeToWorld.c0.xyz * m_halfSize.x *
		(direction.Dot(m_shapeToWorld.c0.xyz) > 0.0f ? 1.0f : -1.0f);
	point += m_shapeToWorld.c1.xyz * m_halfSize.y *
		(direction.Dot(m_shapeToWorld.c1.xyz) > 0.0f ? 1.0f : -1.0f);
	point += m_shapeToWorld.c2.xyz * m_halfSize.z *
		(direction.Dot(m_shapeToWorld.c2.xyz) > 0.0f ? 1.0f : -1.0f);
	return point;
}

