SET(LIB_NAME cspsim)

MESSAGE("csp_SOURCE_DIR: ${csp_SOURCE_DIR}")

# FindPythonLibs sets the ${PYTHON_INCLUDE_DIRS} variable
INCLUDE(FindPythonLibs)
INCLUDE_DIRECTORIES(${csp_SOURCE_DIR}
					${PYTHON_INCLUDE_DIRS}
				   )

ADD_LIBRARY(${LIB_NAME} SHARED  
	"CSPSim.cpp"
)