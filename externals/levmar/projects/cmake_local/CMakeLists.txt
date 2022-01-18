# CMakeLists.txt file for LevMar library

project(LevMar)

# add include directories
include_directories( ${PROJECT_SOURCE_DIR}/../../src )

# build library
add_library( LevMar
	src/Axb.c
	src/levm.c
	src/misc.c
)

