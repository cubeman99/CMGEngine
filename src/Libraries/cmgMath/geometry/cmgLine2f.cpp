#include "cmgLine2f.h"
#include <cmgMath/cmgMathLib.h>


// =================== CONSTANTS =================== //

const Line2f Line2f::ZERO = Line2f(0.0f, 0.0f, 0.0f, 0.0f);



// ================== CONSTRUCTORS ================== //

// Construct a line with uninitialized components.
Line2f::Line2f()
{
}

// Construct a line with the given end-point coordinates.
Line2f::Line2f(float x1, float y1, float x2, float y2) :
end1(x1, y1), end2(x2, y2)
{
}

// Construct a line with the given end-points.
Line2f::Line2f(const Vector2f& end1, const Vector2f& end2) :
end1(end1), end2(end2)
{
}



// =================== ACCESSORS =================== //

float Line2f::getLength() const
{
	return (end2 - end1).Length();
}

float Line2f::getLengthSqr() const
{
	return ((end2.x - end1.x) * (end2.x - end1.x) +
		(end2.y - end1.y) * (end2.y - end1.y));
}


// ==================== MUTATORS ==================== //

Line2f& Line2f::set(float x1, float y1, float x2, float y2)
{
	end1.Set(x1, y1);
	end2.Set(x2, y2);
	return *this;
}

Line2f& Line2f::set(const Vector2f& end1, const Vector2f& end2)
{
	this->end1 = end1;
	this->end2 = end2;
	return *this;
}

//-----------------------------------------------------------------------------
// Static Methods
//-----------------------------------------------------------------------------

Vector2f Line2f::GetLineIntersection(
	const Vector2f& ps1, const Vector2f& pe1,
	const Vector2f& ps2, const Vector2f& pe2)
{
	// "2D Line Intersection in C#" By mas (2013)
	// http://www.wyrmtale.com/blog/2013/115/2d-line-intersection-in-c

	// Get A,B,C of first line - points : ps1 to pe1
	float A1 = pe1.y - ps1.y;
	float B1 = ps1.x - pe1.x;
	float C1 = (A1 * ps1.x) + (B1 * ps1.y);

	// Get A,B,C of second line - points : ps2 to pe2
	float A2 = pe2.y - ps2.y;
	float B2 = ps2.x - pe2.x;
	float C2 = (A2 * ps2.x) + (B2 * ps2.y);

	// Get delta and check if the lines are parallel
	float delta = (A1 * B2) - (A2 * B1);
	if (delta == 0.0f)
		return Vector2f::ZERO; // lines are parallel!

	// now return the Vector2 intersection point
	return Vector2f(
		((B2 * C1) - (B1 * C2)) / delta,
		((A1 * C2) - (A2 * C1)) / delta);
}
