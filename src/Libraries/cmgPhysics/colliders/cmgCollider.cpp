#include "cmgCollider.h"
#include "cmgRigidBody.h"


Collider::Collider(ColliderType type, const Matrix4f& offset) :
	m_type(type),
	m_shapeToBody(offset),
	m_bodyToShape(offset),
	m_body(nullptr)
{
	m_bodyToShape.InvertAffine();
}

void Collider::CalcDerivedData()
{
	m_shapeToWorld = m_body->GetBodyToWorld() * m_shapeToBody;
	m_worldToShape = m_bodyToShape * m_body->GetWorldToBody();
}


