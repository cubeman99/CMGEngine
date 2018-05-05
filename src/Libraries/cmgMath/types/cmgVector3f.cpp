#include "cmgVector3f.h"
#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/types/cmgMatrix4f.h>
#include <cmgMath/types/cmgPoint3i.h>
#include <cmgMath/types/cmgQuaternion.h>
#include <cmgMath/cmgMathLib.h>


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const Vector3f Vector3f::ZERO = Vector3f(0.0f, 0.0f, 0.0f);
const Vector3f Vector3f::ONE = Vector3f(1.0f, 1.0f, 1.0f);

const Vector3f Vector3f::UNITX = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f Vector3f::UNITY = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f Vector3f::UNITZ = Vector3f(0.0f, 0.0f, 1.0f);
const Vector3f Vector3f::NEG_UNITX = Vector3f(-1.0f, 0.0f, 0.0f);
const Vector3f Vector3f::NEG_UNITY = Vector3f(0.0f, -1.0f, 0.0f);
const Vector3f Vector3f::NEG_UNITZ = Vector3f(0.0f, 0.0f, -1.0f);

const Vector3f Vector3f::RIGHT = Vector3f::UNITX;
const Vector3f Vector3f::LEFT = Vector3f::NEG_UNITX;
const Vector3f Vector3f::UP = Vector3f::UNITY;
const Vector3f Vector3f::DOWN = Vector3f::NEG_UNITY;
const Vector3f Vector3f::BACK = Vector3f::UNITZ;
const Vector3f Vector3f::FORWARD = Vector3f::NEG_UNITZ;


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// Construct a vector with uninitialized components.
Vector3f::Vector3f()
{
}

// Construct a vector with the given components.
Vector3f::Vector3f(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

// Contruct a 3d vector from a 2d vector with a specified z-component.
Vector3f::Vector3f(const Vector2f& vec2, float z)
	: x(vec2.x), y(vec2.y), z(z)
{
}

// Cast an integer vector to a float vector.
Vector3f::Vector3f(const Point3i& p)
	: x((float) p.x), y((float) p.y), z((float) p.z)
{
}

// Construct a vector with all components set to a single value.
Vector3f::Vector3f(float value)
	: x(value), y(value), z(value)
{
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

// Access an indexed component.
float Vector3f::operator [](int index) const
{
	CMG_ASSERT_MSG(index >= 0 && index < 3, "Invalid component index.");
	return v[index];
}

// Modify an indexed component.
float& Vector3f::operator [](int index)
{
	CMG_ASSERT_MSG(index >= 0 && index < 3, "Invalid component index.");
	return v[index];
}

// Return the xy-components of this vector.
const Vector2f& Vector3f::GetXY() const
{
	return xy;
}

// Return the xz-components of this vector.
Vector2f Vector3f::GetXZ() const
{
	return Vector2f(x, z);
}

// Return the yz-components of this vector.
Vector2f Vector3f::GetYZ() const
{
	return Vector2f(y, z);
}

// Return the yx-components of this vector.
Vector2f Vector3f::GetYX() const
{
	return Vector2f(y, x);
}

// Return the zx-components of this vector.
Vector2f Vector3f::GetZX() const
{
	return Vector2f(z, x);
}

// Return the zy-components of this vector.
Vector2f Vector3f::GetZY() const
{
	return Vector2f(z, y);
}

// Calculate the length of this vector.
float Vector3f::Length() const
{
	return Math::Sqrt((x * x) + (y * y) + (z * z));
}

// Calculate the squared-length of this vector.
float Vector3f::LengthSquared() const
{
	return ((x * x) + (y * y) + (z * z));
}

// Compute the dot product of this vector with another vector.
float Vector3f::Dot(const Vector3f& vOther) const
{
	return Vector3f::Dot(*this, vOther);
}

// Compute the cross product of this vector with another vector.
Vector3f Vector3f::Cross(const Vector3f& vOther) const
{
	return Vector3f::Cross(*this, vOther);
}

// Compute the distance between this vector and another vector.
float Vector3f::DistTo(const Vector3f& vOther) const
{
	return Vector3f::Dist(*this, vOther);
}

// Compute the squared-distance between this vector and another vector.
float Vector3f::DistToSqr(const Vector3f& vOther) const
{
	return Vector3f::DistSqr(*this, vOther);
}

// Get the components of this vector as a float array.
float* Vector3f::data()
{
	return  v;
}

// Get the components of this vector as a constant float array.
const float* Vector3f::data() const
{
	return v;
}


//-----------------------------------------------------------------------------
// Mutators
//-----------------------------------------------------------------------------

// Set all the components of this vector to zero.
Vector3f& Vector3f::SetZero()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	return *this;
}

// Set the components of this vector.
Vector3f& Vector3f::Set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
}

// Set all the components of this vector to one value.
Vector3f& Vector3f::Fill(float value)
{
	x = value;
	y = value;
	z = value;
	return *this;
}

// Set the xy-components of this vector.
Vector3f& Vector3f::SetXY(const Vector2f& xy)
{
	x = xy.x;
	y = xy.y;
	return *this;
}

// Negate this vector.
Vector3f& Vector3f::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

// Normalize this vector.
Vector3f& Vector3f::Normalize()
{
	float length = Length();

	if (length > 0.0f)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}

	return *this;
}

