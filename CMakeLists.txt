cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

PROJECT(csp)

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})
MESSAGE("exe out: ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE("out bindir: ${OUTPUT_BINDIR}")

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})
MESSAGE("lib output path: ${LIBRARY_OUTPUT_PATH}")

IF (WIN32)
	SET(CMAKE_DEBUG_POSTFIX  "d")
	ADD_DEFINITIONS( -D__WIN32__ -D_CRT_SECURE_NO_DEPRECATE -D_USE_MATH_DEFINES)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /wd4251 /wd4275 /wd4290")
ENDIF (WIN32)

#${CMAKE_SOURCE_DIR}/CMakeModules contains custom CMake modules to find our project specific dependencies
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# this calls the script FindCSPDEVPACK.cmake in CMakeModules
# and sets DEVPACK_INCLUDE_DIR and DEVPACK_LIB_DIR
FIND_PACKAGE(CSPDEVPACK)

# FindPythonLibs sets the PYTHON_INCLUDE_DIRS and PYTHON_LIBRARIES variables
INCLUDE(FindPythonLibs)
# FindPythonInterp sets PYTHON_EXECUTABLE and PYTHONINTERP_FOUND
INCLUDE(FindPythonInterp)

INCLUDE_DIRECTORIES(BEFORE
					${csp_SOURCE_DIR}
					${PYTHON_INCLUDE_DIRS}
					${DEVPACK_INCLUDE_DIR}
				   )

LINK_DIRECTORIES(BEFORE
				 ${DEVPACK_LIB_DIR}
				 ${PYTHON_LIBRARIES}
				)

ADD_SUBDIRECTORY(csp/csplib/util)				
ADD_SUBDIRECTORY(csp/csplib)
#ADD_SUBDIRECTORY(csp/cspwf)
ADD_SUBDIRECTORY(csp/cspsim)
#ADD_SUBDIRECTORY(csp/examples)



# IF(WIN32)

	#######################################################################################################
	# the foolowing options are MSVC specific,
	# the first dtPhysics_MSVC_VERSIONED_DLL activate a custom build-time layout that should allow to run examples and application 
	# fron bin folder without requiring installation step.
	# it also prepend "osg${OPENSCENEGRAPH_SOVERSION}-" to only .dll files, leaving .lib files untouched in lib
	# it also use a hack to get rid of Debug and Release folder in MSVC projects 
	# all the .dll and .pdb are in bin and all the .lib and .exp are in lib
	
	# the second option disable incremental linking in debug build , that is enabled by default by CMake
	#########################################################################################################

	# IF(MSVC)
			# OPTION(csplib_MSVC_VERSIONED_DLL "Set to ON to build OpenSceneGraph with versioned dll names" ON)
			# MARK_AS_ADVANCED(csplib_MSVC_VERSIONED_DLL)
			# OPTION(csplib_MSVC_DEBUG_INCREMENTAL_LINK "Set to OFF to build OpenSceneGraph without incremental linking in debug (release is off by default)" ON)
			# MARK_AS_ADVANCED(csplib_MSVC_DEBUG_INCREMENTAL_LINK)
			# IF(NOT csplib_MSVC_DEBUG_INCREMENTAL_LINK)
				# SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
				# SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
				# SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
			# ENDIF(NOT csplib_MSVC_DEBUG_INCREMENTAL_LINK)
	# ENDIF(MSVC)
# ENDIF(WIN32)

# PROJECT(csplib)

# SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR})
# MAKE_DIRECTORY(${OUTPUT_BINDIR})
# SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})


# INSTALL(TARGETS cspEarth
       # RUNTIME DESTINATION "c:/temp/bin")
