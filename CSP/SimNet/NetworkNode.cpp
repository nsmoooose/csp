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
 * @file NetworkNode.cpp:
 * @author Scott Flicker (Wolverine)
 *
 * This class encapsulates a node in the simulation network.
 */

#include <SimNet/NetworkNode.h>

namespace simnet {

NetworkNode::NetworkNode() {
	m_port = 0;
}

NetworkNode::NetworkNode(ost::InetHostAddress addr, ost::tpport_t port) {
	m_addr = addr;
	m_port = port;
}

NetworkNode::NetworkNode(const char * hostname, ost::tpport_t port) {
	m_addr = ost::InetHostAddress(hostname);
	m_port = port;
}

NetworkNode::NetworkNode(simdata::uint32 addr, ost::tpport_t port) {
	struct in_addr my_in_addr;
	my_in_addr.s_addr = addr;
	m_addr = ost::InetHostAddress(my_in_addr);
	m_port = port;
}

NetworkNode::NetworkNode(ConnectionPoint const &point) {
	struct in_addr my_in_addr;
	my_in_addr.s_addr = point.first;
	m_addr = ost::InetHostAddress(my_in_addr);
	m_port = point.second;
}

void NetworkNode::setAddress(ost::InetHostAddress addr) {
	m_addr = addr;
}

void NetworkNode::setPort(ost::tpport_t port) {
	m_port = port;
}

ost::InetHostAddress const &NetworkNode::getAddress() const {
	return m_addr;
}

ost::tpport_t NetworkNode::getPort() const {
	return m_port;
}

const char * NetworkNode::getHostname() const {
	return m_addr.getHostname();
}

} // namespace simnet

