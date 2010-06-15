SET(EXE_NAME csplib_config)

SET (SOURCE_AND_HEADER_PATH ${csp_SOURCE_DIR}/csp/csplib/util)

ADD_EXECUTABLE(${EXE_NAME} 
	${SOURCE_AND_HEADER_PATH}/Config.cpp)

# the only purpose of csplib_config is to create a platform-specific header file
# this needs to be done only once, so we add a custom build step that calls csplib_config
# and copies its output to the appropriate csplib source/header directory
ADD_CUSTOM_COMMAND(TARGET csplib_config
				   POST_BUILD
				   COMMAND ${EXE_NAME} > Config.h
				   WORKING_DIRECTORY ${csp_SOURCE_DIR}/csp/csplib/util
				   COMMENT run csplib_config to create Config.h for csplib in the appropriate directory)