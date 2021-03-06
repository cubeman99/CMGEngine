#ifndef _CMG_VECTOR_2F_H_
#define _CMG_VECTOR_2F_H_

#include <iostream>
#include <cmgCore/cmgBase.h>

struct Matrix3f;
template <typename T>
struct Vector2;


// 2D floating-point vector.
struct Vector2f
{
	union
	{
		struct { float x, y; };
		struct { float v[2]; };
	};


	// Constants
	static const Vector2f ZERO;
	static const Vector2f ONE;
	static const Vector2f UNITX;
	static const Vector2f UNITY;

	// Constructors
	Vector2f();
	Vector2f(float x, float y);
	explicit Vector2f(float value);
	template <typename T>
	explicit Vector2f(const Vector2<T>& v);

	// Accessors.
	float operator [](int index) const;
	float& operator [](int index);
	float Length() const;
	float LengthSquared() const;

	// Mutators.
	Vector2f& SetZero();
	Vector2f& Set(float x, float y);
	Vector2f& Fill(float value);
	Vector2f& Negate();
	Vector2f& Normalize();
	Vector2f& Rotate(float angle);
	Vector2f& Rotate(const Vector2f& origin, float angle);
	float Dot(const Vector2f& other) const;
	float Cross(const Vector2f& other) const;
	float DistTo(const Vector2f& other) const;
	float DistToSqr(const Vector2f& other) const;
		
	float* data();
	const float* data() const;

	// Unary operators
	Vector2f operator -() const;
	void operator +=(const Vector2f& other);
	void operator -=(const Vector2f& other);
	void operator *=(const Vector2f& other);
	void operator /=(const Vector2f& other);
	void operator *=(float scalar);
	void operator /=(float invScalar);

	// Binary operators
	Vector2f operator +(const Vector2f& other) const;
	Vector2f operator -(const Vector2f& other) const;
	Vector2f operator *(const Vector2f& other) const;
	Vector2f operator /(const Vector2f& other) const;
	Vector2f operator *(float scalar) const;
	Vector2f operator /(float invScalar) const;

	// Static Methods

	// Returns a normalized vector.
	static Vector2f Normalize(const Vector2f& vec);								
	// Returns the distance between two vectors.
	static float Dist(const Vector2f& a, const Vector2f& b);				
	// Returns the sqaured distance between two vectors.
	static float DistSqr(const Vector2f& a, const Vector2f& b);					
	// Returns the dot product of two vectors.
	static float Dot(const Vector2f& a, const Vector2f& b);						
	// Returns cross product of two vectors.
	static float Cross(const Vector2f& a, const Vector2f& b);					
	// Performs a linear interpolation between two vectors.
	static Vector2f Lerp(const Vector2f& a, const Vector2f& b, float lerpFactor);	
};


// Left-hand operators.
Vector2f operator *(float scalar, const Vector2f& v);
Vector2f operator /(float numerator, const Vector2f& v);
std::ostream& operator <<(std::ostream &out, const Vector2f& v);
	

template <typename T>
Vector2f::Vector2f(const Vector2<T>& v) :
	x((float) v.x),
	y((float) v.y)
{
}

#endif // _CMG_VECTOR_2F_H_