#ifndef _CMG_PHYSICS_COLLISION_DETECTOR_H_
#define _CMG_PHYSICS_COLLISION_DETECTOR_H_

#include <cmgPhysics/cmgContact.h>
#include <cmgPhysics/cmgRigidBody.h>
#include <cmgPhysics/colliders/cmgBoxCollider.h>
#include <cmgPhysics/colliders/cmgSphereCollider.h>
#include <cmgPhysics/colliders/cmgCapsuleCollider.h>
#include <cmgPhysics/colliders/cmgCylinderCollider.h>
#include <cmgPhysics/colliders/cmgConeCollider.h>
#include <cmgPhysics/cmgGJK.h>


//-----------------------------------------------------------------------------
// CollisionDetector - Narrow phase collision detector.
//-----------------------------------------------------------------------------
class CollisionDetector
{
public:
	CollisionDetector();
	
	void DetectCollision(RigidBody* one, RigidBody* two, CollisionData* collisionData);

	void DetectCollision(Collider* a, Collider* b, CollisionData* collisionData);
	void GenerateContactsEPA(Collider* a, Collider* b, CollisionData* collisionData, const EPAResult& epa);

	void DetectCollision(CollisionPrimitive* one, CollisionPrimitive* two, CollisionData* collisionData);

	unsigned int CollideSphereAndSphere(
		const CollisionSphere& one,
		const CollisionSphere& two,
		CollisionData* collisionData);

	unsigned int CollideBoxAndSphere(
		const CollisionBox& box,
		const CollisionSphere& sphere,
		CollisionData* collisionData);

	unsigned int CollideBoxAndBox(
		const CollisionBox& one,
		const CollisionBox& two,
		CollisionData* collisionData);

	Vector3f GetEdgeContactPoint(
		const Vector3f& axisOne,
		const Vector3f& axisTwo,
		const Vector3f& pointOnEdgeOne,
		const Vector3f& pointOnEdgeTwo);

private:
};


#endif // _CMG_PHYSICS_COLLISION_DETECTOR_H_