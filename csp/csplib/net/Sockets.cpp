#include <csp/csplib/net/Sockets.h>

namespace csp {

boost::asio::io_context &getIOContext() {
	static boost::asio::io_context io_context;
	return io_context;
}

static bool waitFor(
	boost::asio::ip::udp::socket &socket,
	boost::asio::socket_base::wait_type w,
	timeout_t timeout) {
	auto &context = getIOContext();
	boost::asio::steady_timer timer(context);
	boost::system::error_code ec1;
	boost::system::error_code ec2;
	socket.async_wait(
		w,
		[&](const boost::system::error_code &error) {
			ec1 = error;
			timer.cancel();
		});

	timer.expires_after(std::chrono::milliseconds(timeout));
	timer.async_wait(
		[&](const boost::system::error_code& error) {
			ec2 = error;
			socket.cancel();
		});

	context.run();
	context.restart();

	if (ec1 == boost::asio::error::operation_aborted) {
		return false;
	} else if (ec1) {
		throw boost::system::system_error(ec1);
	}
	return true;
}

bool DatagramReceiveSocket::isPendingReceive(timeout_t timeout) {
	return waitFor(m_socket, boost::asio::socket_base::wait_read, timeout);
}

bool DatagramTransmitSocket::isOutputReady(timeout_t timeout) {
	return waitFor(m_socket, boost::asio::socket_base::wait_write, timeout);
}

} // namespace csp
