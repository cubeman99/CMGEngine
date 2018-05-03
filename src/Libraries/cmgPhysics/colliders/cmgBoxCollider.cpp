#include "cmgBoxCollider.h"
#include <cmgMath/types/cmgPlane.h>


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

Matrix3f BoxCollider::CalcInertiaTensor(float mass) const
{
	float scale = (1.0f / 12.0f) * mass;
	return Matrix3f::CreateScale(
		scale * ((m_halfSize.y * m_halfSize.y) + (m_halfSize.z * m_halfSize.z)),
		scale * ((m_halfSize.z * m_halfSize.z) + (m_halfSize.x * m_halfSize.x)),
		scale * ((m_halfSize.x * m_halfSize.x) + (m_halfSize.y * m_halfSize.y)));
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

bool BoxCollider::CastBoundedRay(const Ray& ray, float& inOutDistance, Vector3f& outNormal) const
{
	bool hit = false;

	Vector3f position = m_shapeToWorld.c3.xyz;

	for (unsigned int i = 0; i < 6; ++i)
	{
		unsigned int axis = i / 2;
		
		// create point and normal in local box space
		Vector3f normal = Vector3f::ZERO;
		normal.v[axis] = (i % 2 == 0 ? 1.0f : -1.0f);
		Vector3f point = Vector3f::ZERO;
		point.v[axis] = m_halfSize.v[axis] * normal.v[axis];
	
		// transform into world space
		normal = m_shapeToWorld.Rotate(normal);
		point = m_shapeToWorld * point;
		Plane plane = Plane(normal, point);

		// Cast onto the infinite face plane.
		float dist;
		if (plane.CastRay(ray, PLANE_SIDE_FRONT, dist) && dist < inOutDistance)
		{
			// Make sure the point is within the face's rectangular area.
			Vector3f hitPoint = ray.GetPoint(inOutDistance);
			unsigned int tangentAxis1 = (axis + 1) % 3;
			unsigned int tangentAxis2 = (axis + 2) % 3;
			Vector3f tangent1 = m_shapeToWorld.col[tangentAxis1].xyz;
			Vector3f tangent2 = m_shapeToWorld.col[tangentAxis2].xyz;
			float dist1 = Math::Abs(Vector3f::Dot(hitPoint, tangent1) -
				Vector3f::Dot(position, tangent1));
			float dist2 = Math::Abs(Vector3f::Dot(hitPoint, tangent2) -
				Vector3f::Dot(position, tangent2));

			if (dist1 <= m_halfSize.v[tangentAxis1] &&
				dist2 <= m_halfSize.v[tangentAxis2])
			{
				inOutDistance = dist;
				outNormal = plane.normal;
				hit = true;
			}
		}
	}

	return hit;

	return false;
}

