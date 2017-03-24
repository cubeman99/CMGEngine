#include "cmgCircle2f.h"


// =================== CONSTANTS =================== //

const Circle2f Circle2f::ZERO = Circle2f(0.0f, 0.0f, 0.0f);
const Circle2f Circle2f::UNIT = Circle2f(0.0f, 0.0f, 1.0f);



// ================== CONSTRUCTORS ================== //

// Construct a circle with uninitialized components.
Circle2f::Circle2f()
{
}

// Construct a circle with the given radius and center position.
Circle2f::Circle2f(float x, float y, float radius) :
	position(x, y),
	radius(radius)
{
}

// Construct a circle with the given radius and center position.
Circle2f::Circle2f(const Vector2f& position, float radius) :
	position(position),
	radius(radius)
{
}



// =================== ACCESSORS =================== //


// ==================== MUTATORS ==================== //

Circle2f& Circle2f::set(float x, float y, float radius)
{
	return *this;
}

Circle2f& Circle2f::set(const Vector2f& center, float radius)
{
	return *this;
}



// ================ STATIC METHODS ================ //

// Create a circle that passes through 3 points.
Circle2f Circle2f::circumscribe(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3)
{
	return ZERO;
}

