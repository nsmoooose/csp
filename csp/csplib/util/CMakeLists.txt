SET(EXE_NAME csplib_config)

MESSAGE("csplib_config")
SET (SOURCE_AND_HEADER_PATH ${csp_SOURCE_DIR}/csp/csplib/util)

ADD_EXECUTABLE(${EXE_NAME} 
	${SOURCE_AND_HEADER_PATH}/Config.cpp
)