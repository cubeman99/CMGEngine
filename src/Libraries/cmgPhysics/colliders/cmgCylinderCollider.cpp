#include "cmgCylinderCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

CylinderCollider::CylinderCollider(
		float radius, float halfHeight,
		const Matrix4f& offset) :
	Collider(ColliderType::k_cylinder, offset),
	m_radius(radius),
	m_halfHeight(halfHeight)
{
}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float CylinderCollider::GetVolume() const
{
	return (Math::TWO_PI * m_radius * m_radius * m_halfHeight);
}

Matrix3f CylinderCollider::CalcInertiaTensor(float mass) const
{
	float radiusSqr = m_radius * m_radius;
	float xz = (mass / 12.0f) * ((3.0f * radiusSqr) +
		(4.0f * m_halfHeight * m_halfHeight));
	return Matrix3f::CreateScale(
		xz, mass * radiusSqr * 0.5f, xz);
}

Vector3f CylinderCollider::GetSupportPoint(const Vector3f& direction) const
{
	Vector3f point = m_worldToShape.Rotate(direction);
	point.y = (point.y > 0.0f ? m_halfHeight : -m_halfHeight);
	Vector2f xz(point.x, point.z);
	xz.Normalize();
	xz *= m_radius;
	point.x = xz.x;
	point.z = xz.y;
	return m_shapeToWorld.TransformAffine(point);
}
