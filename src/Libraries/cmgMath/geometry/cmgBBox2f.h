#ifndef _BBOX_2F_H_
#define _BBOX_2F_H_

#include <cmgMath/types/cmgVector2f.h>

struct Rect2f;


// Axis-aligned, 3D floating-point rectangle
// stored with min and max points.
struct BBox2f
{
	Vector2f mins, maxs;

	// Constructors.
	BBox2f();
	BBox2f(const Rect2f& rect);
	BBox2f(const Vector2f& mins, const Vector2f& maxs);
	BBox2f(float xMin, float yMin, float xMax, float yMax);

	// Accessors.
	inline float  GetArea()		const { return (maxs.x - mins.x) * (maxs.y - mins.y); }
	inline Vector2f GetCenter()		const { return (mins + maxs) * 0.5f; }
	inline Vector2f GetSize()		const { return (maxs - mins); }
	inline Vector2f GetExtents()	const { return (maxs - mins) * 0.5f; }
	bool			Contains(const Vector2f& point) const;
	bool			Intersects(const BBox2f& bounds) const;

	// Mutators.
	BBox2f& SetMinMax(const Vector2f& mins, const Vector2f& maxs);
	BBox2f& SetCenterSize(const Vector2f& center, const Vector2f& size);
	BBox2f& Expand(float amount);
	BBox2f& Expand(const Vector2f& amount);
	BBox2f& Translate(const Vector2f& amount);
	BBox2f& Encapsulate(const Vector2f& point);

	// Operators.
	void operator=(const Rect2f& rect);
	BBox2f operator+(const Vector2f& translation);

	// Static methods.
	static BBox2f GetUnion(const BBox2f& a, const BBox2f& b);
};


#endif // _BBOX_2F_H_