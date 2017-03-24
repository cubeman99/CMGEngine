#ifndef _POINT_3I_H_
#define _POINT_3I_H_

#include <iostream>

struct Point2i;
struct Vector3f;


// 3D integer vector.
struct Point3i
{
	int x, y, z;
	

	// Constants.
	static const Point3i ZERO;
	static const Point3i ONE;
	static const Point3i UNITX;
	static const Point3i UNITY;
	static const Point3i UNITZ;

	// Constructors.
	Point3i();
	Point3i(int x, int y, int z);
	Point3i(const Point2i& xy, int z);
	explicit Point3i(const Vector3f& vVec);
	explicit Point3i(int value);
	
	// Accessors.
	int			operator [](int index) const;
	int&		operator [](int index);
	Point2i		GetXY() const;
	Point2i		GetXZ() const;
	Point2i		GetYZ() const;
	Point2i		GetYX() const;
	Point2i		GetZX() const;
	Point2i		GetZY() const;

	// Mutators.
	Point3i&	SetZero();
	Point3i&	Set(int x, int y, int z);
	Point3i&	Fill(int value);
	Point3i&	Negate();

	// Unary operators.
	Point3i		operator -() const;
	void		operator +=(const Point3i& v);
	void		operator -=(const Point3i& v);
	void		operator *=(const Point3i& v);
	void		operator /=(const Point3i& v);
	void		operator *=(int scalar);
	void		operator /=(int denominator);

	// Binary operators.
	Point3i		operator +(const Point3i& other) const;
	Point3i		operator -(const Point3i& other) const;
	Point3i		operator *(const Point3i& other) const;
	Point3i		operator /(const Point3i& other) const;
	Point3i		operator *(int scalar) const;
	Point3i		operator /(int denominator) const;
};


// Left-hand Operators.
Vector3f		operator *(float scalar, const Point3i& v);
Vector3f		operator /(float numerator, const Point3i& v);
std::ostream&	operator <<(std::ostream &out, const Point3i& v);


typedef Point3i Vector3i;


#endif // _POINT_3I_H_