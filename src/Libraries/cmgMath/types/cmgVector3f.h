#ifndef _CMG_VECTOR_3F_H_
#define _CMG_VECTOR_3F_H_

#include <iostream>
#include <cmgMath/types/cmgVector2f.h>

struct Matrix3f;
struct Matrix4f;
struct Quaternion;
struct Point3i;


// 3D floating-point vector.
struct Vector3f
{
	//-------------------------------------------------------------------------
	// Members
	//-------------------------------------------------------------------------

	union
	{
		struct
		{
			// The x component.
			float x;
			// The y component.
			float y;
			// The z component.
			float z;
		};
		struct
		{
			// The xy components as a 2D vector.
			Vector2f xy;
		};
		struct
		{
			// The components of the vector as an array of 3 floats.
			float v[3];
		};
	};


	//-------------------------------------------------------------------------
	// Constants
	//-------------------------------------------------------------------------

	static const Vector3f ZERO;		// (0, 0, 0)
	static const Vector3f ONE;		// (1, 1, 1)
	static const Vector3f UNITX;	// (1, 0, 0)
	static const Vector3f UNITY;	// (0, 1, 0)
	static const Vector3f UNITZ;	// (0, 0, 1)
	static const Vector3f NEG_UNITX;	// (-1, 0, 0)
	static const Vector3f NEG_UNITY;	// (0, -1, 0)
	static const Vector3f NEG_UNITZ;	// (0, 0, -1)

	static const Vector3f RIGHT;	// +X
	static const Vector3f LEFT;		// -X
	static const Vector3f UP;		// +Y
	static const Vector3f DOWN;		// -Y
	static const Vector3f BACK;		// +Z
	static const Vector3f FORWARD;	// -Z


	//-------------------------------------------------------------------------
	// Constructors
	//-------------------------------------------------------------------------

	// Construct a vector with uninitialized components.
	Vector3f();

	// Construct a vector with the given components.
	Vector3f(float x, float y, float z);

	// Contruct a 3d vector from a 2d vector with a specified z-component.
	Vector3f(const Vector2f& vec2, float z);

	// Cast an integer vector to a float vector.
	Vector3f(const Point3i& p);

	// Construct a vector with all components set to a single value.
	explicit Vector3f(float value);


	//-------------------------------------------------------------------------
	// Accessors
	//-------------------------------------------------------------------------

	// Access an indexed component.
	float operator [](int index) const;

	// Modify an indexed component.
	float& operator [](int index);

	// Get the xy-components of this vector.
	const Vector2f& GetXY() const;

	// Get the xy-components of this vector.
	Vector2f GetXZ() const;

	// Get the swizzled yz-components of this vector.
	Vector2f GetYZ() const;

	// Get the swizzled yx-components of this vector.
	Vector2f GetYX() const;

	// Get the swizzled zx-components of this vector.
	Vector2f GetZX() const;

	// Get the swizzled zy-components of this vector.
	Vector2f GetZY() const;

	// Calculate the length of this vector.
	float Length() const;

	// Calculate the squared-length of this vector.
	float LengthSquared() const;

	// Compute the dot product of this vector with another vector.
	float Dot(const Vector3f& other) const;

	// Compute the cross product of this vector with another vector.
	Vector3f Cross(const Vector3f& other) const;

	// Compute the distance between this vector and another vector.
	float DistTo(const Vector3f& other) const;

	// Compute the squared-distance between this vector and another vector.
	float DistToSqr(const Vector3f& other) const;

	// Get the components of this vector as a float array.
	float* data();

	// Get the components of this vector as a constant float array.
	const float* data() const;


	//-------------------------------------------------------------------------
	// Mutators
	//-------------------------------------------------------------------------

	// Set all the components of this vector to zero.
	Vector3f& SetZero();

	// Set the components of this vector.
	Vector3f& Set(float x, float y, float z);

	// Set all the components of this vector to one value.
	Vector3f& Fill(float value);

	// Set the xy-components of this vector.
	Vector3f& SetXY(const Vector2f& xy);

	// Negate this vector.
	Vector3f& Negate();

	// Normalize this vector.
	Vector3f& Normalize();

	// Rotate this vector by a quaternion rotation.
	Vector3f& Rotate(const Quaternion& rotation);

	// Rotate this vector by an axis-angle rotation (right-handed, clockwise
	// angle).
	Vector3f& Rotate(const Vector3f& axis, float angle);

	// Rotate this vector by a 4x4 rotation matrix (Assuming the vector has a
	// w-component of 0).
	Vector3f& Rotate(const Matrix4f& rotation);

	// Transform this vector by a 3D matrix.
	Vector3f& Transform(const Matrix3f& mTransform);

	// Transform this vector by a 4D matrix (Assuming the vector has a
	// w-component of 1).
	Vector3f& Transform(const Matrix4f& mTransform);


	//-------------------------------------------------------------------------
	// Unary operators
	//-------------------------------------------------------------------------

	// Return the inverse of this vector.
	Vector3f operator -() const;

	// Add another vector.
	void operator +=(const Vector3f& other);

	// Subtract another vector.
	void operator -=(const Vector3f& other);

	// Multiply by another vector.
	void operator *=(const Vector3f& other);

	// Multiply by a scalar.
	void operator *=(float scalar);

	// Divide by another vector.
	void operator /=(const Vector3f& other);

	// Divide by a scalar.
	void operator /=(float denominator);


	//-------------------------------------------------------------------------
	// Binary operators
	//-------------------------------------------------------------------------

	// Compute the sum with another vector.
	Vector3f operator +(const Vector3f& other) const;

	// Compute the difference with another vector.
	Vector3f operator -(const Vector3f& other) const;

	// Compute the product with another vector.
	Vector3f operator *(const Vector3f& other) const;

	// Compute the division with another vector.
	Vector3f operator /(const Vector3f& other) const;

	// Compute the product with a scalar.
	Vector3f operator *(float scalar) const;

	// Compute the division with a scalar.
	Vector3f operator /(float invScalar) const;


	//-------------------------------------------------------------------------
	// Static methods
	//-------------------------------------------------------------------------

	// Returns a normalized vector.
	static Vector3f Normalize(const Vector3f& v);

	// Returns the distance between two vectors.
	static float Dist(const Vector3f& a, const Vector3f& b);

	// Returns the sqaured distance between two vectors.
	static float DistSqr(const Vector3f& a, const Vector3f& b);

	// Returns the dot product of two vectors.
	static float Dot(const Vector3f& a, const Vector3f& b);

	// Returns cross product of two vectors.
	static Vector3f Cross(const Vector3f& a, const Vector3f& b);

	// Performs a linear interpolation between two vectors.
	static Vector3f Lerp(const Vector3f& a, const Vector3f& b, float lerpFactor);

	// Return a vector rotated by a quaternion.
	static Vector3f Rotate(const Vector3f& v, const Quaternion& rotation);

	// Return a vector rotated by a an axis-angle rotation (right-handed).
	static Vector3f Rotate(const Vector3f& v, const Vector3f axis, float angle);
};


//-----------------------------------------------------------------------------
// Left-hand operators
//-----------------------------------------------------------------------------

// Returns a vector multiplied by a scalar.
Vector3f operator *(float scalar, const Vector3f& v);

// Returns the quotient of a numerater and a vector.
Vector3f operator /(float numerator, const Vector3f& v);

// Create a string representation of a vector.
std::ostream& operator <<(std::ostream &out, const Vector3f& v);


#endif // _CMG_VECTOR_3F_H_