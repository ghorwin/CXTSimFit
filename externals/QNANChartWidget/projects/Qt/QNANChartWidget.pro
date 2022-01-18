# qmake project file to build the plugin and the widget

TEMPLATE 	= lib
QT += widgets xml printsupport

DESTDIR = ../../../lib

FORMS += \
	../../src/qnancharteditdialog.ui \
	../../src/qnanprintchartdialog.ui \
	../../src/qnanserieseditdialog.ui

HEADERS += \
	../../src/qnanabstractchartseries.h \
	../../src/qnanchartaxis.h \
	../../src/qnancharteditdialog.h \
	../../src/QNANChartWidget \
	../../src/qnanchartwidget.h \
	../../src/qnandefaultchartseries.h \
	../../src/qnanprintchartdialog.h \
	../../src/qnanserieseditdialog.h \
	../../src/qnanseriestabledelegate.h

SOURCES += \
	../../src/qnanabstractchartseries.cpp \
	../../src/qnanchartaxis.cpp \
	../../src/qnancharteditdialog.cpp \
	../../src/qnanchartwidget.cpp \
	../../src/qnandefaultchartseries.cpp \
	../../src/qnanprintchartdialog.cpp \
	../../src/qnanserieseditdialog.cpp \
	../../src/qnanseriestabledelegate.cpp


