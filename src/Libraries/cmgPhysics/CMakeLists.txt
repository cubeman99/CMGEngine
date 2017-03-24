
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
	
	collision/cmgCollider.h
	collision/cmgCollider.cpp
	collision/cmgBoxCollider.h
	collision/cmgBoxCollider.cpp
	collision/cmgSphereCollider.h
	collision/cmgSphereCollider.cpp
)

cmg_install_library(cmgPhysics "${CMG_PHYSICS_SOURCE_FILES}")

target_link_libraries(cmgPhysics
	cmgMath
	cmgCore
)
