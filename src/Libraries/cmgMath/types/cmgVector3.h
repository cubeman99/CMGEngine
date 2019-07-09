#ifndef __CMG_MATH_TYPES_VECTOR_3_H__
#define __CMG_MATH_TYPES_VECTOR_3_H__

#include <iostream>
#include <cmgMath/types/cmgVector2.h>

struct Vector3f;


// 3D vector
template <typename T_Type>
struct Vector3
{
	//-------------------------------------------------------------------------
	// Members
	//-------------------------------------------------------------------------

	union
	{
		struct
		{
			// The x component
			T_Type x;
			// The y component
			T_Type y;
			// The z component
			T_Type z;
		};
		struct
		{
			// The xy components as a 2D vector
			Vector2<T_Type> xy;
		};
		struct
		{
			// The components of the vector as an array of 3 values
			T_Type v[3];
		};
	};
	

	//-------------------------------------------------------------------------
	// Constants
	//-------------------------------------------------------------------------

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 UNITX;
	static const Vector3 UNITY;
	static const Vector3 UNITZ;

	//-------------------------------------------------------------------------
	// Constructors
	//-------------------------------------------------------------------------

	Vector3()
	{
	}
	Vector3(T_Type x, T_Type y, T_Type z) :
		x(x),
		y(y),
		z(z)
	{
	}
	Vector3(const Vector2<T_Type>& xy, T_Type z) :
		x(xy.x),
		y(xy.y),
		z(z)
	{
	}
	Vector3(const Vector3<T_Type>& v) :
		x(v.x),
		y(v.y),
		z(v.z)
	{
	}
	explicit Vector3(const Vector3f& vVec) :
		x((T_Type) v.x),
		y((T_Type) v.y),
		z((T_Type) v.z)
	{
	}
	explicit Vector3(T_Type value) :
		x(value),
		y(value),
		z(value)
	{
	}
	
	//-------------------------------------------------------------------------
	// Accessors
	//-------------------------------------------------------------------------

	T_Type operator [](T_Type index) const
	{
		CMG_ASSERT_MSG(index >= 0 && index < 3, "Invalid component index.");
		return (&x)[index];
	}
	T_Type& operator [](T_Type index)
	{
		CMG_ASSERT_MSG(index >= 0 && index < 3, "Invalid component index.");
		return (&x)[index];
	}
	Vector3 GetXY() const { return Vector3(x, y); }
	Vector3 GetXZ() const { return Vector3(x, z); }
	Vector3 GetYZ() const { return Vector3(y, z); }
	Vector3 GetYX() const { return Vector3(y, x); }
	Vector3 GetZX() const { return Vector3(z, x); }
	Vector3 GetZY() const { return Vector3(z, y); }

	//-----------------------------------------------------------------------------
	// Mutators
	//-----------------------------------------------------------------------------

	Vector3& SetZero()
	{
		x = 0;
		y = 0;
		z = 0;
		return *this;
	}
	Vector3& Set(T_Type x, T_Type y, T_Type z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		return *this;
	}
	Vector3& Fill(T_Type value)
	{
		x = value;
		y = value;
		z = value;
		return *this;
	}
	Vector3& Negate()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	//-----------------------------------------------------------------------------
	// Unary operators
	//-----------------------------------------------------------------------------

	Vector3 operator -() const
	{
		return Vector3<T_Type>(-x, -y, -z);
	}
	void operator +=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
	void operator -=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	void operator *=(const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}
	void operator /=(const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}
	void operator *=(T_Type scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}
	void operator /=(T_Type denominator)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
	}

	//-----------------------------------------------------------------------------
	// Binary operators
	//-----------------------------------------------------------------------------

	Vector3 operator +(const Vector3& other) const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}
	Vector3 operator -(const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
	Vector3 operator *(const Vector3& other) const
	{
		return Vector3(x * other.x, y * other.y, z * other.z);
	}
	Vector3 operator /(const Vector3& other) const
	{
		return Vector3(x / other.x, y / other.y, z / other.z);
	}
	Vector3 operator *(T_Type scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}
	Vector3 operator /(T_Type denominator) const
	{
		return Vector3(x / denominator, y / denominator, z / denominator);
	}

	// implementation for use as a comparator
	bool operator <(const Vector3& other) const
	{
		if (x < other.x)
			return true;
		else if (x == other.x)
		{
			if (y < other.y)
				return true;
			else if (y == other.y)
				return (z < other.z);
		}
		return false;
	}
};


// Left-hand Operators
template <typename T_Type>
Vector3<T_Type> operator *(T_Type scalar, const Vector3<T_Type>& v)
{
	return Vector3(scalar * v.x, scalar * v.y, scalar * v.z);
}

template <typename T_Type>
Vector3<T_Type> operator /(T_Type numerator, const Vector3<T_Type>& v)
{
	return Vector3(numerator / v.x, numerator / v.y, numerator / v.z);
}

template <typename T_Type>
std::ostream& operator <<(std::ostream &out, const Vector3<T_Type>& v)
{
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

template <typename T_Type>
const Vector3<T_Type> Vector3<T_Type>::ZERO = Vector3<T_Type>(0, 0, 0);
template <typename T_Type>
const Vector3<T_Type> Vector3<T_Type>::ONE = Vector3<T_Type>(1, 1, 1);
template <typename T_Type>
const Vector3<T_Type> Vector3<T_Type>::UNITX = Vector3<T_Type>(1, 0, 0);
template <typename T_Type>
const Vector3<T_Type> Vector3<T_Type>::UNITY = Vector3<T_Type>(0, 1, 0);
template <typename T_Type>
const Vector3<T_Type> Vector3<T_Type>::UNITZ = Vector3<T_Type>(0, 0, 1);


typedef Vector3<int32> Vector3i;
typedef Vector3<uint32> Vector3ui;


#endif // __CMG_MATH_TYPES_VECTOR_3_H__