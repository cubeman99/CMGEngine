
#------------------------------------------------------------------------------
# Declare projects.

set(SOLUTION_DEPENDENCY_PROJECTS
	FreeType
)

set(SOLUTION_LIBRARY_PROJECTS
	CMGEngine
)

set(SOLUTION_TEST_PROJECTS
	CMGTests
)


set(SOLUTION_LIBRARY_PROJECTS_FOLDER "Libraries")
set(SOLUTION_TEST_PROJECTS_FOLDER "Tests")


# Create a regex of the file extensions to include in the distribution.
if(DISTRIBUTION_FULL_SOURCE)
	set(DISTRIBUTION_FILE_EXTENSIONS "^(.h|.cpp)$")
else()
	set(DISTRIBUTION_FILE_EXTENSIONS "^(.h)$")
endif()

#------------------------------------------------------------------------------
# Install function for dependencies.

function(cmg_install_dependency target_name target_source_files)
	message (STATUS " * Installing dependency [${target_name}]")
	
	# Add the library project.
	add_library(${target_name} STATIC
		${target_source_files})
	
	#target_link_libraries(${target_name}
		#${OPENGL_LIBRARIES}
		#${CMG_DEP_INSTALL_PATH}/lib/Debug/dinput8.lib
	#)
	
	# Set the include directory.
	#include_directories(
	#	"../"
	#	"./"
	#	 ${OPENGL_INCLUDE_DIRS}
	#	 #"${CMG_DEP_INSTALL_PATH}/include"
	#)
	
	# Install the target.
	install (TARGETS ${target_name} DESTINATION "lib")
	
	# Put the project in the "Libraries" folder
	set_target_properties(${lib} PROPERTIES
		FOLDER ${SOLUTION_LIBRARY_PROJECTS_FOLDER})
	
	# Install the target's source files.
	foreach(source_file ${target_source_files})
		# Get the directory component of the file.
		get_filename_component(source_file_dir ${source_file} DIRECTORY)
				
		# Assign the file a source group.
		if (NOT "${source_file_dir}" STREQUAL "")
			source_group(${source_file_dir} FILES ${source_file})
		endif()
		
		# Install the file (include it in the distribution).
		get_filename_component(extension ${source_file} EXT)
		if (${extension} MATCHES "${DISTRIBUTION_FILE_EXTENSIONS}")
			message (STATUS "   + ${source_file}")
			install (FILES ${source_file}
				DESTINATION "include/${target_name}/${source_file_dir}")
		else()
			message (STATUS "   ~ ${source_file}")
		endif()
	endforeach(source_file)
endfunction()

#------------------------------------------------------------------------------
# Install function for libraries.

function(cmg_install_library target_name target_source_files)
	message (STATUS " * Installing library [${target_name}]")
	
	# Add the library project.
	add_library(${target_name} STATIC
		${target_source_files})
	
	target_link_libraries(${target_name}
		${OPENGL_LIBRARIES}
		#${CMG_DEP_INSTALL_PATH}/lib/Debug/dinput8.lib
	)
	
	# Set the include directory.
	include_directories(
		"../"
		"./"
		"../../${SOLUTION_DEPENDENCY_PROJECTS_FOLDER}/FreeType/include"
		"../../${SOLUTION_DEPENDENCY_PROJECTS_FOLDER}"
		 ${OPENGL_INCLUDE_DIRS}
		 #"${CMG_DEP_INSTALL_PATH}/include"
	)
	
	# Install the target.
	install (TARGETS ${target_name} DESTINATION "lib")
	
	# Put the project in the "Libraries" folder
	set_target_properties(${lib} PROPERTIES
		FOLDER ${SOLUTION_LIBRARY_PROJECTS_FOLDER})
	
	# Install the target's source files.
	foreach(source_file ${target_source_files})
		# Get the directory component of the file.
		get_filename_component(source_file_dir ${source_file} DIRECTORY)
				
		# Assign the file a source group.
		if (NOT "${source_file_dir}" STREQUAL "")
			source_group(${source_file_dir} FILES ${source_file})
		endif()
		
		# Install the file (include it in the distribution).
		get_filename_component(extension ${source_file} EXT)
		if (${extension} MATCHES "${DISTRIBUTION_FILE_EXTENSIONS}")
			message (STATUS "   + ${source_file}")
			install (FILES ${source_file}
				DESTINATION "include/${target_name}/${source_file_dir}")
		else()
			message (STATUS "   ~ ${source_file}")
		endif()
	endforeach(source_file)
endfunction()


#------------------------------------------------------------------------------
# Install function for tests.

function(cmg_install_test target_name target_source_files)
	message (STATUS " * Installing test [${target_name}]")
	
	# Set the include directory.
	include_directories(
		"../"
		"./"
		"../../${SOLUTION_DEPENDENCY_PROJECTS_FOLDER}"
		 ${OPENGL_INCLUDE_DIRS}
		 #"${CMG_DEP_INSTALL_PATH}/include"
	)
	
	target_link_libraries(${target_name}
		${OPENGL_LIBRARIES}
		#${CMG_DEP_INSTALL_PATH}/lib/Debug/dinput8.lib
	)
	
	# Install the test into the lib folder.
	install (TARGETS ${target_name} DESTINATION "lib")
	
	# Put the project in the "Tests" folder
	set_target_properties(${lib} PROPERTIES
		FOLDER ${SOLUTION_TEST_PROJECTS_FOLDER})
	
	# Assign source groups to the files.
	foreach(source_file ${target_source_files})
		# Get the directory component of the file.
		get_filename_component(source_file_dir ${source_file} DIRECTORY)
		
		# Assign the file a source group.
		if (NOT "${source_file_dir}" STREQUAL "")
			source_group(${source_file_dir} FILES ${source_file})
		endif()
		
		# Install the file (include it in the distribution).
		get_filename_component(extension ${source_file} EXT)
		if (DISTRIBUTION_FULL_SOURCE AND ${extension} MATCHES "${DISTRIBUTION_FILE_EXTENSIONS}")
			message (STATUS "   + ${source_file}")
			install (FILES ${source_file}
				DESTINATION "include/${target_name}/${source_file_dir}")
		endif()
	endforeach(source_file)
endfunction()

#------------------------------------------------------------------------------
# Add dependency projects.

message("")
foreach(lib ${SOLUTION_DEPENDENCY_PROJECTS})
	message(STATUS " + Adding dependency project: ${lib}")
	#set(SOLUTION_CURRENT_PROJECT_TYPE ${SOLUTION_PROJECT_TYPE_LIB})
	set(SOLUTION_CURRENT_PROJECT_NAME ${lib})
	add_subdirectory(Dependencies/${lib})
endforeach()

#------------------------------------------------------------------------------
# Add library projects.

message("")
foreach(lib ${SOLUTION_LIBRARY_PROJECTS})
	message(STATUS " + Adding library project: ${lib}")
	#set(SOLUTION_CURRENT_PROJECT_TYPE ${SOLUTION_PROJECT_TYPE_LIB})
	set(SOLUTION_CURRENT_PROJECT_NAME ${lib})
	add_subdirectory(Libraries/${lib})
endforeach()

#------------------------------------------------------------------------------
# Add test projects.

foreach(lib ${SOLUTION_TEST_PROJECTS})
	message(STATUS " + Adding test project: ${lib}")
	#set(SOLUTION_CURRENT_PROJECT_TYPE ${SOLUTION_PROJECT_TYPE_TEST})
	set(SOLUTION_CURRENT_PROJECT_NAME ${lib})
	add_subdirectory(Tests/${lib})
endforeach()

message("")
