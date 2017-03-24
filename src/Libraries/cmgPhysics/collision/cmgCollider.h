#ifndef _CMG_PHYSICS_COLLISION_COLLIDER_H_
#define _CMG_PHYSICS_COLLISION_COLLIDER_H_

#include <cmgMath/types/cmgMatrix4f.h>

class RigidBody;


enum class ColliderType
{
	k_unknown = -1,

	k_sphere = 0,
	k_box,
	k_plane,

	k_cylinder,
	k_capsule,
	k_polyhedra,

	k_count,
};


class Collider
{
public:
	Collider();

private:

	// The body this collider is a part of.
	RigidBody* m_body;
	
	// The offset of this shape from the body.
	// Transforms from body space to local primitive space.
	Matrix4f m_bodyToShape;
};


#endif // _CMG_PHYSICS_COLLISION_COLLIDER_H_