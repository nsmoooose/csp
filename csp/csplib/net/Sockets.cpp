#include <csp/csplib/net/Sockets.h>

namespace csp {

boost::asio::io_context &getIOContext() {
	static boost::asio::io_context io_context;
	return io_context;
}

} // namespace csp
