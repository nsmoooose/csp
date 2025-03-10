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

DatagramReceiveSocket::DatagramReceiveSocket(boost::asio::io_context &context, const boost::asio::ip::address &host, unsigned short port) :
	m_socket(context), m_endpoint(host, port) {
	m_socket.open(boost::asio::ip::udp::v4());
	m_socket.bind(m_endpoint);
}

void DatagramReceiveSocket::connect(const boost::asio::ip::address &host, unsigned short port) {
	m_endpoint = boost::asio::ip::udp::endpoint(host, port);
	m_socket.open(boost::asio::ip::udp::v4());
	m_socket.bind(m_endpoint);
}

void DatagramReceiveSocket::disconnect(void) {
	m_socket.close();
}

boost::asio::ip::address DatagramReceiveSocket::getSender(unsigned short *port) const {
	if(port) {
		*port = m_sender_endpoint.port();
	}
	return m_sender_endpoint.address();
}

int DatagramReceiveSocket::receive(void *buffer, std::size_t len) {
	return m_socket.receive_from(boost::asio::buffer(buffer, len), m_sender_endpoint);
}

int DatagramReceiveSocket::receive(void* buffer, std::size_t len, int timeoutMilliseconds) {
	auto timeout = std::chrono::milliseconds(timeoutMilliseconds);
	auto &context = getIOContext();
	boost::asio::steady_timer timer(context);
	boost::system::error_code ec;
	std::size_t received_bytes = 0;
	bool receive_completed = false;

	// Start the asynchronous receive
	m_socket.async_receive_from(
		boost::asio::buffer(buffer, len),
		m_sender_endpoint,
		[&](const boost::system::error_code& error, std::size_t bytes_transferred) {
			ec = error;
			received_bytes = bytes_transferred;
			receive_completed = true;
			// Cancel the timeout timer
			timer.cancel();
		});

	// Set up a timer for the timeout duration
	timer.expires_after(timeout);
	timer.async_wait([&](const boost::system::error_code& error) {
		if (!error && !receive_completed) {
			// Cancel the socket operation if it’s still pending
			m_socket.cancel();
		}
	});

	// Run the I/O context to process the operations
	context.run();
	// Reset the context for future calls
	context.restart();

	if (ec == boost::asio::error::operation_aborted) {
		// Indicate timeout
		return -1;
	} else if (ec) {
		// Throw an error if any occurred
		throw boost::system::system_error(ec);
	}

	return static_cast<int>(received_bytes);
}

int DatagramReceiveSocket::peek(void *buf, size_t len) {
	return m_socket.receive_from(
		boost::asio::buffer(buf, len),
		m_sender_endpoint, boost::asio::socket_base::message_peek);
}

bool DatagramReceiveSocket::isPendingReceive(timeout_t timeout) {
	return waitFor(m_socket, boost::asio::socket_base::wait_read, timeout);
}

DatagramTransmitSocket::DatagramTransmitSocket(boost::asio::io_context &context) : m_socket(context) {
}

void DatagramTransmitSocket::connect(const boost::asio::ip::address &host, unsigned short port) {
	m_endpoint = boost::asio::ip::udp::endpoint(host, port);
	m_socket.open(boost::asio::ip::udp::v4());
}

void DatagramTransmitSocket::disconnect(void) {
	m_socket.close();
}

int DatagramTransmitSocket::transmit(const char* data, std::size_t size) {
	return m_socket.send_to(boost::asio::buffer(data, size), m_endpoint);
}

bool DatagramTransmitSocket::isOutputReady(timeout_t timeout) {
	return waitFor(m_socket, boost::asio::socket_base::wait_write, timeout);
}

} // namespace csp
