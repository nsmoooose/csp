// Combat Simulator Project - FlightSim Demo
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


#include <SimData/Uniform.h>
#include <cc++/socket.h>

namespace simnet {

class NetworkNode
{
private:
	ost::InetHostAddress m_addr;
	ost::tpport_t m_port;

public:
	NetworkNode();
	NetworkNode(ost::InetHostAddress addr, ost::tpport_t port);
	NetworkNode(simdata::uint32 addr, ost::tpport_t port);
	NetworkNode(const char * hostname, ost::tpport_t port);

	void setAddress(ost::InetHostAddress addr);
	void setPort(ost::tpport_t port);

	ost::tpport_t getPort() const;
	ost::InetHostAddress const &getAddress() const;
	const char * getHostname() const;
};

} // namespace simnet

#endif // __SIMNET_NETWORKNODE_H__
