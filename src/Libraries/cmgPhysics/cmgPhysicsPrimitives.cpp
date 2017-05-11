#include "cmgPhysicsPrimitives.h"
#include "cmgRigidBody.h"
#include <cmgMath/cmgMathLib.h>


//-----------------------------------------------------------------------------
// CollisionPrimitive
//-----------------------------------------------------------------------------

CollisionPrimitive::CollisionPrimitive() :
	body(nullptr),
	bodyToShape(Matrix4f::IDENTITY)
{
}

Vector3f CollisionPrimitive::GetWorldPosition() const
{
	return TransformLocalToWold(Vector3f::ZERO);
}

Vector3f CollisionPrimitive::TransformLocalToWold(const Vector3f& localPoint) const
{
	Vector3f result = localPoint;
	result = bodyToShape.TransformAffine(result);
	result = body->m_bodyToWorld.TransformAffine(result);
	return result;
}
	
Vector3f CollisionPrimitive::RotateLocalToWold(const Vector3f& localDirection) const
{
	Vector3f result = localDirection;
	result = bodyToShape.Rotate(result);
	result = body->m_bodyToWorld.Rotate(result);
	return result;
}

Vector3f CollisionPrimitive::TransformWorldToLocal(const Vector3f& localPoint) const
{
	Vector3f result = localPoint;
	result = body->m_bodyToWorld.TransformAffineInverse(result);
	result = bodyToShape.TransformAffineInverse(result);
	return result;
}
	
Vector3f CollisionPrimitive::RotateWorldToLocal(const Vector3f& localDirection) const
{
	Vector3f result = localDirection;
	result = body->m_bodyToWorld.RotateInverse(result);
	result = bodyToShape.RotateInverse(result);
	return result;
}


//-----------------------------------------------------------------------------
// CollisionBox
//-----------------------------------------------------------------------------

Plane CollisionBox::GetPlane(int axisIndex, bool negate) const
{
	// Create a normal and point in local space.
	Vector3f normal(0, 0, 0);
	normal[axisIndex] = (negate ? -1.0f : 1.0f);
	Vector3f point = normal * halfSize;

	// Move normal and point into world space.
	point = TransformLocalToWold(point);
	normal = RotateLocalToWold(normal);

	// Create the plane from point and direction.
	return Plane(normal, point);
}

Vector3f CollisionBox::GetAxis(int axisIndex) const
{
	Vector3f axis(0, 0, 0);
	axis[axisIndex] = 1.0f;
	return RotateLocalToWold(axis);
}

Vector3f CollisionBox::GetVertex(unsigned int index) const
{
	Vector3f vertex = halfSize;
	if (index & 0x1) vertex.x = -vertex.x;
	if (index & 0x2) vertex.y = -vertex.y;
	if (index & 0x4) vertex.z = -vertex.z;
	return TransformLocalToWold(vertex);
}

void CollisionBox::GetEdge(unsigned int edgeIndex, Vector3f& outEndPoint1, Vector3f& outEndPoint2) const
{
	unsigned int axis = edgeIndex / 6;
	unsigned int axis2 = 1 - axis;
	unsigned int side = (edgeIndex / 3) % 2;
	unsigned int step = edgeIndex % 3;

	unsigned int v1 = 0;
	if (side == 1)
	{
		v1 = 1 << axis;
		v1 ^= 1 << axis2; // invert v1 on bi-tangent axis.
	}
	if (axis == 1)
		v1 ^= 1 << axis2;

	unsigned int v2 = v1;

	if (step >= 1)
	{
		v2 |= 4; // make v2 to positive side of normal axis.
		v1 ^= 1 << axis2; // invert v1 on bi-tangent axis.
	}
	if (step >= 2)
	{
		v1 |= 4; // make v1 to positive side of normal axis.
	}
	if (step < 2)
	{
		v2 ^= 1 << axis2; // invert v2 on bi-tangent axis.
	}

	outEndPoint1 = GetVertex(v1);
	outEndPoint2 = GetVertex(v2);
}

void CollisionBox::ProjectOntoAxis(const Vector3f& axis, float& outMin, float& outMax) const
{
	outMin = FLT_MAX;
	outMax = -FLT_MAX;

	for (unsigned int index = 0; index < 8; ++index)
	{
		Vector3f vertex = GetVertex(index);
		float projection = vertex.Dot(axis);

		if (projection < outMin)
			outMin = projection;
		if (projection > outMax)
			outMax = projection;
	}
}


float CollisionBox::GetVolume() const
{
	return (8.0f * (halfSize.x * halfSize.y * halfSize.z));
}

void CollisionBox::CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const
{
	float scale = (1.0f / 12.0f) * mass;
	outInertiaTensor.InitScale(
		scale * ((halfSize.y * halfSize.y) + (halfSize.z * halfSize.z)),
		scale * ((halfSize.z * halfSize.z) + (halfSize.x * halfSize.x)),
		scale * ((halfSize.x * halfSize.x) + (halfSize.y * halfSize.y)));
}

Vector3f CollisionBox::GetSupport(const Vector3f& direction) const
{
	Vector3f farthestVertex;
	float farthestDot = -FLT_MAX;
	for (unsigned int i = 0; i < 8; ++i)
	{
		Vector3f vertex = GetVertex(i);
		float dot = vertex.Dot(direction);
		if (dot > farthestDot)
		{
			farthestVertex = vertex;
			farthestDot = dot;
		}
	}
	return farthestVertex;
}


//-----------------------------------------------------------------------------
// CollisionSphere
//-----------------------------------------------------------------------------

float CollisionSphere::GetVolume() const
{
	return (4.0f / 3.0f) * (Math::PI * radius * radius * radius);
}

void CollisionSphere::CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const
{
	outInertiaTensor.InitScale((2.0f / 5.0f) * mass * radius * radius);
}



//-----------------------------------------------------------------------------
// CollisionPlane
//-----------------------------------------------------------------------------

float CollisionPlane::GetVolume() const
{
	return 0.0f;
}

void CollisionPlane::CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const
{
	outInertiaTensor.SetZero();
}

//-----------------------------------------------------------------------------
// CollisionPolyhedron
//-----------------------------------------------------------------------------

float CollisionPolyhedron::GetVolume() const
{
	return 0.0f;
}

void CollisionPolyhedron::CalculateInertiaTensor(float mass, Matrix3f& outInertiaTensor) const
{
	outInertiaTensor.SetZero();
}

