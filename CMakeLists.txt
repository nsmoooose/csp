# root makefile for CSP
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

PROJECT(csp)

#SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
#SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/bin)

MESSAGE("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
MESSAGE("csp_SOURCE_DIR ${csp_SOURCE_DIR}")
MESSAGE("OUTPUT_BINDIR: ${OUTPUT_BINDIR}")
MESSAGE("OUTPUT_LIBDIR: ${OUTPUT_LIBDIR}")
MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
MESSAGE("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE("EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH}")

SET(CMAKE_DEBUG_POSTFIX  "d" CACHE STRING "add a postfix, usually d on windows")

IF (WIN32)
	IF (MSVC)
		ADD_DEFINITIONS( -D__WIN32__ -D_CRT_SECURE_NO_DEPRECATE -D_USE_MATH_DEFINES -D_SCL_SECURE_NO_WARNINGS)
		# make sure the /Od flag is set to turn off optimizations. Otherwise the release build will fail!
		# TODO: Find out why optimization doesn't work (Trac ticket [csp] #23)
		SET(CMAKE_CXX_FLAGS_RELEASE "/MD /Od /Ob2 /D NDEBUG")
		SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /Od /Ob1 /D NDEBUG")
		# The following compiler warnings has been disabled:
		# warning C4251: 'X' : struct 'Y' needs to have dll-interface to be used by clients of class 'Z' 
		# warning C4275: non dll-interface class 'X' used as base for dll-interface class 'Y'
		# warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
		# warning C4305: The identifier is converted to a smaller type, resulting in loss of information (double to float conversion)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /wd4251 /wd4275 /wd4290 /wd4305")
		OPTION(csp_MSVC_VERSIONED_DLL "Set to ON to build with versioned dll names" ON)
		
		OPTION(WIN32_USE_MP "Set to ON to build csp with the /MP option (Visual Studio 2005 and above)." OFF)
		MARK_AS_ADVANCED(WIN32_USE_MP)
        IF(WIN32_USE_MP)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        ENDIF(WIN32_USE_MP)
		
		# This option is to enable the /DYNAMICBASE switch
        # It is used to workaround a bug in Windows 7 when linking in release, which results in corrupt
        # binaries. See this page for details: http://www.wintellect.com/CS/blogs/jrobbins/archive/2009/01/24/the-case-of-the-corrupt-pe-binaries.aspx
        OPTION(WIN32_USE_DYNAMICBASE "Set to ON to build OpenSceneGraph with the /DYNAMICBASE option to work around a bug when linking release executables on Windows 7." ON)
        MARK_AS_ADVANCED(WIN32_USE_DYNAMICBASE)
        IF(WIN32_USE_DYNAMICBASE)
            SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DYNAMICBASE")
        ENDIF(WIN32_USE_DYNAMICBASE)
		
	ENDIF (MSVC)
ENDIF (WIN32)

# Create common bin directory
FILE(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#${CMAKE_SOURCE_DIR}/CMakeModules contains custom CMake modules to find our project specific dependencies
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# this calls the script FindCSPDEVPACK.cmake in CMakeModules
# and sets DEVPACK_INCLUDE_DIR and DEVPACK_LIB_DIR
FIND_PACKAGE(CSPDEVPACK REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

# FindPythonLibs sets the PYTHON_INCLUDE_DIRS and PYTHON_LIBRARIES variables
INCLUDE(FindPythonLibs)
# FindPythonInterp sets PYTHON_EXECUTABLE and PYTHONINTERP_FOUND
INCLUDE(FindPythonInterp)

INCLUDE_DIRECTORIES(${csp_SOURCE_DIR}
					${DEVPACK_INCLUDE_DIR}
					${DEVPACK_INCLUDE_DIR}/AL
					${PYTHON_INCLUDE_DIRS}
				   )

LINK_DIRECTORIES(${OUTPUT_LIBDIR}
				 ${DEVPACK_LIB_DIR}
				)

SET(SOUND_LIBS
	oalpp
	ogg
	vorbis
	vorbisfile
   )

SET(OSG_LIBS
	osg
	osgAL
	osgDB
	osgFX
	osgGA
	osgParticle
	osgViewer
	osgText
	osgUtil
	OpenThreads
   )
   
SET(OSGEARTH_LIBS
    osgEarth.lib 
	osgEarthSymbology.lib 
	osgEarthUtil.lib 
	osgEarthFeatures.lib
   )

SET(OPENGL_LIBS
    glu32
	opengl32
   )
				
ADD_SUBDIRECTORY(csp/csplib/util)				
ADD_SUBDIRECTORY(csp/csplib)
ADD_SUBDIRECTORY(csp/cspwf)
ADD_SUBDIRECTORY(csp/cspsim)
ADD_SUBDIRECTORY(csp/examples/cspEarth)

# INSTALL(TARGETS cspEarth
       # RUNTIME DESTINATION "c:/temp/bin")
