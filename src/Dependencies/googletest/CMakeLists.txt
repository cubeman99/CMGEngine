
# Compiler definitions for building the library
#add_definitions(-DFT2_BUILD_LIBRARY)

# Specify library include directories
include_directories("include")

set(BASE_SRCS
  src/gtest-all.cc
)

#Copy files
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/include"
  DESTINATION "${CMG_INSTALL_PATH}/include/"
  FILE_PERMISSIONS OWNER_READ)
  ##FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.cpp")

include_directories(.)
include_directories(src)
#include_directories(src/sfnt)
#include_directories(src/autofit)
#include_directories(src/smooth)
#include_directories(src/raster)
#include_directories(src/psaux)
#include_directories(src/psnames)

cmg_install_dependency(googletest "${BASE_SRCS}")

#add_library(
  #FreeType STATIC
  #${BASE_SRCS}
#)

#set_platform_specific_properties(FreeType)
