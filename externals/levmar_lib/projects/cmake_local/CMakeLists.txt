# CMakeLists.txt file for LevMar library

project(LevMar)

CMAKE_MINIMUM_REQUIRED( VERSION 2.4.5 )

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# add include directories
include_directories( src )

# build library
add_library( LevMar
	src/Axb.c
	src/levm.c
	src/misc.c
)

#install_target( /lib LevMar )
#file( glob LEVMAR_HEADER_FILES src/*.h)
#install_files( /include FILES ${LEVMAR_HEADER_FILES} )
