SET(LIB_NAME csplib)

MESSAGE("csp_SOURCE_DIR: ${csp_SOURCE_DIR}")
SET (SOURCE_AND_HEADER_PATH ${csp_SOURCE_DIR}/csp/csplib)

ADD_LIBRARY(${LIB_NAME} SHARED 
	${SOURCE_AND_HEADER_PATH}/data/Archive.h
	${SOURCE_AND_HEADER_PATH}/data/Archive.cpp
)

# make sure csplib_config is run initially, because it creates a 
# config file needed by csplib
ADD_DEPENDENCIES(csplib csplib_config)