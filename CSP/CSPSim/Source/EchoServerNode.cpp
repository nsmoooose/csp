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
 * @file EchoServerNode.cpp
 *
 **/

#include <SimNet/Networking.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/NetworkMessenger.h>
#include <SimNet/EchoMessageHandler.h>
#include <SimNet/PrintMessageHandler.h>

#include "EchoServerNode.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "Config.h"
#include "Log.h"

#include <SimData/Ref.h>
#include <SimData/Date.h>
#include <SimData/DataManager.h>

#include <KineticsChannels.h>

#include <SimData/Types.h>
#include <SimData/ExceptionBase.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/FileUtility.h>
#include <SimData/GeoPos.h>
#include <SimData/Timing.h>

using bus::Kinetics;

EchoServerNode::EchoServerNode() {
}

int EchoServerNode::run() {
	int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
	csplog().setLogCategory(CSP_ALL);
	csplog().setLogPriority(level);
	std::string logfile = g_Config.getString("Debug", "LogFile", "ServerNode.log", true);
	csplog().setOutput(logfile);

	CSP_LOG(NETWORK, INFO, "Network test echo server starting up...");

	//Port remotePort = g_Config.getInt("Networking", "LocalMessagePort", 10000, true);
	std::string remoteHost = g_Config.getString("Networking", "LocalMessageHost", "127.0.0.1", true);

	Port localPort = (Port)g_Config.getInt("Networking", "RemoteMessagePort", 0, true);
	std::string localHost = g_Config.getString("Networking", "RemoteMessageHost", "127.0.0.1", true);

	//NetworkNode * remoteNode = new NetworkNode(remoteHost.c_str(), remotePort );
	NetworkNode * localNode =  new NetworkNode(localHost.c_str(), localPort);

	NetworkMessenger * networkMessenger = new NetworkMessenger(localNode);
	PrintMessageHandler * printMessageHandler = new PrintMessageHandler();
	printMessageHandler->setFrequency(1);
	networkMessenger->registerMessageHandler(printMessageHandler);

	EchoMessageHandler * echoMessageHandler = new EchoMessageHandler();
	echoMessageHandler->setMessenger(networkMessenger);
	networkMessenger->registerMessageHandler(echoMessageHandler);

	//MessageSocketDuplex * socketDuplex = new MessageSocketDuplex(localPort);
	//NetworkMessage * message=NULL;

	while(1) {
		networkMessenger->receiveMessages();
		int count = networkMessenger->getSendQueueCount();
		CSP_LOG(NETWORK, INFO, "SendQueueCount: " << count);
		networkMessenger->sendQueuedMessages();

		simdata::tstart();

		simdata::tend();
		double etime;
		etime = simdata::tval();
		while((etime = simdata::tval()) < 3.0 ) {
			simdata::tend();
		}
	}

	return 0;
}


