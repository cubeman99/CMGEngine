#ifndef _LINE_2F_H_
#define _LINE_2F_H_

#include <cmgMath/types/cmgVector2f.h>


// 2D floating-point line segement.
struct Line2f
{
	Vector2f end1, end2;

	
	// Constants.
	static const Line2f ZERO;

	// Constructors
	Line2f();
	Line2f(float x1, float y1, float x2, float y2);
	Line2f(const Vector2f& end1, const Vector2f& end2);

	// Accessors.
	float getLength()		const;
	float getLengthSqr()	const;
	inline float	getX1()			const { return end1.x; }
	inline float	getY1()			const { return end1.y; }
	inline float	getX2()			const { return end2.x; }
	inline float	getY2()			const { return end2.y; }
	inline Vector2f getCenter()		const { return (end1 + end2) * 0.5f; }
	inline Vector2f getVector()		const { return end2 - end1; }

	// Mutators.
	inline void setX1(float x1) { end1.x = x1; }
	inline void setY1(float y1) { end1.x = y1; }
	inline void setX2(float x2) { end1.x = x2; }
	inline void setY2(float y2) { end1.x = y2; }
	Line2f& set(float x1, float y1, float x2, float y2);
	Line2f& set(const Vector2f& end1, const Vector2f& end2);

	// Static Methods.
	Vector2f getSegmentIntersection(const Line2f& a, const Line2f& b);
	Vector2f getLineIntersection(const Line2f& a, const Line2f& b);
};


#endif // _LINE_2F_H_