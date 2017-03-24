#include "cmgTransform3f.h"
#include <cmgMath/types/cmgMatrix4f.h>


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const Transform3f Transform3f::IDENTITY = Transform3f(
	Vector3f::ZERO, Quaternion::IDENTITY,Vector3f::ONE);


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// Construct a transform with uninitialized members.
Transform3f::Transform3f() :
	position(Vector3f::ZERO),
	rotation(Quaternion::IDENTITY),
	scale(Vector3f::ONE)
{
}

// Construct a transform from the given position, rotation, and scale.
Transform3f::Transform3f(const Vector3f& position,
						const Quaternion& rotation,
						const Vector3f& scale) :
	position(position),
	rotation(rotation),
	scale(scale)
{
}

// Construct a transform from the given position, rotation, and scale.
Transform3f::Transform3f(const Vector3f& position,
						const Quaternion& rotation,
						float scale) :
	position(position),
	rotation(rotation),
	scale(scale)
{
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

Matrix4f Transform3f::GetMatrix() const
{
	return Matrix4f::CreateTranslation(position) *
			Matrix4f::CreateRotation(rotation) *
			Matrix4f::CreateScale(scale);
}

Matrix4f Transform3f::GetInvMatrix() const
{
	return Matrix4f::CreateScale(1.0f / scale) *
			Matrix4f::CreateRotation(rotation.GetConjugate()) *
			Matrix4f::CreateTranslation(-position);
}


//-----------------------------------------------------------------------------
// Mutators
//----------------------------------------------------------------------------

// Set to the identity transform (no transform).
void Transform3f::SetIdentity()
{
	position = Vector3f::ZERO;
	rotation = Quaternion::IDENTITY;
	scale = Vector3f::ONE;
}

// Set the position and rotation of this transform.
void Transform3f::Set(const Vector3f& _pos,
					  const Quaternion& _rot,
					  const Vector3f& _scale)
{
	position = _pos;
	rotation = _rot;
	scale = _scale;
}

// Set the position and rotation of this transform.
void Transform3f::Set(const Vector3f& _pos,
					  const Quaternion& _rot,
					  float _scale)
{
	position = _pos;
	rotation = _rot;
	scale = Vector3f(_scale);
}

