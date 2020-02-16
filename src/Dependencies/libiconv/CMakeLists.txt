
set(BASE_SRCS
	include/iconv.h
	include/export.h
	src/iconv.c
	src/iconv_no_i18n.c
	srclib/uniwidth/width.c
	srclib/progname.c
	srclib/safe-read.c
	lib/iconv.c
	libcharset/lib/localcharset.c
)

# Specify library include directories
include_directories(include)
include_directories(src)
include_directories(srclib)
include_directories(lib)

cmg_install_dependency(libiconv "${BASE_SRCS}")

install(FILES "include/iconv.h" DESTINATION "include")
