#include "cmgQuaternion.h"
#include <cmgMath/types/cmgVector3f.h>
#include <cmgMath/types/cmgVector4f.h>
#include <cmgMath/types/cmgMatrix3f.h>
#include <cmgMath/cmgMathLib.h>


//-----------------------------------------------------------------------------
// Quaternion constants.
//-----------------------------------------------------------------------------

const Quaternion Quaternion::IDENTITY = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);



//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

// Construct a quaternion with uninitialized components.
Quaternion::Quaternion()
{
}

// Construct a quaternion with the given components.
Quaternion::Quaternion(float x, float y, float z, float w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}

// Contruct a quaternion from a 4d vector.
Quaternion::Quaternion(const Vector4f& v) :
	x(v.x),
	y(v.y),
	z(v.z),
	w(v.w)
{
}

// Construct a quaternion from an axis-angle rotation.
Quaternion::Quaternion(const Vector3f& axis, float angle)
{
	Set(axis, angle);
}



//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

// Get a component of this quaternion.
float Quaternion::operator [](int index) const
{
	CMG_ASSERT_MSG(index >= 0 && index < 4, "Invalid component index.");
	return (&x)[index];
}

// Modify a component of this quaternion.
float& Quaternion::operator [](int index)
{
	CMG_ASSERT_MSG(index >= 0 && index < 4, "Invalid component index.");
	return (&x)[index];
}

// Return the length of this quaternion as a vector.
float Quaternion::Length() const
{
	return Math::Sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Return the squared-length of this quaternion as a vector.
float Quaternion::LengthSquared() const
{
	return ((x * x) + (y * y) + (z * z) + (w * w));
}

float Quaternion::Dot(const Quaternion& other) const
{
	return ((x * other.x) + (y * other.y) + (z * other.z) + (w * other.w));
}

// Return the conjugate of this quaternion.
Quaternion Quaternion::GetConjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Vector3f Quaternion::GetXBasis() const
{
	Vector3f v;
	RotateVector(Vector3f::UNITX, v);
	return v;
}

Vector3f Quaternion::GetYBasis() const
{
	Vector3f v;
	RotateVector(Vector3f::UNITY, v);
	return v;
}

Vector3f Quaternion::GetZBasis() const
{
	Vector3f v;
	RotateVector(Vector3f::UNITZ, v);
	return v;
}

// Return the forward vector of this quaternion.
Vector3f Quaternion::GetForward() const
{
	Vector3f v;
	RotateVector(Vector3f::FORWARD, v);
	return v;
}

// Return the back vector of this quaternion.
Vector3f Quaternion::GetBack() const
{
	Vector3f v;
	RotateVector(Vector3f::BACK, v);
	return v;
}

// Return the up vector of this quaternion.
Vector3f Quaternion::GetUp() const
{
	Vector3f v;
	RotateVector(Vector3f::UP, v);
	return v;
}

// Return the down vector of this quaternion.
Vector3f Quaternion::GetDown() const
{
	Vector3f v;
	RotateVector(Vector3f::DOWN, v);
	return v;
}

// Return the left vector of this quaternion.
Vector3f Quaternion::GetLeft() const
{
	Vector3f v;
	RotateVector(Vector3f::LEFT, v);
	return v;
}

// Return the right vector of this quaternion.
Vector3f Quaternion::GetRight() const
{
	Vector3f v;
	RotateVector(Vector3f::RIGHT, v);
	return v;
}

const float* Quaternion::data() const
{
	return &x;
}

float* Quaternion::data()
{
	return  &x;
}



//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------

// Set the components of this quaternion to zero.
Quaternion& Quaternion::SetIdentity()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
	return *this;
}

// Set the components of this quaternion.
Quaternion& Quaternion::Set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

// Set this quaternion from an axis-angle.
Quaternion& Quaternion::Set(const Vector3f& axis, float angle)
{
	float sinHalfAngle = Math::Sin(angle * 0.5f);
	x = axis.x * sinHalfAngle;
	y = axis.y * sinHalfAngle;
	z = axis.z * sinHalfAngle;
	w = Math::Cos(angle * 0.5f);
	return *this;
}

