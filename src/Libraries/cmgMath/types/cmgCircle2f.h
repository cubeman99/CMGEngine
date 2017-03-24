#ifndef _CIRCLE_2F_H_
#define _CIRCLE_2F_H_

#include <cmgMath/types/cmgVector2f.h>
#include <cmgMath/cmgMathLib.h>


// 2D floating-point circle.
struct Circle2f
{
	Vector2f position;
	float  radius;

	// Constants.
	static const Circle2f ZERO;
	static const Circle2f UNIT;

	// Constructors.
	Circle2f();
	Circle2f(float x, float y, float radius);
	Circle2f(const Vector2f& position, float radius);

	// Accessors.
	inline float getX()				const { return position.x; }
	inline float getY()				const { return position.y; }
	inline float getDiameter()		const { return radius * 2.0f; }
	inline float getCircumference()	const { return (Math::TWO_PI * radius); }
	inline float getArea()			const { return (Math::PI * radius * radius); }

	// Mutators.
	Circle2f& set(float x, float y, float radius);
	Circle2f& set(const Vector2f& center, float radius);

	// Static Methods.
	static Circle2f circumscribe(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3);
};


#endif // _CIRCLE_2F_H_