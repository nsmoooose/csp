#pragma once
// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Sockets.h
 *
 */

#include <boost/asio.hpp>
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Export.h>

namespace csp {

/** Networking error.
 */
CSP_EXCEPTION(NetworkException)

typedef int timeout_t;

CSPLIB_EXPORT boost::asio::io_context &getIOContext();

/** Minimal wrapper for boost::asio socket.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT DatagramReceiveSocket {
public:
	DatagramReceiveSocket(boost::asio::io_context &context, const boost::asio::ip::address &host, unsigned short port) :
		m_socket(context), m_endpoint(host, port) {
		m_socket.open(boost::asio::ip::udp::v4());
		m_socket.bind(m_endpoint);
	}

	void connect(const boost::asio::ip::address &host, unsigned short port) {
		m_endpoint = boost::asio::ip::udp::endpoint(host, port);
		m_socket.open(boost::asio::ip::udp::v4());
		m_socket.bind(m_endpoint);
	}

	void disconnect(void) {
		m_socket.close();
	}

	bool isPendingReceive(timeout_t timeout);

	boost::asio::ip::address getSender(unsigned short *port = NULL) const {
		if(port) {
			*port = m_sender_endpoint.port();
		}
		return m_sender_endpoint.address();
	}

	inline int receive(void *buffer, std::size_t len) {
		return m_socket.receive_from(boost::asio::buffer(buffer, len), m_sender_endpoint);
	}

	template <typename Rep, typename Period>
	int receive(void* buffer, std::size_t len, const std::chrono::duration<Rep, Period>& timeout) {
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

	inline int peek(void *buf, size_t len) {
		return m_socket.receive_from(
			boost::asio::buffer(buf, len),
			m_sender_endpoint, boost::asio::socket_base::message_peek);
	}
private:
	boost::asio::ip::udp::socket m_socket;
	boost::asio::ip::udp::endpoint m_endpoint;
	boost::asio::ip::udp::endpoint m_sender_endpoint;
};


/** Minimal wrapper for UDP transmit socket.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT DatagramTransmitSocket {
public:
	DatagramTransmitSocket(boost::asio::io_context &context) : m_socket(context) {
	}

	inline void connect(const boost::asio::ip::address &host, unsigned short port) {
		m_endpoint = boost::asio::ip::udp::endpoint(host, port);
		m_socket.open(boost::asio::ip::udp::v4());
	}

	inline void disconnect(void) {
		m_socket.close();
	}

	inline int transmit(const char* data, std::size_t size) {
		return m_socket.send_to(boost::asio::buffer(data, size), m_endpoint);
	}

	bool isOutputReady(timeout_t timeout=0);

private:
	boost::asio::ip::udp::socket m_socket;
	boost::asio::ip::udp::endpoint m_endpoint;
};

} // namespace csp
