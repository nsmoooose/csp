#include <csp/csplib/util/LogConstants.h>

namespace csp {

static const char* LogCategoryNames[64] = {};

__attribute__((constructor)) static void RegisterCategoryNames() {
	LogCategoryNames[__builtin_ctz(cLogCategory_DATA)] = "DATA";
	LogCategoryNames[__builtin_ctz(cLogCategory_ARCHIVE)] = "ARCHIVE";
	LogCategoryNames[__builtin_ctz(cLogCategory_REGISTRY)] = "REGISTRY";
	LogCategoryNames[__builtin_ctz(cLogCategory_THREAD)] = "THREAD";
	LogCategoryNames[__builtin_ctz(cLogCategory_TIME)] = "TIME";
	LogCategoryNames[__builtin_ctz(cLogCategory_NETWORK)] = "NETWORK";
	LogCategoryNames[__builtin_ctz(cLogCategory_AUDIO)] = "AUDIO";
	LogCategoryNames[__builtin_ctz(cLogCategory_OPENGL)] = "OPENGL";
	LogCategoryNames[__builtin_ctz(cLogCategory_INPUT)] = "INPUT";
	LogCategoryNames[__builtin_ctz(cLogCategory_NUMERIC)] = "NUMERIC";
	LogCategoryNames[__builtin_ctz(cLogCategory_TESTING)] = "TESTING";
	LogCategoryNames[__builtin_ctz(cLogCategory_GENERIC)] = "GENERIC";

	// net
	LogCategoryNames[__builtin_ctz(cLogCategory_TIMING)] = "TIMING";
	LogCategoryNames[__builtin_ctz(cLogCategory_HANDSHAKE)] = "HANDSHAKE";
	LogCategoryNames[__builtin_ctz(cLogCategory_BALANCING)] = "BALANCING";
	LogCategoryNames[__builtin_ctz(cLogCategory_PACKET)] = "PACKET";
	LogCategoryNames[__builtin_ctz(cLogCategory_PEER)] = "PEER";
	LogCategoryNames[__builtin_ctz(cLogCategory_MESSAGE)] = "MESSAGE";

	// cspsim
	LogCategoryNames[__builtin_ctz(cLogCategory_TERRAIN)] = "TERRAIN";
	LogCategoryNames[__builtin_ctz(cLogCategory_SCENE)] = "SCENE";
	LogCategoryNames[__builtin_ctz(cLogCategory_BATTLEFIELD)] = "BATTLEFIELD";
	LogCategoryNames[__builtin_ctz(cLogCategory_OBJECT)] = "OBJECT";
	LogCategoryNames[__builtin_ctz(cLogCategory_APP)] = "APP";
	LogCategoryNames[__builtin_ctz(cLogCategory_VIEW)] = "VIEW";
	LogCategoryNames[__builtin_ctz(cLogCategory_THEATER)] = "THEATER";
}

const char *getLogCategoryName(int category) {
	return LogCategoryNames[__builtin_ctz(category)];
}

} // namespace csp
