
set(CMG_PHYSICS_SOURCE_FILES
	
	cmg_physics.h

	cmgMotionIntegrators.h

	cmgRigidBody.h
	cmgRigidBody.cpp

	cmgPhysicsEngine.h
	cmgPhysicsEngine.cpp
	cmgPhysicsPrimitives.h
	cmgPhysicsPrimitives.cpp
	cmgPhysicsMesh.h
	cmgPhysicsMesh.cpp
	cmgCollisionDetector.h
	cmgCollisionDetector.cpp
	cmgCollisionCache.h
	cmgCollisionCache.cpp

	cmgContact.h
	cmgContact.cpp
	
	cmgGJK.h
	cmgGJK.cpp

	cmgQuickHull.h
	cmgQuickHull.cpp

	colliders/cmgCollider.h
	colliders/cmgCollider.cpp
	colliders/cmgBoxCollider.h
	colliders/cmgBoxCollider.cpp
	colliders/cmgSphereCollider.h
	colliders/cmgSphereCollider.cpp
	colliders/cmgCylinderCollider.h
	colliders/cmgCylinderCollider.cpp
	colliders/cmgCapsuleCollider.h
	colliders/cmgCapsuleCollider.cpp
	colliders/cmgConeCollider.h
	colliders/cmgConeCollider.cpp
	colliders/cmgPolygonCollider.h
	colliders/cmgPolygonCollider.cpp
	colliders/cmgConvexMeshCollider.h
	colliders/cmgConvexMeshCollider.cpp

	ecs/cmgPhysicsComponents.h
)

cmg_install_library(cmgPhysics "${CMG_PHYSICS_SOURCE_FILES}")

target_link_libraries(cmgPhysics
	cmgMath
	cmgCore
)
