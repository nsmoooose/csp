CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(APP_NAME Examples_cspEarth)

SET(SOURCE_FILES program.cpp 
				 inputHandler.cpp
				 showNodeNamesVisitor.cpp
   )
				 
SET(HEADER_FILES inputHandler.h
				 ShowNodeNamesVisitor.h 
   )
				   
ADD_EXECUTABLE(${APP_NAME}
			   ${SOURCE_FILES}
               ${HEADER_FILES}
			  )

TARGET_LINK_LIBRARIES(${APP_NAME}
					  ${OSG_LIBS}
					  ${OSGEARTH_LIBS}
					  csplib
					  cspsim
                     )

ADD_DEPENDENCIES(${APP_NAME} csplib cspsim)

#INSTALL(TARGETS cspEarth
#        RUNTIME DESTINATION "c:/temp/bin")
 
