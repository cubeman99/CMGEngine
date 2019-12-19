
set(CMG_APPLICATION_SOURCE_FILES
	
	cmg_application.h

	cmgApplication.h
	cmgApplication.cpp
	cmgResourceManager.h
	cmgResourceManager.cpp

)

cmg_install_library(cmgApplication "${CMG_APPLICATION_SOURCE_FILES}")

target_link_libraries(cmgApplication
	cmgCore
	cmgMath
	cmgInput
	cmgGraphics
)
