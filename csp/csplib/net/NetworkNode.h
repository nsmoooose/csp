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

#ifdef _WIN32
#	pragma warning(push)
#	pragma warning(disable: 4100 4996)
#endif

#include <cc++/socket.h>

#ifdef _WIN32
#	pragma warning(pop)
#endif

#include <string>

namespace csp {


/** Class representing a remote host address (ip address and receive port)
 *  @ingroup net
 */
class CSPLIB_EXPORT NetworkNode {
private:
	ost::InetHostAddress m_addr;
	ost::tpport_t m_port;

public:
	/** Construct a default node.  The address defaults to the interface
	 *  matching gethostname(), port 0.
	 */
	NetworkNode();

	/** Construct from an existing cc++ address and receive port.
	 */
	NetworkNode(ost::InetHostAddress addr, ost::tpport_t port);

	/** Construct a new node from ip address and receive port.
	 *
	 *  @param addr the binary address of the host.
	 *  @param port the port number used by the host for receiving data.
	 */
	NetworkNode(uint32_t addr, ost::tpport_t port);

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
	NetworkNode(std::string const &hostname, ost::tpport_t port);

	/** Same as the (string const&, ost::tpport_t) constructor, but unfortunately
	 *  necessary to disambiguate const char* hostnames.
	 */
	NetworkNode(const char *hostname, ost::tpport_t port);

	/** Set the host ip address.
	 */
	void setAddress(ost::InetHostAddress addr);

	/** Set the host receive port.
	 */
	void setPort(ost::tpport_t port);

	/** Get the host receive port.
	 */
	inline ost::tpport_t getPort() const { return m_port; }

	/** Get the host ip address.
	 */
	ost::InetHostAddress const &getAddress() const;

	/** Convert ip address and port to a ConnectionPoint.
	 */
	inline ConnectionPoint getConnectionPoint() const {
		return ConnectionPoint(getIp(), getPort());
	}

	/** Get the ip address as a 32-bit int, in network byte-order.
	 */
	inline uint32_t getIp() const {
		return m_addr.getAddress().s_addr;
	}

	/** Get the host name.
	 */
	const char * getHostname() const;

	/** Get ip address as a dotted-quad string.
	 */
	inline std::string getIpString() const {
		return ipToString(getIp());
	}

	/** Return true if this ip is routable (ie, not 127.0.0.0/8, 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16).
	 */
	inline bool isRoutable() const {
		return isRoutable(getIp());
	}

	/** Test if an ip address is routable (ie, not 127.0.0.0/8, 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16).
	 *  @param addr 32-bit ipv4 address in network byte-order.
	 */
	static bool isRoutable(uint32_t addr) {
		return ((addr & 0xffff) != 43200) &&  // 192.168.  0.  0 - 192.168.255.255
		       ((addr & 0x00ff) !=    10) &&  //  10.  0.  0.  0 -  10.255.255.255
		       ((addr & 0x00ff) !=   127) &&  // 127.  0.  0.  0 - 127.255.255.255
		       ((addr & 0xf0ff) !=  4268);    // 172. 16.  0.  0 - 172. 31.255.255
	}

	/** Convert an ip address to a dotted-quad string.
	 *  @param addr 32-bit ipv4 address in network byte-order.
	 */
	static std::string ipToString(uint32_t addr);
};


inline std::ostream &operator<<(std::ostream &os, NetworkNode const &node) {
	return os << node.getIpString() << ':' << node.getPort() << " (" << node.getHostname() << ")";
}

inline std::ostream &operator<<(std::ostream &os, ConnectionPoint const &point) {
	return os << NetworkNode(point);
}

} // namespace csp
