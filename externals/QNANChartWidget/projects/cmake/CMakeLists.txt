# -------------------------------------------------------------
# Initial commands
# -------------------------------------------------------------

# Require a fairly recent cmake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.5 )

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Uncomment this to enable detailed make output
# SET( CMAKE_VERBOSE_MAKEFILE ON )

# The project name
PROJECT( QNANChartWidget )

# Set some variables with info on the project
SET(PACKAGE_BUGREPORT "andreas.nicolai@gmx.net")
SET(PACKAGE_NAME "QNANChartWidget")

# Hide some more cache variables to keep things tidy
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

# Set release mode as default
IF (NOT CMAKE_BUILD_TYPE)
	SET( CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

# -------------
# Build options
# -------------

OPTION( BUILD_DESIGNER_PLUGINS  "Build Designer Plugins"  ON) 
OPTION( BUILD_EXAMPLES  "Build Examples"  ON) 

# ---------------
# Packages needed
# ---------------

FIND_PACKAGE( Qt4 REQUIRED )

# ---------------
# QT Setup
# ---------------

# if QT was found, set some macros
IF (QT_FOUND)
	MARK_AS_ADVANCED(QT_MKSPECS_DIR)
	MARK_AS_ADVANCED(QT_PLUGINS_DIR)
	MARK_AS_ADVANCED(QT_QMAKE_EXECUTABLE)
	MARK_AS_ADVANCED(QT_X11_X11_LIBRARY)
	MARK_AS_ADVANCED(QT_X11_Xext_LIBRARY)
	MARK_AS_ADVANCED(QT_X11_m_LIBRARY)
ENDIF (QT_FOUND) 

# set Qt modules that we need
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTXML TRUE )
INCLUDE(${QT_USE_FILE})

# ----------------------
# Widget library project
# ----------------------

# compile definitions
# ADD_DEFINITIONS( -Wall )

# include directories
SET(QNANChartWidget_INCLUDEPATH
	.
	src
)

# forms
SET( QNANChartWidget_UIS
	src/qnancharteditdialog.ui 
	src/qnanprintchartdialog.ui
	src/qnanserieseditdialog.ui
)

# regular sources
SET( QNANChartWidget_SRCS
	src/qnanabstractchartseries.cpp  
	src/qnanchartaxis.cpp            
	src/qnancharteditdialog.cpp      
	src/qnanchartwidget.cpp          
	src/qnandefaultchartseries.cpp   
	src/qnanprintchartdialog.cpp     
	src/qnanserieseditdialog.cpp     
	src/qnanseriestabledelegate.cpp  
)

# files we need to run moc over (all files with Q_OBJECT in it)
SET( QNANChartWidget_MOC_HDRS
	src/qnanabstractchartseries.h
	src/qnanchartaxis.h
	src/qnancharteditdialog.h
	src/qnanchartwidget.h
	src/qnandefaultchartseries.h
	src/qnanprintchartdialog.h
	src/qnanserieseditdialog.h
	src/qnanseriestabledelegate.h
)

# uic on .ui files
QT4_WRAP_UI( QNANChartWidget_UI_HDRS ${QNANChartWidget_UIS} )

# run moc on header files with Q_OBJECT
QT4_WRAP_CPP( QNANChartWidget_MOC_SRCS ${QNANChartWidget_MOC_HDRS} )

# add our include directories (necessary for ui, moc and so that we find the generated files)
INCLUDE_DIRECTORIES(
	${QNANChartWidget_INCLUDEPATH}
	${QT_ADDITIONAL_INCLUDE_PATHS}
	${CMAKE_BINARY_DIR}
	${QNANChartWidget_BINARY_DIR}
)

# add the QNANChartWidget widget library (static library for now)
ADD_LIBRARY( QNANChartWidget 
	${QNANChartWidget_SRCS} 
	${QNANChartWidget_RCS} 
	${QNANChartWidget_MOC_SRCS} 
	${QNANChartWidget_UI_HDRS} 
)

# link to dependent libraries (for shared lib only)
#TARGET_LINK_LIBRARIES( QNANChartWidget ${QNANChartWidget_LINK_LIBS} ${QT_LIBRARIES})

# --------
# Examples
# --------

IF (BUILD_EXAMPLES)
	ADD_SUBDIRECTORY( examples/simple )
	ADD_SUBDIRECTORY( examples/dataplotter )
ENDIF (BUILD_EXAMPLES)


# -------------
# Install stuff
# -------------

# Install directories
IF (WIN32)
	IF (NOT CMAKE_INSTALL_PREFIX)
		SET( CMAKE_INSTALL_PREFIX "C:\Qt\qnanwidgets" CACHE STRING
			"Specify install prefix"
			FORCE)
	ENDIF (NOT CMAKE_INSTALL_PREFIX)
ELSE (WIN32)
	IF (NOT CMAKE_INSTALL_PREFIX)
		SET( CMAKE_INSTALL_PREFIX "\usr\local\lib\qnanwidgets" CACHE STRING
			"Specify install prefix"
			FORCE)
	ENDIF (NOT CMAKE_INSTALL_PREFIX)
ENDIF (WIN32)

