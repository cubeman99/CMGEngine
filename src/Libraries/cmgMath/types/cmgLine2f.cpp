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


// =================== OPERTAORS =================== //


