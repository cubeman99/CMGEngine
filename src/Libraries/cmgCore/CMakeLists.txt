
set(CMG_CORE_SOURCE_FILES
	
	cmg_core.h

	cmgBase.h
	cmgAssert.h
	cmgAssert.cpp
	cmgConsole.h
	cmgConsole.cpp
	cmgRandom.h
	cmgRandom.cpp
	cmgEventSystem.h
	cmgEventSystem.cpp

	containers/cmgArray.h
	containers/cmgArray.cpp

	ecs/cmgECS.h
	ecs/cmgECS.cpp
	ecs/cmgECSComponent.h
	ecs/cmgECSComponent.cpp
	ecs/cmgECSComponentPool.h
	ecs/cmgECSComponentPool.cpp
	ecs/cmgECSSystem.h
	ecs/cmgECSSystem.cpp

	error/cmgError.h
	error/cmgError.cpp

	os/cmgClipboard.h
	os/cmgClipboard.cpp

	resource/cmgResource.h
	resource/cmgResource.cpp
	resource/cmgResourceLoader.h
	resource/cmgResourceLoader.cpp

	io/cmgFile.h
	io/cmgFile.cpp
	io/cmgPath.h
	io/cmgPath.cpp

	log/cmgLogging.h
	log/cmgLogging.cpp

	string/cmgString.h
	string/cmgString.cpp

	time/cmgTimer.h
	time/cmgTimer.cpp

	smart_ptr/cmg_smart_ptr.h
	smart_ptr/cmgSharedPtr.h
)

cmg_install_library(cmgCore "${CMG_CORE_SOURCE_FILES}")

#target_link_libraries(cmgCore
#	${OPENGL_LIBRARIES}
#	${DIRECTINPUT_LIBRARIES}
#	FreeType
#)
