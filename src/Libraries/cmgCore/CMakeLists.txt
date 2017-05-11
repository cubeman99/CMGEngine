
set(CMG_CORE_SOURCE_FILES
	
	cmg_core.h

	cmgBase.h
	cmgAssert.h
	cmgAssert.cpp
	cmgConsole.h
	cmgConsole.cpp
	cmgRandom.h
	cmgRandom.cpp

	containers/cmgArray.h
	containers/cmgArray.cpp

	error/cmgError.h
	error/cmgError.cpp

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
)

cmg_install_library(cmgCore "${CMG_CORE_SOURCE_FILES}")

#target_link_libraries(cmgCore
#	${OPENGL_LIBRARIES}
#	${DIRECTINPUT_LIBRARIES}
#	FreeType
#)
