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

#ifndef __SIMNET_NETWORKNODE_H__
#define __SIMNET_NETWORKNODE_H__


#include <SimNet/NetBase.h>
#include <SimData/Uniform.h>
#ifdef _WIN32
#define NOMINMAX
#endif
#include <cc++/socket.h>

namespace simnet {


/** Class representing a remote host address (ip address and receive port)
 */
class NetworkNode
{
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
	NetworkNode(simdata::uint32 addr, ost::tpport_t port);

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
	NetworkNode(const char * hostname, ost::tpport_t port);

	/** Set the host ip address.
	 */
	void setAddress(ost::InetHostAddress addr);

	/** Set the host receive port.
	 */
	void setPort(ost::tpport_t port);

	/** Get the host receive port.
	 */
	ost::tpport_t getPort() const;

	/** Get the host ip address.
	 */
	ost::InetHostAddress const &getAddress() const;

	/** Convert ip address and port to a ConnectionPoint.
	 */
	inline ConnectionPoint getConnectionPoint() const {
		return ConnectionPoint(m_addr.getAddress().s_addr, m_port);
	}

	/** Get the host name.
	 */
	const char * getHostname() const;
};


} // namespace simnet

#endif // __SIMNET_NETWORKNODE_H__
