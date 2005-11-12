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
 * @file IndexServer.cpp
 *
 **/

#include "IndexServer.h"
#include "Config.h"

#include <SimCore/Util/Log.h>
#include <SimCore/Util/SimpleConfig.h>
#include <SimCore/Battlefield/GlobalBattlefield.h>
#include <SimData/Thread.h>
#include <SimData/Trace.h>
#include <SimNet/ClientServer.h>


IndexServer::IndexServer() {
	CSP_LOG(APP, INFO, "IndexServer()");
}

IndexServer::~IndexServer() {
}

void IndexServer::initPrimaryInterface() {
	assert(m_NetworkServer.isNull());
	simnet::NetworkNode default_node;
	const std::string server_interface = g_Config.getString("Network", "Bind", default_node.getIpString() + ":3160", true);
	const std::string external_ip = g_Config.getString("Network", "ExternalIp", "", false);
	const std::string::size_type colon = server_interface.find(':');
	std::string address;
	simnet::Port port = 0;
	if (colon != std::string::npos) {
		port = static_cast<simnet::Port>(atoi(server_interface.substr(colon + 1).c_str()));
		address = server_interface.substr(0, colon);
	}
	if (address.size() == 0 || port == 0) {
		std::cerr << "Invalid value for Network.Bind in .ini file: " << server_interface << "\n";
		std::cerr << "Should be of the form www.xxx.yyy.zzz:port\n";
		::exit(1);
	}
	simnet::NetworkNode local_node(address, port);
	CSP_LOG(APP, INFO, "binding to interface " << local_node);

	const int incoming_bw = g_Config.getInt("Network", "IncomingBandwidth", 12000, true);
	const int outgoing_bw = g_Config.getInt("Network", "OutgoingBandwidth", 12000, true);
	m_NetworkServer = new simnet::Server(local_node, incoming_bw, outgoing_bw);

	if (external_ip.empty()) {
		if (!local_node.isRoutable()) {
			CSP_LOG(APP, WARNING, "no external ip address specified; accepting only local (LAN) connections");
		}
	} else {
		if (!local_node.isRoutable()) {
			simnet::NetworkNode external_node(external_ip, port);
			CSP_LOG(APP, INFO, "external interface is " << external_node);
			if (external_node.isRoutable()) {
				m_NetworkServer->setExternalNode(external_node);
			} else {
				CSP_LOG(APP, ERROR, "external interface is not routable; ignoring ExternalIp and accepting only local (LAN) connections");
			}
		} else {
			if (external_ip != address) {
				CSP_LOG(APP, ERROR, "binding to a routable interface that does not match the specified external ip (" << external_ip << "); ignoring ExternalIp");
			}
		}
	}
}

void IndexServer::initialize() {
	initPrimaryInterface();
	m_Battlefield = new GlobalBattlefield();
	m_Battlefield->setNetworkServer(m_NetworkServer);
}

void IndexServer::run() {
	initialize();
	CSP_LOG(APP, INFO, "starting network loop");
	simdata::Timer timer;
	timer.start();
	for (;;) {
		m_NetworkServer->processAndWait(0.01, 0.01, 0.10);
		m_Battlefield->update(timer.incremental());
	}
}


int main() {
	simdata::AutoTrace::install();

	if (!openConfig("IndexServer.ini")) {
		std::cerr << "Unable to open config file 'IndexServer.ini'\n";
		return 1;
	}

	csplog().logToFile("IndexServer.log");
	csplog().setCategories(simdata::LOG_ALL);
	csplog().setPriority(g_Config.getInt("Debug", "CspLoggingThreshold", simdata::LOG_INFO, true));
	simnet::netlog().setPriority(g_Config.getInt("Debug", "NetLoggingThreshold", simdata::LOG_INFO, true));

	IndexServer server;
	server.run();
}

