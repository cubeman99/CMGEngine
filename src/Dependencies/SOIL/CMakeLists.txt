
# Specify library include directories
#include_directories("include")

set(BASE_SRCS
  image_DXT.h
  image_DXT.c
  image_helper.h
  image_helper.c
  SOIL.h
  SOIL.c
  stb_image_aug.h
  stb_image_aug.c
  stbi_DDS_aug.h
  stbi_DDS_aug_c.h
)

include_directories(.)

cmg_install_dependency(SOIL "${BASE_SRCS}")
