#include "cmgBBox2f.h"
#include <cmgMath/types/cmgRect2f.h>
#include <cmgMath/cmgMathLib.h>


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// Construct a bounding-box with uninitialized components.
BBox2f::BBox2f()
{
}

// Construct a bounding-box from a rectangle.
BBox2f::BBox2f(const Rect2f& rect) :
	mins(rect.position), maxs(rect.position + rect.size)
{
}

// Creates new BBox2f with a given min and max points.
BBox2f::BBox2f(const Vector2f& mins, const Vector2f& maxs) :
	mins(mins), maxs(maxs)
{
}

// Creates new BBox2f with a given min and max xy-components.
BBox2f::BBox2f(float xMin, float yMin, float xMax, float yMax) :
	mins(xMin, yMin), maxs(xMax, yMax)
{
}


//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

// Is point contained in the bounding-box?
bool BBox2f::Contains(const Vector2f& point) const
{
	return (point.x >= mins.x && point.x < maxs.x &&
			point.y >= mins.y && point.y < maxs.y);
}

// Does another bounding-box intersect with this bounding-box?
bool BBox2f::Intersects(const BBox2f& BBox2f) const
{
	return !(BBox2f.mins.x - maxs.x >= 0.0f || mins.x - BBox2f.maxs.x >= 0.0f ||
				BBox2f.mins.y - maxs.y >= 0.0f || mins.y - BBox2f.maxs.y >= 0.0f);
}


//-----------------------------------------------------------------------------
// Mutators
//-----------------------------------------------------------------------------

BBox2f& BBox2f::SetMinMax(const Vector2f& mins, const Vector2f& maxs)
{
	this->mins = mins;
	this->maxs = maxs;
	return *this;
}

// Creates new BBox2f with a given center and total size. Bound extents will be half the given size.
BBox2f& BBox2f::SetCenterSize(const Vector2f& center, const Vector2f& size)
{
	mins = center - (size * 0.5f);
	maxs = center + (size * 0.5f);
	return *this;
}

// Expand the BBox2f by increasing its size by amount along each side.
BBox2f& BBox2f::Expand(float amount)
{
	mins.x -= amount;
	mins.y -= amount;
	maxs.x += amount;
	maxs.y += amount;
	return *this;
}

// Expand the BBox2f by increasing its size by amount along each side.
BBox2f& BBox2f::Expand(const Vector2f& amount)
{
	mins -= amount;
	maxs += amount;
	return *this;
}

// Translates the position of the bounding-box by amount.
BBox2f& BBox2f::Translate(const Vector2f& amount)
{
	mins += amount;
	maxs += amount;
	return *this;
}

// Grows the BBox2f to include the point.
BBox2f& BBox2f::Encapsulate(const Vector2f& point)
{
	if (point.x < mins.x)
		mins.x = point.x;
	else if (point.x > maxs.x)
		maxs.x = point.x;

	if (point.y < mins.y)
		mins.y = point.y;
	else if (point.y > maxs.y)
		maxs.y = point.y;

	return *this;
}


//-----------------------------------------------------------------------------
// Operators
//-----------------------------------------------------------------------------

void BBox2f::operator=(const Rect2f& rect)
{
	mins = rect.position;
	maxs = rect.position + rect.size;
}

BBox2f BBox2f::operator+(const Vector2f& translation)
{
	return BBox2f(mins + translation, maxs + translation);
}



//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

BBox2f BBox2f::GetUnion(const BBox2f& a, const BBox2f& b)
{
	return BBox2f(Math::Min(a.mins.x, b.mins.x),
					Math::Min(a.mins.y, b.mins.y),
					Math::Max(a.maxs.x, b.maxs.x),
					Math::Max(a.maxs.y, b.maxs.y));
}

