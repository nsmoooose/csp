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

#include <SimNet/Networking.h>
#include <SimNet/NetworkNode.h>

#include "Animation.h"
#include "ObjectModel.h"

NetworkNode::NetworkNode() {
	// m_node_id = 0;
	m_addr = "0.0.0.0";
	m_port = 0;
}

NetworkNode::NetworkNode(/* int node_id, */ ost::InetHostAddress addr, Port port) {
	// m_node_id = node_id;
	m_addr = addr;
	m_port = port;
}

NetworkNode::NetworkNode(/* int node_id, */ const char * hostname, Port port) {
	// m_node_id = node_id;
	m_addr = ost::InetHostAddress(hostname);
	m_port = port;
}

NetworkNode::NetworkNode(/* int node_id, */ simdata::uint32 addr, Port port) {
	// m_node_id = node_id;
	struct in_addr my_in_addr;
	my_in_addr.s_addr = addr;
	m_addr = ost::InetHostAddress(my_in_addr);
	m_port = port;
}

void NetworkNode::setAddress(ost::InetHostAddress addr) {
	m_addr = addr;
}

void NetworkNode::setPort(Port port) {
	m_port = port;
}


//void NetworkNode::setId(short node_id) {
//  m_node_id = node_id;
//}

ost::InetHostAddress NetworkNode::getAddress() {
	return m_addr;
}

Port NetworkNode::getPort() {
	return m_port;
}

//short NetworkNode::getId() {
//  return m_node_id;
//}

const char * NetworkNode::getHostname() {
	return m_addr.getHostname();
}
