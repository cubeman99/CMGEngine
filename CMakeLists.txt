cmake_minimum_required (VERSION 2.6)

# The engine version number.
set (CMG_VERSION_MAJOR 1)
set (CMG_VERSION_MINOR 0)

# Solution name.
set(SOLUTION_NAME "solution" CACHE STRING "You may change the name based on build configuration")
project(${SOLUTION_NAME})

# Allow projects to be put in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(internal_FIXME "FIXME!")


#------------------------------------------------------------------------------
# Engine CMake options:
#------------------------------------------------------------------------------


set(DISTRIBUTION_BUILD TRUE CACHE BOOL "Is this a distribution build?")
set(DISTRIBUTION_FULL_SOURCE TRUE CACHE BOOL "Include full source code with distribution")

##set(CMG_DEP_INSTALL_PATH ${internal_FIXME} CACHE PATH "The CMG dependency INSTALL path goes here.")
set(CMG_DEP_INSTALL_PATH "${CMAKE_SOURCE_DIR}/dep" CACHE PATH "The CMG dependency INSTALL path goes here.")
##set(CMG_ENGINE_INSTALL_PATH ${internal_FIXME} CACHE PATH "Install directory for the engine.")
set(CMG_INCLUDE_TESTS TRUE CACHE BOOL "Include tests in the project.")

set(SOLUTION_PATH "${CMAKE_SOURCE_DIR}" CACHE PATH "Root folder for this project. You do not need to change this path" FORCE)
set(SOLUTION_INSTALL_PATH "${CMAKE_SOURCE_DIR}/INSTALL" CACHE PATH "Install directory for the binaries")
set(SOLUTION_ASSETS_PATH "${CMAKE_SOURCE_DIR}/assets" CACHE PATH "You can specify your custom assets path here. NOTE: In Distribution build, the assets from this folder are copied to be relative to the binary files.")


set(CMG_ARCHITECTURE "x86")
set(DIRECTINPUT_LIBRARIES "${CMG_DEP_INSTALL_PATH}/lib/Debug/dinput8.lib")


#------------------------------------------------------------------------------
# Adjust build options.
#------------------------------------------------------------------------------

# Set CMake's install prefix (because we use the 'install' command)
set(CMAKE_INSTALL_PREFIX ${SOLUTION_INSTALL_PATH})

# Set the default solution name.
get_filename_component(SOLUTION_BUILD_FOLDER_NAME ${CMAKE_BINARY_DIR} NAME)
if(${SOLUTION_NAME} STREQUAL "solution")
	set(SOLUTION_NAME "cmgEngine_${SOLUTION_BUILD_FOLDER_NAME}" CACHE STRING "You may change the name based on build configurations" FORCE)
	project(${SOLUTION_NAME})
endif()

#------------------------------------------------------------------------------
# Print out some stuff.
#------------------------------------------------------------------------------

set(CMG_DEP_INSTALL_PATH "${CMAKE_SOURCE_DIR}/dep")

message (STATUS "CMAKE_CONFIGURATION_TYPES  = \"${CMAKE_CONFIGURATION_TYPES}\"")
message (STATUS "CMAKE_BUILD_TYPE = \"${CMAKE_BUILD_TYPE}\"")

message (STATUS "DISTRIBUTION_BUILD = ${DISTRIBUTION_BUILD}")
message (STATUS "DISTRIBUTION_FULL_SOURCE = ${DISTRIBUTION_FULL_SOURCE}")

message (STATUS "CMG_DEP_INSTALL_PATH = \"${CMG_DEP_INSTALL_PATH}\"")
#message (STATUS "CMG_ENGINE_INSTALL_PATH = \"${CMG_ENGINE_INSTALL_PATH}\"")
message (STATUS "CMG_INCLUDE_TESTS = ${CMG_INCLUDE_TESTS}")

message (STATUS "SOLUTION_NAME = \"${SOLUTION_NAME}\"")
message (STATUS "SOLUTION_PATH = \"${SOLUTION_PATH}\"")
message (STATUS "SOLUTION_INSTALL_PATH = \"${SOLUTION_INSTALL_PATH}\"")
message (STATUS "SOLUTION_ASSETS_PATH = \"${SOLUTION_ASSETS_PATH}\"")


#------------------------------------------------------------------------------

function(cmg_check_paths path_list)
	foreach(PATHDIR ${path_list})
		if (${${PATHDIR}} MATCHES ${internal_FIXME})
			message(FATAL_ERROR "Please enter a path for ${PATHDIR}")
		endif()
	endforeach()
endfunction()

#------------------------------------------------------------------------------

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/cmgVersion.in.h"
	"${PROJECT_SOURCE_DIR}/src/cmgVersion/cmgVersion.h"
)

# Find OpenGL package.
find_package(OpenGL)
message(STATUS "OPENGL_INCLUDE_DIRS = ${OPENGL_INCLUDE_DIRS}")
message(STATUS "OPENGL_LIBRARIES = ${OPENGL_LIBRARIES}")

# Add the source subdirectory.
add_subdirectory (src)
 
# add the install targets
#install (TARGETS Tutorial DESTINATION bin)
install (FILES "${PROJECT_SOURCE_DIR}/src/cmgVersion/cmgVersion.h"
		DESTINATION "include/cmgVersion")

