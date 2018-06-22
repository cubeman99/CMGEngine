
set(CMG_CORE_TESTS
	cmgCoreTestsMain.cpp
	cmgLogUtilityTests.cpp
	cmgTimerUtilityTests.cpp
	cmgECSTests.cpp
)

add_executable(cmgCoreTests
	${CMG_CORE_TESTS})

target_link_libraries(cmgCoreTests
	cmgCore
)

cmg_install_test(cmgCoreTests "${CMG_CORE_TESTS}")

