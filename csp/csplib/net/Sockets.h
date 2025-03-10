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

CSPLIB_EXPORT boost::asio::ip::address resolveByHostname(const std::string &hostname, unsigned short port);

CSPLIB_EXPORT std::string getHostnameByAddress(const boost::asio::ip::address &address);

/** Minimal wrapper for boost::asio socket.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT DatagramReceiveSocket {
public:
	DatagramReceiveSocket(const boost::asio::ip::address &host, unsigned short port);

	void connect(const boost::asio::ip::address &host, unsigned short port);

	void disconnect(void);

	bool isPendingReceive(timeout_t timeout);

	boost::asio::ip::address getSender(unsigned short *port = NULL) const;

	int receive(void *buffer, std::size_t len);

	int receive(void* buffer, std::size_t len, int timeoutMilliseconds);

	int peek(void *buf, size_t len);

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
	DatagramTransmitSocket();

	void connect(const boost::asio::ip::address &host, unsigned short port);

	void disconnect(void);

	int transmit(const char* data, std::size_t size);

	bool isOutputReady(timeout_t timeout=0);

private:
	boost::asio::ip::udp::socket m_socket;
	boost::asio::ip::udp::endpoint m_endpoint;
};

} // namespace csp
