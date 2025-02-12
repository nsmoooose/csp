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
 * @file NetworkNode.h
 *
 */

#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Uniform.h>

#include <boost/asio.hpp>
#include <string>

namespace csp {

/** Class representing a remote host address (ip address and receive port)
 *  @ingroup net
 */
class CSPLIB_EXPORT NetworkNode {
private:
	boost::asio::ip::address m_addr;
	unsigned short m_port;

public:
	/** Construct a default node.  The address defaults to the interface
	 *  matching gethostname(), port 0.
	 */
	NetworkNode();

	/** Construct from an existing cc++ address and receive port.
	 */
	NetworkNode(boost::asio::ip::address addr, unsigned short port);

	/** Construct a new node from a ConnectionPoint.
	 *
	 *  @param point ConnectionPoint with the node's ip address and port.
	 */
	explicit NetworkNode(ConnectionPoint const &point);

	/** Construct a new node, from hostname and receive port.
	 *
	 *  @param hostname the physical host address or the DNS name of a
	 *    host machine (e.g. "csp.sourceforge.net").
	 *  @param port the port number used by the host for receiving data.
	 */
	NetworkNode(std::string const &hostname, unsigned short port);

	/** Set the host ip address.
	 */
	void setAddress(boost::asio::ip::address addr);

	/** Set the host receive port.
	 */
	void setPort(unsigned short port);

	/** Get the host receive port.
	 */
	inline unsigned short getPort() const { return m_port; }

	/** Get the host ip address.
	 */
	boost::asio::ip::address const &getAddress() const;

	/** Convert ip address and port to a ConnectionPoint.
	 */
	inline ConnectionPoint getConnectionPoint() const {
		return ConnectionPoint(getAddress(), getPort());
	}

	/** Get the host name.
	 */
	std::string getHostname() const;

	/** Get ip address as a dotted-quad string.
	 */
	inline std::string getIpString() const {
		return m_addr.to_string();
	}

	/** Return true if this ip is routable (ie, not 127.0.0.0/8, 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16).
	 */
	inline bool isRoutable() const {
		return isRoutable(m_addr);
	}

	/** Test if an ip address is routable (ie, not 127.0.0.0/8, 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16).
	 *  @param addr 32-bit ipv4 address in network byte-order.
	 */
	static bool isRoutable(const boost::asio::ip::address &address) {
		auto addr = address.to_v4().to_uint();
		return ((addr & 0xffff) != 43200) &&  // 192.168.  0.  0 - 192.168.255.255
		       ((addr & 0x00ff) !=    10) &&  //  10.  0.  0.  0 -  10.255.255.255
		       ((addr & 0x00ff) !=   127) &&  // 127.  0.  0.  0 - 127.255.255.255
		       ((addr & 0xf0ff) !=  4268);    // 172. 16.  0.  0 - 172. 31.255.255
	}
};


inline std::ostream &operator<<(std::ostream &os, NetworkNode const &node) {
	return os << node.getIpString() << ':' << node.getPort() << " (" << node.getHostname() << ")";
}

inline std::ostream &operator<<(std::ostream &os, ConnectionPoint const &point) {
	return os << NetworkNode(point);
}

} // namespace csp
