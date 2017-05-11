#include "cmgConeCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

ConeCollider::ConeCollider(
		float radius, float height,
		const Matrix4f& offset) :
	Collider(ColliderType::k_cone, offset),
	m_radius(radius),
	m_height(height)
{
	m_sideLength = Math::Sqrt(
		(m_height * m_height) + (m_radius * m_radius));
}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float ConeCollider::GetVolume() const
{
	return ((Math::PI * m_radius * m_radius * m_height) / 3.0f);
}

Vector3f ConeCollider::GetCenterOfMassOffset() const
{
	return Vector3f(0.0f, m_height * 0.25f, 0.0f);
}

Matrix3f ConeCollider::CalcInertiaTensor(float mass) const
{
	float radiusSqr = m_radius * m_radius;
	float xz = 0.15f * mass * (radiusSqr + (4.0f * m_height * m_height));
	return Matrix3f::CreateScale(
		xz, mass * radiusSqr * 0.3f, xz);
}

Vector3f ConeCollider::GetSupportPoint(const Vector3f& direction) const
{
	Vector3f point = m_worldToShape.Rotate(direction);
	point.Normalize();

	if (point.y >= m_radius / m_sideLength)
	{
		point = Vector3f(0.0f, m_height, 0.0f);
		return m_shapeToWorld.TransformAffine(point);
		//return (m_shapeToWorld.c3.xyz + (m_shapeToWorld.c1.xyz * m_height));
	}
	else
	{
		Vector2f xz(point.x, point.z);
		xz.Normalize();
		xz *= m_radius;
		point.x = xz.x;
		point.z = xz.y;
		point.y = 0.0f;
		return m_shapeToWorld.TransformAffine(point);
	}
}
