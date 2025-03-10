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
 * @file NetworkNode.cpp:
 * @author Scott Flicker (Wolverine)
 *
 * This class encapsulates a node in the simulation network.
 */

#include <csp/csplib/net/NetworkNode.h>
#include <csp/csplib/net/Sockets.h>
#include <sstream>

namespace csp {

NetworkNode::NetworkNode() {
	m_port = 0;
}

NetworkNode::NetworkNode(boost::asio::ip::address addr, unsigned short port) {
	m_addr = addr;
	m_port = port;
}

NetworkNode::NetworkNode(std::string const &hostname, unsigned short port) {
	m_addr = resolveByHostname(hostname, port);
	m_port = port;
}

NetworkNode::NetworkNode(ConnectionPoint const &point) {
	m_addr = point.first;
	m_port = point.second;
}

void NetworkNode::setAddress(boost::asio::ip::address addr) {
	m_addr = addr;
}

void NetworkNode::setPort(unsigned short port) {
	m_port = port;
}

boost::asio::ip::address const &NetworkNode::getAddress() const {
	return m_addr;
}

std::string NetworkNode::getHostname() const {
	return getHostnameByAddress(m_addr);
}

} // namespace csp
