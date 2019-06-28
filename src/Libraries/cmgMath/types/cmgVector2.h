#ifndef __CMG_MATH_TYPES_VECTOR_2_H__
#define __CMG_MATH_TYPES_VECTOR_2_H__

#include <iostream>
#include <cmgCore/cmgBase.h>
#include <cmgMath/cmgMathLib.h>

struct Vector2f;


// 2D integer vector.
template <typename T_Type>
struct Vector2
{
	union
	{
		struct { T_Type x; T_Type y; };
		struct { T_Type v[2]; };
	};

	

	//-------------------------------------------------------------------------
	// Constants
	//-------------------------------------------------------------------------

	static const Vector2 ZERO;	// (0, 0)
	static const Vector2 ONE;	// (1, 1)
	static const Vector2 UNITX;	// +X
	static const Vector2 UNITY;	// +Y


	//-------------------------------------------------------------------------
	// Constructors
	//-------------------------------------------------------------------------

	Vector2()
	{
	}
	Vector2(T_Type x, T_Type y) :
		x(x), y(y)
	{
	}
	Vector2(const Vector2<T_Type>& v) :
		x(v.x), y(v.y)
	{
	}
	template <typename Y_Type>
	explicit Vector2(const Vector2<Y_Type>& v) :
		x((T_Type) v.x),
		y((T_Type) v.y)
	{
	}
	explicit Vector2(const Vector2f& v) :
		x((T_Type) v.x),
		y((T_Type) v.y)
	{
	}
	explicit Vector2(T_Type value) :
		x(value), y(value)
	{
	}


	//-------------------------------------------------------------------------
	// Accessors
	//-------------------------------------------------------------------------

	T_Type operator [](T_Type index) const
	{
		CMG_ASSERT_MSG(index >= 0 && index < 2, "Invalid component index.");
		return v[index];
	}
	T_Type& operator [](T_Type index)
	{
		CMG_ASSERT_MSG(index >= 0 && index < 2, "Invalid component index.");
		return v[index];
	}
	Vector2 GetYX() const { return Vector2(y, x); }

	
	//-------------------------------------------------------------------------
	// Mutators
	//-------------------------------------------------------------------------

	Vector2& SetZero()
	{
		x = 0;
		y = 0;
		return *this;
	}
	Vector2& Set(T_Type x, T_Type y)
	{
		this->x = x;
		this->y = y;
		return *this;
	}
	Vector2& Fill(T_Type value)
	{
		x = value;
		y = value;
		return *this;
	}
	Vector2& Negate()
	{
		x = -x;
		y = -y;
		return *this;
	}


	//-------------------------------------------------------------------------
	// Unary operators
	//-------------------------------------------------------------------------

	Vector2 operator -() const
	{
		return Vector2(-x, -y);
	}
	void operator +=(const Vector2& v)
	{
		x += v.x;
		y += v.y;
	}
	void operator -=(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
	}
	void operator *=(const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
	}
	void operator /=(const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
	}
	void operator *=(T_Type scalar)
	{
		x *= scalar;
		y *= scalar;
	}
	void operator /=(T_Type denominator)
	{
		x /= denominator;
		y /= denominator;
	}


	//-------------------------------------------------------------------------
	// Binary operators
	//-------------------------------------------------------------------------

	Vector2 operator +(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator -(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	Vector2 operator *(const Vector2& other) const
	{
		return Vector2(x * other.x, y * other.y);
	}
	Vector2 operator /(const Vector2& other) const
	{
		return Vector2(x / other.x, y / other.y);
	}
	Vector2 operator *(T_Type scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}
	Vector2 operator /(T_Type denominator) const
	{
		return Vector2(x / denominator, y / denominator);
	}
};


//-----------------------------------------------------------------------------
// Left-hand Operators
//-----------------------------------------------------------------------------

template <typename T>
Vector2<T> operator *(T scalar, const Vector2<T>& v)
{
	return Vector2<T>(scalar * v.x, scalar * v.y);
}
template <typename T>
Vector2<T> operator /(T numerator, const Vector2<T>& v)
{
	return Vector2<T>(numerator / v.x, numerator / v.y);
}
template <typename T>
std::ostream& operator <<(std::ostream &out, const Vector2<T>& v)
{
	out << "(" << v.x << ", " << v.y << ")";
	return out;
}


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

template <typename T_Type>
const Vector2<T_Type> Vector2<T_Type>::ZERO = Vector2<T_Type>(0, 0);
template <typename T_Type>
const Vector2<T_Type> Vector2<T_Type>::ONE = Vector2<T_Type>(1, 1);
template <typename T_Type>
const Vector2<T_Type> Vector2<T_Type>::UNITX = Vector2<T_Type>(1, 0);
template <typename T_Type>
const Vector2<T_Type> Vector2<T_Type>::UNITY = Vector2<T_Type>(0, 1);


typedef Vector2<int32> Vector2i;
typedef Vector2<uint32> Vector2ui;


#endif // __CMG_MATH_TYPES_VECTOR_2_H__