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
	const std::string server_interface = g_Config.getString("Network", "Bind", "127.0.0.1:4999", true);
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
	CSP_LOG(APP, INFO, "binding to interface " << address << ":" << port);
	simnet::NetworkNode local(address, port);
	const int incoming_bw = g_Config.getInt("Network", "IncomingBandwidth", 12000, true);
	const int outgoing_bw = g_Config.getInt("Network", "OutgoingBandwidth", 12000, true);
	m_NetworkServer = new simnet::Server(local, incoming_bw, outgoing_bw);
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
	simdata::Trace::install();

	if (!openConfig("IndexServer.ini")) {
		std::cerr << "Unable to open config file 'IndexServer.ini'\n";
		return 1;
	}

	csplog().setOutput("IndexServer.log");
	csplog().setLogCategory(simdata::LOG_ALL);
	csplog().setLogPriority(g_Config.getInt("Debug", "CspLoggingThreshold", simdata::LOG_INFO, true));
	simnet::netlog().setLogPriority(g_Config.getInt("Debug", "NetLoggingThreshold", simdata::LOG_INFO, true));

	IndexServer server;
	server.run();
}