// Set this quaternion from a set of euler angles.
Quaternion& Quaternion::SetEuler(float pitch, float yaw, float roll)
{
	SetIdentity();
	Rotate(Vector3f::UNITX, pitch);
	Rotate(Vector3f::UNITY, yaw);
	Rotate(Vector3f::UNITZ, roll);
	return *this;
}

// Normalize this quaternion.
Quaternion& Quaternion::Normalize()
{
	float length = Length();
		
	if (length > 0.0f)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}

	return *this;
}

// Rotate this quaternion Q by a quaternion rotation R.
// Q = R * Q
Quaternion& Quaternion::Rotate(const Quaternion& q)
{
	Set((q.x * w) + (q.w * x) + (q.y * z) - (q.z * y),
		(q.y * w) + (q.w * y) + (q.z * x) - (q.x * z),
		(q.z * w) + (q.w * z) + (q.x * y) - (q.y * x),
		(q.w * w) - (q.x * x) - (q.y * y) - (q.z * z));
	Normalize();
	return *this;
}

// Rotate this quaternion by an axis-angle rotation.
Quaternion& Quaternion::Rotate(const Vector3f& axis, float angle)
{
	return Rotate(Quaternion(axis, angle));
}



//-----------------------------------------------------------------------------
// Quaternion to Vector operations.
//-----------------------------------------------------------------------------

void Quaternion::RotateVector(Vector3f& vVec) const
{
	RotateVector(vVec, vVec);
}

void Quaternion::RotateVector(const Vector3f& inVec, Vector3f& outVec) const
{
	// Simplified form of (Q * V * Q')
	float vx = inVec.x;
	float vy = inVec.y;
	float vz = inVec.z;
	outVec.x = vx*(w*w + x*x - y*y - z*z) + 2*vy*(x*y - z*w) + 2*vz*(z*x + y*w);
	outVec.y = vy*(w*w + y*y - z*z - x*x) + 2*vz*(y*z - x*w) + 2*vx*(x*y + z*w);
	outVec.z = vz*(w*w + z*z - x*x - y*y) + 2*vx*(z*x - y*w) + 2*vy*(y*z + x*w);
	//Quaternion q = ((*this) * inVec) * GetConjugate();
	//outVec.Set(q.x, q.y, q.z);
}


//-----------------------------------------------------------------------------
// Operators
//-----------------------------------------------------------------------------

// Return the inverse of this quaternion.
Quaternion Quaternion::operator -() const
{
	return Quaternion(-x, -y, -z, -w);
}

