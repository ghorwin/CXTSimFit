# Project file for CXTSimFit
#
# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# set LD_LIBRARY_PATH on Linux

TARGET = CXTSimFit
TEMPLATE = app

# this pri must be sourced from all our applications
include( ../../../externals/IBK/projects/Qt/IBK.pri )

QT += printsupport widgets svg

CONFIG += c++11

unix {
	QMAKE_CXXFLAGS += -Wno-deprecated-copy
}

LIBS += -L../../../lib$${DIR_PREFIX} \
	-lIBK \
	-llevmar \
	-lsundials 

win32 {
	LIBS += -luser32
}

INCLUDEPATH = \
	../../src \
	../../../externals/IBK/src \
	../../../externals/levmar/src \
	../../../externals/sundials/src/include

DEPENDPATH = $${INCLUDEPATH}

win32 {
PRE_TARGETDEPS += \
	$$PWD/../../../externals/lib$${DIR_PREFIX}/IBK.lib \
	$$PWD/../../../externals/lib$${DIR_PREFIX}/sundials.lib \
	$$PWD/../../../externals/lib$${DIR_PREFIX}/levmar.lib \
}



TRANSLATIONS += ../../resources/translations/CXTSimFit_de.ts
CODECFORSRC = UTF-8

RESOURCES += \
	../../resources/CXTSimFit.qrc 





