
set(CMG_GENERIC_TESTS
	main.cpp
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