// Add another quaternion.
void Quaternion::operator +=(const Quaternion& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

// Subtract another quaternion.
void Quaternion::operator -=(const Quaternion& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

// Multiply by a scalar.
void Quaternion::operator *=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}

// Returns the sum of two quaternions.
Quaternion Quaternion::operator +(const Quaternion& other) const
{
	return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

// Returns the difference of two quaternions.
Quaternion Quaternion::operator -(const Quaternion& other) const
{
	return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

// Multiplied by a scalar.
Quaternion Quaternion::operator *(float scalar) const
{
	return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
}

// Multiply this quaternion by another.
Quaternion Quaternion::operator *(const Quaternion& other) const
{
	return Quaternion(
		(x * other.w) + (w * other.x) + (y * other.z) - (z * other.y),
		(y * other.w) + (w * other.y) + (z * other.x) - (x * other.z),
		(z * other.w) + (w * other.z) + (x * other.y) - (y * other.x),
		(w * other.w) - (x * other.x) - (y * other.y) - (z * other.z));
}

// Multiply a quaternion by a vector.
Quaternion Quaternion::operator *(const Vector3f& v) const
{
	return Quaternion(
		(w * v.x) + (y * v.z) - (z * v.y),
		(w * v.y) + (z * v.x) - (x * v.z),
		(w * v.z) + (x * v.y) - (y * v.x),
		-(x * v.x) - (y * v.y) - (z * v.z));
}

// Create a string representation.
std::ostream& operator <<(std::ostream &out, const Quaternion& q)
{
	out << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
	return out;
}




//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------

// Dot product
float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

// Linear interpolation between two quaternions.
Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t)
{
	float srcFactor = 1.0f - t;
	float dstFactor = t;

	// Always take the shortest path.
	if (Quaternion::Dot(a, b) < 0.0f)
		dstFactor = -t;
	
	// Perform a component-wise linear interpolation.
	Quaternion result(
		(a.x * srcFactor) + (b.x * dstFactor),
		(a.y * srcFactor) + (b.y * dstFactor),
		(a.z * srcFactor) + (b.z * dstFactor),
		(a.w * srcFactor) + (b.w * dstFactor));

	// Normalize the resulting quaternion.
	result.Normalize();
	return result;
}

// Spherical Linear Interpolation
// See http://en.wikipedia.org/wiki/Slerp
Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
	static const float SLERP_EPSILON = 0.001f;
	
    // Compute the cosine of the angle between the two 4D-vectors.
	float cosHalfTheta = Quaternion::Dot(a, b);

	// if a = b or a = -b then theta = 0 and we can return a.
	if (Math::Abs(cosHalfTheta) >= 1.0f)
		return a;
	
	if (Math::Abs(cosHalfTheta) > 1.0f - SLERP_EPSILON)
		return Lerp(a, (cosHalfTheta < 0.0f ? -b : b), t);

	float sinHalfTheta = Math::Sqrt(1.0f - (cosHalfTheta * cosHalfTheta));
	float halfTheta    = Math::ATan2(sinHalfTheta, Math::Abs(cosHalfTheta));

	float srcFactor;
	float dstFactor;

	// Determine a linear interpolation factor based on the angles.
	if (Math::Abs(sinHalfTheta) < SLERP_EPSILON)
	{
		srcFactor = 0.5f;
		dstFactor = 0.5f;
	}
	else
	{
		float invSinHalfTheta = 1.0f / sinHalfTheta;
		srcFactor = invSinHalfTheta * Math::Sin(halfTheta * (1.0f - t));
		dstFactor = invSinHalfTheta * Math::Sin(halfTheta * t);
		if (cosHalfTheta < 0.0f)
			dstFactor = -dstFactor;
	}
	
	// Perform a component-wise linear interpolation.
	Quaternion result(
		(a.x * srcFactor) + (b.x * dstFactor),
		(a.y * srcFactor) + (b.y * dstFactor),
		(a.z * srcFactor) + (b.z * dstFactor),
		(a.w * srcFactor) + (b.w * dstFactor));

	// Normalize the resulting quaternion.
	result.Normalize();
	return result;
}

// Compute the smallest angle between two quaternions.
float Quaternion::SmallestAngle(const Quaternion& a, const Quaternion& b)
{
	float dot = Quaternion::Dot(a, b);
	return Math::ACos((2.0f * dot * dot) - 1.0f);
}

// Create a quaternion from Euler angles (pitch, yaw, then roll).
Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll)
{
	Quaternion q;
	q.SetEuler(pitch, yaw, roll);
	return q;
}

Quaternion Quaternion::LookAtRotation(const Vector3f& forward, const Vector3f& up)
{
	Vector3f orthoForward = forward;
	orthoForward.Normalize();
	Vector3f orthoRight = Vector3f::Cross(orthoForward, up);
	orthoRight.Normalize();
	Vector3f orthoUp = orthoRight.Cross(orthoForward);
	orthoUp.Normalize();

	Matrix3f m;
	m.SetColumn(0, orthoRight);
	m.SetColumn(1, orthoUp);
	m.SetColumn(2, -orthoForward);
	return m.ToQuaternion();
}


