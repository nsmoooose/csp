#include <csp/csplib/util/LogConstants.h>

namespace csp {

static const char* LogCategoryNames[64] = {};

__attribute__((constructor)) static void RegisterCategoryNames() {
	LogCategoryNames[__builtin_ctz(Cat_DATA)] = "DATA";
	LogCategoryNames[__builtin_ctz(Cat_ARCHIVE)] = "ARCHIVE";
	LogCategoryNames[__builtin_ctz(Cat_REGISTRY)] = "REGISTRY";
	LogCategoryNames[__builtin_ctz(Cat_THREAD)] = "THREAD";
	LogCategoryNames[__builtin_ctz(Cat_TIME)] = "TIME";
	LogCategoryNames[__builtin_ctz(Cat_NETWORK)] = "NETWORK";
	LogCategoryNames[__builtin_ctz(Cat_AUDIO)] = "AUDIO";
	LogCategoryNames[__builtin_ctz(Cat_OPENGL)] = "OPENGL";
	LogCategoryNames[__builtin_ctz(Cat_INPUT)] = "INPUT";
	LogCategoryNames[__builtin_ctz(Cat_NUMERIC)] = "NUMERIC";
	LogCategoryNames[__builtin_ctz(Cat_TESTING)] = "TESTING";
	LogCategoryNames[__builtin_ctz(Cat_GENERIC)] = "GENERIC";

	// net
	LogCategoryNames[__builtin_ctz(Cat_TIMING)] = "TIMING";
	LogCategoryNames[__builtin_ctz(Cat_HANDSHAKE)] = "HANDSHAKE";
	LogCategoryNames[__builtin_ctz(Cat_BALANCING)] = "BALANCING";
	LogCategoryNames[__builtin_ctz(Cat_PACKET)] = "PACKET";
	LogCategoryNames[__builtin_ctz(Cat_PEER)] = "PEER";
	LogCategoryNames[__builtin_ctz(Cat_MESSAGE)] = "MESSAGE";

	// cspsim
	LogCategoryNames[__builtin_ctz(Cat_TERRAIN)] = "TERRAIN";
	LogCategoryNames[__builtin_ctz(Cat_SCENE)] = "SCENE";
	LogCategoryNames[__builtin_ctz(Cat_BATTLEFIELD)] = "BATTLEFIELD";
	LogCategoryNames[__builtin_ctz(Cat_OBJECT)] = "OBJECT";
	LogCategoryNames[__builtin_ctz(Cat_APP)] = "APP";
	LogCategoryNames[__builtin_ctz(Cat_VIEW)] = "VIEW";
	LogCategoryNames[__builtin_ctz(Cat_THEATER)] = "THEATER";
}

const char *getLogCategoryName(int category) {
	return LogCategoryNames[__builtin_ctz(category)];
}

} // namespace csp
