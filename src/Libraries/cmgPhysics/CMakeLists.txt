
set(CMG_PHYSICS_SOURCE_FILES
	
	cmg_physics.h

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

	cmgContact.h
	cmgContact.cpp
	
	cmgGJK.h
	cmgGJK.cpp

	colliders/cmgCollider.h
	colliders/cmgCollider.cpp
	colliders/cmgBoxCollider.h
	colliders/cmgBoxCollider.cpp
	colliders/cmgSphereCollider.h
	colliders/cmgSphereCollider.cpp
	colliders/cmgCylinderCollider.h
	colliders/cmgCylinderCollider.cpp
	colliders/cmgConeCollider.h
	colliders/cmgConeCollider.cpp
)

cmg_install_library(cmgPhysics "${CMG_PHYSICS_SOURCE_FILES}")

target_link_libraries(cmgPhysics
	cmgMath
	cmgCore
)
