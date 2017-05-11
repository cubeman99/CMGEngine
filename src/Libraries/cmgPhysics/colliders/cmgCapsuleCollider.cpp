#include "cmgCapsuleCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

CapsuleCollider::CapsuleCollider(
		float radius, float halfHeight,
		const Matrix4f& offset) :
	Collider(ColliderType::k_capsule, offset),
	m_radius(radius),
	m_halfHeight(halfHeight)
{
}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float CapsuleCollider::GetVolume() const
{
	float cylinderVolume = Math::TWO_PI * m_radius * m_radius * m_halfHeight;
	float sphereVolume = (4.0f / 3.0f) * Math::PI * m_radius * m_radius * m_radius;
	return (cylinderVolume + sphereVolume);
}

Matrix3f CapsuleCollider::CalcInertiaTensor(float mass) const
{
	float radiusSqr = m_radius * m_radius;
	
	float cylinderVolume = Math::TWO_PI * m_radius * m_radius * m_halfHeight;
	float sphereVolume = (4.0f / 3.0f) * Math::PI * m_radius * m_radius * m_radius;
	float totalVolume = cylinderVolume + sphereVolume;

	// From cylinder.
	float xz = (mass / 12.0f) * ((3.0f * radiusSqr) + (4.0f * m_halfHeight * m_halfHeight));
	float y = mass * radiusSqr * 0.5f;

	// From hemispheres (use parallel axis theorom to handle the offset).
	float sphereMass = mass * (sphereVolume / totalVolume);
	float sphereInteria = (2.0f / 5.0f) * mass * radiusSqr;
	xz += sphereInteria;
	y += sphereInteria * (4.0f * m_halfHeight * m_halfHeight * sphereMass);

	return Matrix3f::CreateScale(xz, y, xz);
}

Vector3f CapsuleCollider::GetSupportPoint(const Vector3f& direction) const
{
	Vector3f point = m_worldToShape.Rotate(direction);
	float yOffset = (point.y > 0.0f ? m_halfHeight : -m_halfHeight);
	point.Normalize();
	point *= m_radius;
	point.y += yOffset;
	return m_shapeToWorld.TransformAffine(point);
}
