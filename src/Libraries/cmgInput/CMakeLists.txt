
set(CMG_INPUT_SOURCE_FILES
	
	cmg_input.h

	cmgInputDevice.h
	cmgInputDevice.cpp
	cmgInputManager.h
	cmgInputManager.cpp
	cmgKeyboard.h
	cmgKeyboard.cpp
	cmgMouse.h
	cmgMouse.cpp

)

cmg_install_library(cmgInput "${CMG_INPUT_SOURCE_FILES}")


target_link_libraries(cmgInput
	cmgCore
	cmgMath
	cmgGraphics
	${DIRECTINPUT_LIBRARIES}
)

#target_link_libraries(${target_name}
		#${OPENGL_LIBRARIES}
		#${CMG_DEP_INSTALL_PATH}/lib/Debug/dinput8.lib
	#)