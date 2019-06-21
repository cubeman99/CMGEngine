#ifndef _CMG_PHYSICS_MOTION_INTEGRATORS_H_
#define _CMG_PHYSICS_MOTION_INTEGRATORS_H_

#include <cmgMath/cmgMathLib.h>
#include <cmgMath/types/cmgTransform3f.h>

namespace integrators
{

inline void Verlet(Vector3f& position, Vector3f& velocity,
	const Vector3f& acceleration, float delta)
{
	float halfDelta = delta * 0.5f;
	position += velocity * halfDelta;
	velocity += acceleration * delta;
	position += velocity * halfDelta;
}

inline void ForestRuth(Vector3f& position, Vector3f& velocity,
	const Vector3f& acceleration, float delta)
{
	static const float frCoefficient = 1.0f / (2.0f - Math::Pow(2.0f, 1.0f / 3.0f));
	static const float frComplement = 1.0f - 2.0f * frCoefficient;
	Verlet(position, velocity, acceleration, delta * frCoefficient);
	Verlet(position, velocity, acceleration, delta * frComplement);
	Verlet(position, velocity, acceleration, delta * frCoefficient);
}

inline void ModifiedEuler(Vector3f& position, Vector3f& velocity,
	const Vector3f& acceleration, float delta)
{
	velocity += acceleration * delta;
	position += velocity * delta;
}

}

#endif // _CMG_PHYSICS_MOTION_INTEGRATORS_H_