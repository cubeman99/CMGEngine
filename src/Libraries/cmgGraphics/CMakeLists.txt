
set(CMG_GRAPHICS_SOURCE_FILES
	
	cmg_graphics.h

	cmgWindow.h
	cmgWindow.cpp

	cmgOpenGLIncludes.h
	cmgImageFormat.h
	cmgImageFormat.cpp
	cmgTexture.h
	cmgTexture.cpp
	cmgTextureParams.h
	cmgTextureParams.cpp
	cmgShader.h
	cmgShader.cpp
	cmgVertexData.h
	cmgVertexData.cpp
	cmgMesh.h
	cmgMesh.cpp
	
	scene/cmgScene.h
	scene/cmgScene.cpp
	scene/cmgSceneNode.h
	scene/cmgSceneNode.cpp

	render/cmgRenderParams.h
	render/cmgRenderParams.cpp
	render/cmgRenderer.h
	render/cmgRenderer.cpp
	render/cmgGraphics2D.h
	render/cmgGraphics2D.cpp

	third_party/glew/GLEW.H
	third_party/glew/GLEW.C
	third_party/glew/wglew.h
	third_party/lodepng/lodepng.h
	third_party/lodepng/lodepng.cpp

	types/cmgColor.h
	types/cmgColor.cpp
)



cmg_install_library(cmgGraphics "${CMG_GRAPHICS_SOURCE_FILES}")

target_link_libraries(cmgGraphics
	cmgCore
	cmgMath
	FreeType
	${OPENGL_LIBRARIES}
)

#target_link_libraries(${target_name}
		#${OPENGL_LIBRARIES}
		#${CMG_DEP_INSTALL_PATH}/lib/Debug/dinput8.lib
	#)