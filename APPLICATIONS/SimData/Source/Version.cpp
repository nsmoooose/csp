#include <SimData/Version.h>

#ifndef SIMDATA_VERSION
	#define SIMDATA_VERSION "internal"
#endif

NAMESPACE_SIMDATA

const char *getVersion() {
	return SIMDATA_VERSION;
}

NAMESPACE_END

