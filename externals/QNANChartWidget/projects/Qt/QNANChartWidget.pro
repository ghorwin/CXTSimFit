# qmake project file to build the plugin and the widget

# first we define what we are
TARGET = QNANChartWidget
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )

QT += printsupport widgets svg xml

HEADERS += \
	../../src/qnanabstractchartseries.h \
	../../src/qnanchartaxis.h \
	../../src/QNANChartWidget \
	../../src/qnanchartwidget.h \
	../../src/qnandefaultchartseries.h

SOURCES += \
	../../src/qnanabstractchartseries.cpp \
	../../src/qnanchartaxis.cpp \
	../../src/qnanchartwidget.cpp \
	../../src/qnandefaultchartseries.cpp


contains( OPTIONS, plugin ) {
	FORMS += \
		../../src/qnancharteditdialog.ui \
		../../src/qnanprintchartdialog.ui \
		../../src/qnanserieseditdialog.ui

	HEADERS += \
		../../src/qnancharteditdialog.h \
		../../src/qnanprintchartdialog.h \
		../../src/qnanserieseditdialog.h \
		../../src/qnanseriestabledelegate.h

	SOURCES += \
		../../src/qnancharteditdialog.cpp \
		../../src/qnanprintchartdialog.cpp \
		../../src/qnanserieseditdialog.cpp \
		../../src/qnanseriestabledelegate.cpp

}
