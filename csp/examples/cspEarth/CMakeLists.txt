CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(cspEarth)

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/../.bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})

IF (WIN32)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /GR /Zi /EHsc /W2 /nologo /wd4251 /wd4275 /wd4290")
SET(CMAKE_DEBUG_POSTFIX  "d")
ADD_DEFINITIONS(-DWIN32 -D__WIN32__ -D_USRDLL -D_DLL -D_CRT_SECURE_NO_DEPRECATE -D_USE_MATH_DEFINES)
ENDIF (WIN32)

INCLUDE_DIRECTORIES("D:/Dev/CSP/cspdevpack-0.7/include"
					"D:/Dev/CSP"
                   )
				   
LINK_DIRECTORIES("D:/Dev/CSP/cspdevpack-0.7/lib"
				 "D:/Dev/CSP/csp/cspsim/.bin"
				 "D:/Dev/CSP/csp/cspsim/"
				 "D:/Dev/CSP/csp/csplib/.bin"
				 "D:/Dev/CSP/csp/csplib"
				 "D:/Dev/CSP/csp/cspwf/.bin"
				 "D:/Dev/CSP/csp/cspwf"
				 "C:/Python24/Lib/site-packages"
                )

SET(SOURCE_FILES program.cpp 
				 inputHandler.cpp
   )
				 
SET(HEADER_FILES inputHandler.h)
				   
ADD_EXECUTABLE(cspEarth ${SOURCE_FILES}
                        ${HEADER_FILES}
			  )

TARGET_LINK_LIBRARIES(cspEarth osg.lib
							   OpenThreads.lib
							   osgDB.lib
							   osgUtil.lib
							   osgGA.lib
							   osgViewer.lib
							   osgal.lib 
							   osg.lib 
							   osgDB.lib 
							   osgFX.lib 
							   osgGA.lib 
							   osgParticle.lib 
							   osgViewer.lib 
							   osgText.lib 
							   osgUtil.lib 
							   OpenThreads.lib 
							   ccgnu2.lib 
							   sigc-2.0.lib 
							   advapi32.lib 
							   ws2_32.lib 
							   glu32.lib
							   opengl32.lib 
							   SDL.lib 
							   oalpp.lib 
							   vorbis.lib 
							   vorbisfile.lib 
							   ogg.lib 
							   osgEarth.lib 
							   osgEarthSymbology.lib 
							   osgEarthUtil.lib 
							   osgEarthFeatures.lib
							   csplib.lib
							   cspsim.lib
							   cspwf.lib
                     )

#INSTALL(TARGETS cspEarth
#        RUNTIME DESTINATION "c:/temp/bin")
 
