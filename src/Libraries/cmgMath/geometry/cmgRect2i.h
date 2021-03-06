#ifndef _CMG_RECT_2I_H_
#define _CMG_RECT_2I_H_

#include <cmgMath/types/cmgVector2.h>


// Axis-aligned, 2D floating-point rectangle.
struct Rect2i
{
	Vector2i position, size;

	
	// Constants.
	static const Rect2i ZERO;

	// Constructors.
	Rect2i();
	Rect2i(int x, int y, int width, int height);
	Rect2i(const Vector2i& position, const Vector2i& size);

	// Accessors.
	inline int		GetX()				const { return position.x; }
	inline int		GetY()				const { return position.y; }
	inline int		GetWidth()			const { return size.x; }
	inline int		GetHeight()			const { return size.y; }
	inline int		GetLeft()			const { return position.x; }
	inline int		GetTop()			const { return position.y; }
	inline int		GetRight()			const { return position.x + size.x; }
	inline int		GetBottom()			const { return position.y + size.y; }
	inline int		GetArea()			const { return size.x * size.y; }
	inline int		GetParimeter()		const { return 2 * (size.x + size.y); }
	inline Vector2i	GetTopLeft()		const { return position; }
	inline Vector2i	GetTopRight()		const { return Vector2i(position.x + size.x, position.y); }
	inline Vector2i	GetBottomLeft()		const { return Vector2i(position.x, position.y + size.y); }
	inline Vector2i	GetBottomRight()	const { return position + size; }
	inline Vector2i	GetCenter()			const { return position + (size / 2); }
	bool Contains(const Vector2i& v)		const;
	bool Contains(const Rect2i& r)		const;
	bool Intersects(const Rect2i& r)	const;

	// Mutators.
	inline void SetX(int x)				{ position.x = x; }
	inline void SetY(int y)				{ position.y = y; }
	inline void SetWidth(int width)		{ size.x = width; }
	inline void SetHeight(int height)	{ size.y = height; }
	Rect2i& Set(int x, int y, int width, int height);
	Rect2i& Set(const Vector2i& position, const Vector2i& size);
	Rect2i& Inset(int left, int top, int right, int bottom);
	Rect2i& Inset(int horizontal, int vertical);
	Rect2i& Inflate(int left, int top, int right, int bottom);
	Rect2i& Inflate(int horizontal, int vertical);
	Rect2i& Translate(const Vector2i& amount);
	Rect2i& Translate(int x, int y);

	// Static Methods.
	static Rect2i GetUnion(const Rect2i& a, const Rect2i& b);
	static Rect2i GetIntersect(const Rect2i& a, const Rect2i& b);
};


#endif // _CMG_RECT_2I_H_