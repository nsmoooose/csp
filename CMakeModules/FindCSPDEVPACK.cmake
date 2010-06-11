FIND_PATH( DEVPACK_INCLUDE_DIR jpeglib.h
	$ENV{CSP_DEVPACK}/include
    ${CMAKE_SOURCE_DIR}/cspdevpack-0.7/include
)

FIND_PATH( DEVPACK_LIB_DIR jpeg.lib
	$ENV{CSP_DEVPACK}/lib
    ${CMAKE_SOURCE_DIR}/cspdevpack-0.7/lib
)