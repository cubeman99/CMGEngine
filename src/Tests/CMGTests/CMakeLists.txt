
set(CMG_GENERIC_TESTS
	main.cpp
	TestApp.h
	TestApp.cpp
	
	physics/PhysicsApp.h
	physics/PhysicsApp.cpp
	physics/CollisionTester.h
	physics/CollisionTester.cpp
	physics/PrimitiveMeshes.h
	physics/PrimitiveMeshes.cpp

	graphics/DebugDraw.h
	graphics/DebugDraw.cpp

	driving/DrivingEngine.h
	driving/DrivingEngine.cpp
	driving/Car.h
	driving/Car.cpp
	driving/TorqueCurve.h
	driving/TorqueCurve.cpp
	driving/Tire.h
	driving/Tire.cpp
	driving/Pacejka.h
	driving/Pacejka.cpp
	driving/TestMeshes.h
	driving/TestMeshes.cpp
	driving/diagrams/Tachometer.h
	driving/diagrams/Tachometer.cpp
	driving/diagrams/Speedometer.h
	driving/diagrams/Speedometer.cpp
	driving/Graph.h
	driving/Graph.cpp
	driving/Graphics.h
	driving/Graphics.cpp
	driving/SpriteFont.h
	driving/SpriteFont.cpp
)

add_executable(CMGTests
	${CMG_GENERIC_TESTS})

target_link_libraries(CMGTests
	cmgCore
	cmgMath
	cmgPhysics
	cmgGraphics
	cmgInput
	cmgApplication
)

cmg_install_test(CMGTests "${CMG_GENERIC_TESTS}")

