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
	const std::string interface = g_Config.getString("Network", "Bind", "127.0.0.1:4999", true);
	const std::string::size_type colon = interface.find(':');
	std::string address;
	simnet::Port port = 0;
	if (colon != std::string::npos) {
		port = atoi(interface.substr(colon + 1).c_str());
		address = interface.substr(0, colon);
	}
	if (address.size() == 0 || port == 0) {
		std::cerr << "Invalid value for Network.Bind in .ini file: " << interface << "\n";
		std::cerr << "Should be of the form www.xxx.yyy.zzz:port\n";
		::exit(1);
	}
	CSP_LOG(APP, INFO, "binding to interface " << address << ":" << port);
	simnet::NetworkNode local(address, port);
	m_NetworkServer = new simnet::Server(local, 100000, 100000); // TODO real bw?
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
	while (1) {
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

	simnet::netlog().setLogPriority(simdata::LOG_INFO);
	csplog().setOutput("IndexServer.log");
	csplog().setLogPriority(simdata::LOG_INFO);
	csplog().setLogCategory(simdata::LOG_ALL);

	IndexServer server;
	server.run();
}
