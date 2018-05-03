#include "cmgPolygonCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

PolygonCollider::PolygonCollider(unsigned int numVertices, const Vector3f* vertices) :
	Collider(ColliderType::k_polygon)
{
	m_vertices.resize(numVertices);
	for (unsigned int i = 0; i < numVertices; ++i)
		m_vertices[i] = vertices[i];
}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float PolygonCollider::GetVolume() const
{
	return 0.0f;
}

Matrix3f PolygonCollider::CalcInertiaTensor(float mass) const
{
	return Matrix3f::ZERO;
}

Vector3f PolygonCollider::GetSupportPoint(const Vector3f& direction) const
{
	float maxDot = -FLT_MAX;
	Vector3f bestVertex;
	for (unsigned int i = 0; i < m_vertices.size(); ++i)
	{
		Vector3f vertex = m_shapeToWorld.TransformAffine(m_vertices[i]);
		float dot = vertex.Dot(direction);
		if (dot > maxDot)
		{
			maxDot = dot;
			bestVertex = vertex;
		}
	}
	return bestVertex;
}


