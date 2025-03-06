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
 * @file IndexServer.cpp
 *
 **/

#include <csignal>
#include <iostream>

#include <csp/cspsim/battlefield/GlobalBattlefield.h>
#include <csp/cspsim/IndexServer.h>
#include <csp/cspsim/Config.h>
#include <csp/csplib/net/ClientServer.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/SimpleConfig.h>
#include <csp/csplib/util/Trace.h>

namespace csp {

bool sigint_quit = false;
void (*sigint_original)(int);

void sigint_handler(int parameter) {
	// Signal to the main loop that we wish to quit.
	sigint_quit = true;

	// Also call the original handler.
	sigint_original(parameter);
}

IndexServer::IndexServer() {
	CSPLOG(Prio_INFO, Cat_APP) << "IndexServer()";
}

IndexServer::~IndexServer() {
}

void IndexServer::initPrimaryInterface() {
	assert(m_NetworkServer.isNull());
	NetworkNode default_node;
	const std::string server_interface = g_Config.getString("Network", "Bind", default_node.getIpString() + ":3160", true);
	const std::string external_ip = g_Config.getString("Network", "ExternalIp", "", false);
	const std::string::size_type colon = server_interface.find(':');
	std::string ipaddress;
	Port port = 0;
	if (colon != std::string::npos) {
		port = static_cast<Port>(atoi(server_interface.substr(colon + 1).c_str()));
		ipaddress = server_interface.substr(0, colon);
	}
	if (ipaddress.size() == 0 || port == 0) {
		std::cerr << "Invalid value for Network.Bind in .ini file: " << server_interface << "\n";
		std::cerr << "Should be of the form www.xxx.yyy.zzz:port\n";
		::exit(1);
	}

	auto address = boost::asio::ip::make_address(ipaddress);

	NetworkNode local_node(address, port);
	CSPLOG(Prio_INFO, Cat_APP) << "binding to interface " << local_node;

	const int incoming_bw = g_Config.getInt("Network", "IncomingBandwidth", 12000, true);
	const int outgoing_bw = g_Config.getInt("Network", "OutgoingBandwidth", 12000, true);
	m_NetworkServer = new Server(local_node, incoming_bw, outgoing_bw);

	if (external_ip.empty()) {
		if (!local_node.isRoutable()) {
			CSPLOG(Prio_WARNING, Cat_APP) << "no external ip address specified; accepting only local (LAN) connections";
		}
	} else {
		auto external_address = boost::asio::ip::make_address(external_ip);
		if (!local_node.isRoutable()) {
			NetworkNode external_node(external_address, port);
			CSPLOG(Prio_INFO, Cat_APP) << "external interface is " << external_node;
			if (external_node.isRoutable()) {
				m_NetworkServer->setExternalNode(external_node);
			} else {
				CSPLOG(Prio_ERROR, Cat_APP) << "external interface is not routable; ignoring ExternalIp and accepting only local (LAN) connections";
			}
		} else {
			if (external_address != address) {
				CSPLOG(Prio_ERROR, Cat_APP) << "binding to a routable interface that does not match the specified external ip (" << external_ip << "); ignoring ExternalIp";
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
	CSPLOG(Prio_INFO, Cat_APP) << "starting network loop";
	Timer timer;
	timer.start();
	while (!sigint_quit) {
		m_NetworkServer->processAndWait(0.01, 0.01, 0.1);
		m_Battlefield->update(timer.incremental());
	}
}

int IndexServer::main() {
	csp::AutoTrace::install();

	// We do want to install a handler so we can respond to CTRL+C etc.
	sigint_original = signal(SIGINT, sigint_handler);

	if (!csp::openConfig("IndexServer.ini", false)) {
		std::cerr << "Unable to open config file 'IndexServer.ini'\n";
		return 1;
	}

	// csp::log().logToFile("IndexServer.log");
	csp::log().setCategories(Cat_ALL &~ (csp::Cat_TIMING));
	csp::log().setPriority(csp::g_Config.getInt("Debug", "LogPriority", Prio_INFO, true));

	csp::IndexServer server;
	server.run();
	return 0;
}

} // namespace csp

