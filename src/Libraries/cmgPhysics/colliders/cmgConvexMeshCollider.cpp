#include "cmgConvexMeshCollider.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

ConvexMeshCollider::ConvexMeshCollider() :
	Collider(ColliderType::k_convexMesh)
{
}


//-----------------------------------------------------------------------------
// Collider implementations
//-----------------------------------------------------------------------------

float ConvexMeshCollider::GetVolume() const
{
	return 0.0f;
}

Matrix3f ConvexMeshCollider::CalcInertiaTensor(float mass) const
{
	return Matrix3f::ZERO;
}

Vector3f ConvexMeshCollider::GetSupportPoint(const Vector3f& direction) const
{
	float maxDot = -FLT_MAX;
	Vector3f bestVertex;
	for (unsigned int i = 0; i < m_vertices.size(); ++i)
	{
		Vector3f vertex = m_shapeToWorld.TransformAffine(
			m_vertices[i].position);
		float dot = vertex.Dot(direction);
		if (dot > maxDot)
		{
			maxDot = dot;
			bestVertex = vertex;
		}
	}
	return bestVertex;
}


