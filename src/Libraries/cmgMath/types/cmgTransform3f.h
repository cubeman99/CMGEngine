#ifndef _CMG_TRANSFORM_3F_H_
#define _CMG_TRANSFORM_3F_H_

#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgQuaternion.h>

struct Matrix4f;


// A 3D transformation (position, rotation, and scale).
struct Transform3f
{
	Vector3f   position;
	Quaternion rotation;
	Vector3f   scale;
	
	static const Transform3f IDENTITY;

	// Constructors.
	Transform3f();
	Transform3f(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale);
	Transform3f(const Vector3f& position, const Quaternion& rotation, float scale);

	// Accessors.
	Matrix4f GetMatrix() const;
	Matrix4f GetInvMatrix() const;

	// Mutators.
	void SetIdentity();
	void Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	void Set(const Vector3f& pos, const Quaternion& rot, float scale);
	inline void SetPosition(const Vector3f& position)	{ this->position = position; }
	inline void SetRotation(const Quaternion& rotation)	{ this->rotation = rotation; }
	inline void SetScale(const Vector3f& scale)			{ this->scale = scale; }
	inline void SetScale(float scale)					{ this->scale = Vector3f(scale); }
};


#endif // _CMG_TRANSFORM_3F_H_