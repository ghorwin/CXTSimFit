# -------------------------------------------------
# Project for levmar library
# -------------------------------------------------

# first we define what we are
TARGET = levmar
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )


# finally we setup our custom library specfic things
# like version number etc., we also may reset all
unix|mac {
	VER_MAJ = 2
	VER_MIN = 6
	VER_PAT = 0
	VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
}

INCLUDEPATH +=	\
	../../src

SOURCES += \
	../../src/Axb.c \
	../../src/levm.c \
	../../src/misc.c
