
set(BASE_SRCS
	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzclose.c
	gzlib.c
	gzread.c
	gzwrite.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)

# Specify library include directories
include_directories(.)

cmg_install_dependency(zlib "${BASE_SRCS}")
