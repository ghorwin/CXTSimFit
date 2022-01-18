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
	-lQNANChartWidget \
	-llevmar \
	-lsundials

win32 {
	LIBS += -luser32
}

INCLUDEPATH = \
	../../src \
	../../../externals/IBK/src \
	../../../externals/levmar/src \
	../../../externals/QNANChartWidget/src \
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

FORMS += \
	../../src/aboutdialog.ui \
	../../src/cxtsimfit.ui \
	../../src/inspectprofiledialog.ui

HEADERS += \
	../../src/aboutdialog.h \
	../../src/curvedata.h \
	../../src/cxtsimfit.h \
	../../src/inspectprofiledialog.h \
	../../src/levmaroptimizer.h \
#	../../src/optimizer.h \
	../../src/solver.h \
	../../src/solverinput.h \
	../../src/solverresults.h

SOURCES += \
	../../src/aboutdialog.cpp \
	../../src/curvedata.cpp \
	../../src/cxtsimfit.cpp \
	../../src/inspectprofiledialog.cpp \
	../../src/levmaroptimizer.cpp \
	../../src/main.cpp \
#	../../src/optimizer.cpp \
	../../src/solver.cpp \
	../../src/solverinput.cpp \
	../../src/solverresults.cpp





