#ifndef _CMG_QUATERNION_H_
#define _CMG_QUATERNION_H_

#include <iostream>
#include <cmgMath/types/cmgVector3f.h>

struct Vector4f;


// Half-angle quaternion (represents a 3D rotation).
// Angles are in radians and counter-clockwise around the axis of rotation.
struct Quaternion
{
	union
	{
		struct { float x, y, z, w; };
		struct { Vector3f xyz; };
	};
	
	// Constants
	static const Quaternion IDENTITY;

	// Constructors
	Quaternion();
	explicit Quaternion(float x, float y, float z, float w);
	explicit Quaternion(const Vector4f& vec);
	explicit Quaternion(const Vector3f& axis, float angle);

	// Accessors
	float		operator [](int index) const;
	float&		operator [](int index);
	float		Length() const;
	float		LengthSquared() const;
	float		Dot(const Quaternion& other) const;
	Quaternion	GetConjugate() const;
	Vector3f	GetXBasis() const;
	Vector3f	GetYBasis() const;
	Vector3f	GetZBasis() const;
	Vector3f	GetForward() const;
	Vector3f	GetBack() const;
	Vector3f	GetUp() const;
	Vector3f	GetDown() const;
	Vector3f	GetLeft()  const;
	Vector3f	GetRight() const;
	const float* data() const;
	float*		data();

	// Mutators
	Quaternion& SetIdentity();
	Quaternion& Set(float x, float y, float z, float w);
	Quaternion& Set(const Vector3f& axis, float angle);
	Quaternion& SetEuler(float pitch, float yaw, float roll);
	Quaternion& Normalize();
	Quaternion& Rotate(const Quaternion& rotation);
	Quaternion& Rotate(const Vector3f& axis, float angle);

	// Vector rotation
	void RotateVector(Vector3f& vec) const;
	void RotateVector(const Vector3f& inVec, Vector3f& outVec) const;

	// Operators
	Quaternion	operator -() const;
	void		operator +=(const Quaternion& q);
	void		operator -=(const Quaternion& q);
	void		operator *=(float scalar);
	Quaternion	operator +(const Quaternion& other) const;
	Quaternion	operator -(const Quaternion& other) const;
	Quaternion	operator *(float scalar) const;
	Quaternion	operator *(const Quaternion& other) const;
	Quaternion	operator *(const Vector3f& vec) const;

	// Static Methods
	static float		Dot(const Quaternion& a, const Quaternion& b);
	static Quaternion	Lerp(const Quaternion& a, const Quaternion& b, float t);
	static Quaternion	Slerp(const Quaternion& a, const Quaternion& b, float t);
	static float		SmallestAngle(const Quaternion& a, const Quaternion& b);
	static Quaternion	FromEuler(float pitch, float yaw, float roll);
	static Quaternion	LookAtRotation(const Vector3f& forward, const Vector3f& up);
};


// Left-hand Operators.
std::ostream& operator <<(std::ostream &out, const Quaternion& q);


#endif // _CMG_QUATERNION_H_