// Rotate this vector by a quaternion rotation.
Vector3f& Vector3f::Rotate(const Quaternion& rotation)
{
	rotation.RotateVector(*this);
	return *this;
}

// Rotate this vector by an axis-angle rotation (right-handed, clockwise angle).
Vector3f& Vector3f::Rotate(const Vector3f& axis, float angle)
{
	Quaternion(axis, angle).RotateVector(*this);
	return *this;
}

// Rotate this vector by a 4x4 rotation matrix (Assuming the vector has a
// w-component of 0).
Vector3f& Vector3f::Rotate(const Matrix4f& mRotation)
{
	Vector3f result;
	mRotation.ApplyRotation(*this, result);
	*this = result;
	return *this;
}

// Transform this vector by a 3D matrix.
Vector3f& Vector3f::Transform(const Matrix3f& transform)
{
	Vector3f result;
	transform.TransformVector(*this, result);
	*this = result;
	return *this;
}

// Transform this vector by a 4D matrix (Assuming the vector has a w-component
// of 1).
Vector3f& Vector3f::Transform(const Matrix4f& transform)
{
	Vector3f result;
	transform.ApplyTransform(*this, result);
	*this = result;
	return *this;
}


//-----------------------------------------------------------------------------
// Unary Operators
//-----------------------------------------------------------------------------

// Return the inverse of this vector.
Vector3f Vector3f::operator -() const
{
	return Vector3f(-x, -y, -z);
}

// Add another vector.
void Vector3f::operator +=(const Vector3f& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

// Subtract another vector.
void Vector3f::operator -=(const Vector3f& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

// Multiply by another vector.
void Vector3f::operator *=(const Vector3f& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

// Multiply by a scalar.
void Vector3f::operator *=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

// Divide by another vector.
void Vector3f::operator /=(const Vector3f& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

// Divide by a scalar.
void Vector3f::operator /=(float denominator)
{
	x /= denominator;
	y /= denominator;
	z /= denominator;
}


//-----------------------------------------------------------------------------
// Binary Operators
//-----------------------------------------------------------------------------

// Compute the sum with another vector.
Vector3f Vector3f::operator +(const Vector3f& other) const
{
	return Vector3f(x + other.x, y + other.y, z + other.z);
}

// Compute the difference with another vector.
Vector3f Vector3f::operator -(const Vector3f& other) const
{
	return Vector3f(x - other.x, y - other.y, z - other.z);
}

// Compute the product with another vector.
Vector3f Vector3f::operator *(const Vector3f& other) const
{
	return Vector3f(x * other.x, y * other.y, z * other.z);
}

// Compute the division with another vector.
Vector3f Vector3f::operator /(const Vector3f& other) const
{
	return Vector3f(x / other.x, y / other.y, z / other.z);
}

// Compute the product with a scalar.
Vector3f Vector3f::operator *(float scalar) const
{
	return Vector3f(x * scalar, y * scalar, z * scalar);
}

// Compute the division with a scalar.
Vector3f Vector3f::operator /(float invScalar) const
{
	float scalar = 1.0f / invScalar;
	return Vector3f(x * scalar, y * scalar, z * scalar);
}

// Returns a vector multiplied by a scalar.
Vector3f operator *(float scalar, const Vector3f& a)
{
	return Vector3f(a.x * scalar, a.y * scalar, a.z * scalar);
}

// Returns the quotient of a numerater and a vector.
Vector3f operator /(float numerator, const Vector3f& v)
{
	return Vector3f(v.x / numerator, v.y / numerator, v.z / numerator);
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Vector3f& v)
{
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}


//-----------------------------------------------------------------------------
// Static Methods
//-----------------------------------------------------------------------------

// Returns a normalized vector.
Vector3f Vector3f::Normalize(const Vector3f& v)
{
	Vector3f result = v;
	result.Normalize();
	return result;
}

// Returns the distance between two vectors.
float Vector3f::Dist(const Vector3f& a, const Vector3f& b)
{
	return Math::Sqrt((b.x - a.x) * (b.x - a.x) +
		(b.y - a.y) * (b.y - a.y) +
		(b.z - a.z) * (b.z - a.z));
}

// Returns the squared distance between two vectors.
float Vector3f::DistSqr(const Vector3f& a, const Vector3f& b)
{
	return (b.x - a.x) * (b.x - a.x) +
		(b.y - a.y) * (b.y - a.y) +
		(b.z - a.z) * (b.z - a.z);
}

// Returns the dot product of two vectors.
float Vector3f::Dot(const Vector3f& a, const Vector3f& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

// Returns cross product of two vectors (in a right-handed coordinate system).
Vector3f Vector3f::Cross(const Vector3f& a, const Vector3f& b)
{
	Vector3f result;
	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	return result;
}

// Performs a linear interpolation between two vectors.
Vector3f Vector3f::Lerp(const Vector3f& a, const Vector3f& b, float lerpFactor)
{
	return (a * (1.0f - lerpFactor)) + (b * lerpFactor);
}

// Return a vector rotated by a quaternion.
Vector3f Vector3f::Rotate(const Vector3f& v, const Quaternion& rotation)
{
	Vector3f result = v;
	result.Rotate(rotation);
	return result;
}

// Return a vector rotated by a an axis-angle rotation (right-handed).
Vector3f Vector3f::Rotate(const Vector3f& v, const Vector3f axis, float angle)
{
	Vector3f result = v;
	result.Rotate(axis, angle);
	return result;
}

