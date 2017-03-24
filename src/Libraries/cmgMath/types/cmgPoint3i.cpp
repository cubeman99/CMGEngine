#include "cmgPoint3i.h"
#include <cmgMath/types/cmgPoint2i.h>
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/cmgMathLib.h>


//-----------------------------------------------------------------------------
// Point3i constants.
//-----------------------------------------------------------------------------

const Point3i Point3i::ZERO  = Point3i(0, 0, 0);
const Point3i Point3i::ONE   = Point3i(1, 1, 1);
const Point3i Point3i::UNITX = Point3i(1, 0, 0);
const Point3i Point3i::UNITY = Point3i(0, 1, 0);
const Point3i Point3i::UNITZ = Point3i(0, 1, 1);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a vector with unitialized components.
Point3i::Point3i()
{
}

// Construct a vector with the given components.
Point3i::Point3i(int x, int y, int z) :
	x(x),
	y(y),
	z(z)
{
}

// Construct a vector with the given components.
Point3i::Point3i(const Point2i& xy, int z) :
	x(xy.x),
	y(xy.y),
	z(z)
{
}

// Construct a vector with the given components.
Point3i::Point3i(const Vector3f& v) :
	x((int) v.x),
	y((int) v.y),
	z((int) v.z)
{
}

// Construct a vector with all components equal to the given value.
Point3i::Point3i(int value) :
	x(value),
	y(value),
	z(value)
{
}


//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Get a component of this vector.
int Point3i::operator [](int index) const
{
	CMG_ASSERT_MSG(index >= 0 && index < 3, "Invalid component index.");
	return (&x)[index];
}

// Modify a component of this vector.
int& Point3i::operator [](int index)
{
	CMG_ASSERT_MSG(index >= 0 && index < 3, "Invalid component index.");
	return (&x)[index];
}

// Return the xy-components of this vector.
Point2i Point3i::GetXY() const { return Point2i(x, y); }

// Return the xz-components of this vector.
Point2i Point3i::GetXZ() const { return Point2i(x, z); }

// Return the yz-components of this vector.
Point2i Point3i::GetYZ() const { return Point2i(y, z); }

// Return the yx-components of this vector.
Point2i Point3i::GetYX() const { return Point2i(y, x); }

// Return the zx-components of this vector.
Point2i Point3i::GetZX() const { return Point2i(z, x); }

// Return the zy-components of this vector.
Point2i Point3i::GetZY() const { return Point2i(z, y); }



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the components of this vector to zero.
Point3i& Point3i::SetZero()
{
	x = 0;
	y = 0;
	z = 0;
	return *this;
}

// Set the components of this vector.
Point3i& Point3i::Set(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
}

// Fill all the components with the given value.
Point3i& Point3i::Fill(int value)
{
	x = value;
	y = value;
	z = value;
	return *this;
}

// Negate this vector.
Point3i& Point3i::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}



//-----------------------------------------------------------------------------
// Unary operations.
//-----------------------------------------------------------------------------

// Return the inverse of this vector.
Point3i Point3i::operator -() const
{
	return Point3i(-x, -y, -z);
}

// Add another vector.
void Point3i::operator +=(const Point3i& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

// Subtract another vector.
void Point3i::operator -=(const Point3i& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

// Multiply by another vector.
void Point3i::operator *=(const Point3i& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

// Divide by another vector.
void Point3i::operator /=(const Point3i& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

// Multiply by scalar.
void Point3i::operator *=(int scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

// Divide by denominator.
void Point3i::operator /=(int denominator)
{
	x /= denominator;
	y /= denominator;
	z /= denominator;
}



//-----------------------------------------------------------------------------
// Binary operations.
//-----------------------------------------------------------------------------

Point3i Point3i::operator +(const Point3i& other) const
{
	return Point3i(x + other.x, y + other.y, z + other.z);
}

Point3i Point3i::operator -(const Point3i& other) const
{
	return Point3i(x - other.x, y - other.y, z - other.z);
}

Point3i Point3i::operator *(const Point3i& other) const
{
	return Point3i(x * other.x, y * other.y, z * other.z);
}

Point3i Point3i::operator /(const Point3i& other) const
{
	return Point3i(x / other.x, y / other.y, z / other.z);
}

Point3i Point3i::operator *(int scalar) const
{
	return Point3i(x * scalar, y * scalar, z * scalar);
}

Point3i Point3i::operator /(int denominator) const
{
	return Point3i(x / denominator, y / denominator, z / denominator);
}

// Returns a vector multiplied by a scalar.
Point3i operator *(int scalar, const Point3i& v)
{
	return Point3i(scalar * v.x, scalar * v.y, scalar * v.z);
}

// Returns the quotient of a numerater and a vector.
Point3i operator /(int numerator, const Point3i& v)
{
	return Point3i(numerator / v.x, numerator / v.y, numerator / v.z);
}

// Returns a vector multiplied by a scalar.
Vector3f operator *(float scalar, const Point3i& a)
{
	return Vector3f((float) a.x * scalar, (float) a.y * scalar, (float) a.z * scalar);
}

// Returns the quotient of a numerater and a vector.
Vector3f operator /(float numerator, const Point3i& v)
{
	return Vector3f(numerator / (float) v.x, numerator / (float) v.y, numerator / (float) v.z);
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Point3i& v)
{
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

