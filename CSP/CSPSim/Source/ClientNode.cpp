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
 * @file ClientNode.cpp
 *
 **/


#include <SimNet/Networking.h>
#include <cstdio>
#include <cstdlib>
#include "Config.h"
#include <SimNet/NetworkMessage.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/NetworkMessenger.h>
#include <SimNet/DispatchMessageHandler.h>
#include <SimNet/PrintMessageHandler.h>

#include "Config.h"

#include "Bus.h"

#include <SimData/Archive.h>
#include <SimData/Ref.h>
#include <SimData/Date.h>

#include <KineticsChannels.h>

#include <SimData/Types.h>
#include <SimData/ExceptionBase.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/FileUtility.h>
#include <SimData/GeoPos.h>

#include <osgDB/FileUtils>

#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library

#include <osg/Timer>
#include <osgDB/FileUtils>

#include <SDL/SDL.h>

using bus::Kinetics;

#include "ClientNode.h"
#include "Battlefield.h"

ClientNode::ClientNode() {
	CSP_LOG(NETWORK, INFO, "ClientNode::ClientNode()");
	m_battlefield = NULL;
	m_networkMessenger = NULL;
}

int ClientNode::run() {
	CSP_LOG(NETWORK, INFO, "ClientNode::run()");

	init();

	MessageHeader header;
	header.dumpOffsets();

	unsigned short messageType = 2;
	unsigned short payloadLen  = sizeof(int) + sizeof(double) + 3*sizeof(simdata::Vector3) + sizeof(simdata::Quat) /* + sizeof(simdata::Matrix3) + sizeof(double) */;

	//  ptrPayload->dumpOffsets();

	//  NetworkMessage * message = networkMessenger->getMessageFromPool(1, 100);
	//  memset(payloadPtr, 0 , 100);
	//  strcpy(payloadPtr, "Hello From CSP Network Test Client!");
	//  Port port = message->getOriginatorPort();

	simdata::uint32 id;
	simdata::uint32 type;
	float timestamp;
	DataChannel<simdata::Vector3>::Ref b_GlobalPosition;
	DataChannel<simdata::Vector3>::Ref b_AngularVelocity;
	DataChannel<simdata::Vector3>::Ref b_LinearVelocity;
	DataChannel<simdata::Quat>::Ref b_Attitude;

	b_GlobalPosition = DataChannel<simdata::Vector3>::newLocal(Kinetics::Position, simdata::Vector3::ZERO);
	b_AngularVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::AngularVelocity, simdata::Vector3::ZERO);
	b_LinearVelocity = DataChannel<simdata::Vector3>::newLocal(Kinetics::Velocity, simdata::Vector3::ZERO);
	b_Attitude = DataChannel<simdata::Quat>::newLocal(Kinetics::Attitude, simdata::Quat::IDENTITY);

	id = 1;
	type = 1;
	timestamp = 0.0;
	b_GlobalPosition->value() = simdata::Vector3(1.0, 1.0, 1.0);
	b_AngularVelocity->value() = simdata::Vector3(1.0, 1.0, 1.0);
	b_LinearVelocity->value() = simdata::Vector3(1.0, 1.0, 1.0);
	b_Attitude->value() = simdata::Quat(1.0, 1.0, 1.0, 0.0);

	// ptrPayload->id = 1;
	while(1) {
		NetworkMessage * message = m_networkMessenger->allocMessageBuffer(messageType, payloadLen);
		ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
		timestamp += 1.0;
		//    ptrPayload->timeStamp = timestamp;
		b_GlobalPosition->value() = simdata::Vector3(timestamp*1.0, timestamp*0.5, 1.0);

		simdata::MemoryWriter writer((simdata::uint8 *)ptrPayload);

		//    writer << id;
		//    writer << type;
		//    writer << timestamp;

		ptrPayload->timeStamp = timestamp;
		ptrPayload->id = 1;
		ptrPayload->objectType = 1;

		b_GlobalPosition->value().writeBinary((unsigned char *)&(ptrPayload->globalPosition),24);
		b_LinearVelocity->value().writeBinary((unsigned char *)&(ptrPayload->linearVelocity),24);
		b_AngularVelocity->value().writeBinary((unsigned char *)&(ptrPayload->angularVelocity),24);
		b_Attitude->value().writeBinary((unsigned char *)&(ptrPayload->attitude),32);

		//  b_GlobalPosition->value().serialize(writer);
		//  b_LinearVelocity->value().serialize(writer);
		//  b_AngularVelocity->value().serialize(writer);
		//  b_Attitude->value().serialize(writer);

		ptrPayload->dump();

		m_networkMessenger->queueMessage(m_remoteNode, message);

		m_networkMessenger->sendQueuedMessages();
		m_networkMessenger->receiveMessages();

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

void ClientNode::init() {
	CSP_LOG(NETWORK, INFO, "Network test client starting up...");
	CSP_LOG(NETWORK, INFO, "ClientNode::init()");
	int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
	csplog().setLogCategory(CSP_ALL);
	csplog().setLogPriority(level);
	std::string logfile = g_Config.getString("Debug", "LogFile", "ClientNode.log", true);
	csplog().setOutput(logfile);

	m_battlefield = new Battlefield();

	// setup osg search path for external data files
	std::string image_path = getDataPath("ImagePath");
	std::string model_path = getDataPath("ModelPath");
	std::string font_path = getDataPath("FontPath");
	std::string search_path;
	simdata::ospath::addpath(search_path, image_path);
	simdata::ospath::addpath(search_path, model_path);
	simdata::ospath::addpath(search_path, font_path);
	osgDB::setDataFilePathList(search_path);

	// open the primary data archive
	std::string cache_path = getCachePath();
	std::string archive_file = simdata::ospath::join(cache_path, "sim.dar");
	try {
		simdata::DataArchive *sim = new simdata::DataArchive(archive_file.c_str(), 1);
		assert(sim);
		m_DataManager.addArchive(sim);
	}
	catch (simdata::Exception &e) {
		CSP_LOG(APP, ERROR, "Error opening data archive " << archive_file);
		CSP_LOG(APP, ERROR, e.getType() << ": " << e.getMessage());
		throw e;
		//::exit(0);
	}

	int  height = 200;
	int  width = 200;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) != 0) {
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << "\n";
		CSP_LOG(APP, ERROR, "ERROR! Unable to initialize SDL: " << SDL_GetError());
		return;
	}
	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	int bpp = info->vfmt->BitsPerPixel;

	Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF;

	SDL_Surface * m_SDLScreen = SDL_SetVideoMode(width, height, bpp, flags);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	if (m_SDLScreen == NULL) {
		std::cerr << "Unable to set video mode: " << SDL_GetError() << "\n";
		CSP_LOG(APP, ERROR, "ERROR! Unable to initialize SDL: " << SDL_GetError());
		return;
	}

	initNetworking();
}

void ClientNode::dumpSizes() {
	CSP_LOG(NETWORK, INFO, "sizeof(int) = " << sizeof(int));
	CSP_LOG(NETWORK, INFO, "sizeof(double) = " << sizeof(double));
	CSP_LOG(NETWORK, INFO, "sizeof(simdata::Vector3) = " << sizeof(simdata::Vector3));
	CSP_LOG(NETWORK, INFO, "sizeof(simdata::Quat) = " << sizeof(simdata::Quat));
	CSP_LOG(NETWORK, INFO, "sizeof(simdata::SimTime) = " << sizeof(simdata::SimTime));
	CSP_LOG(NETWORK, INFO, "sizeof(_Vector3Struct) = " << sizeof(_Vector3Struct));
	CSP_LOG(NETWORK, INFO, "sizeof(_QuatStruct) = " << sizeof(_QuatStruct));
	CSP_LOG(NETWORK, INFO, "sizeof(MessageHeader) = " << sizeof(MessageHeader));
	CSP_LOG(NETWORK, INFO, "sizeof(NetworkMessage) = " << sizeof(NetworkMessage));
	CSP_LOG(NETWORK, INFO, "sizeof(ObjectUpdateMessagePayload) = " << sizeof(ObjectUpdateMessagePayload));
}

void ClientNode::initNetworking() {
	CSP_LOG(NETWORK, INFO, "ClientNode::initNetworking()");
	m_localPort = g_Config.getInt("Networking", "LocalMessagePort", 10000, true);
	m_localHost = g_Config.getString("Networking", "LocalMessageHost", "127.0.0.1", true);

	m_remotePort = (Port)g_Config.getInt("Networking", "RemoteMessagePort", 0, true);
	m_remoteHost = g_Config.getString("Networking", "RemoteMessageHost", "127.0.0.1", true);

	m_remoteNode = new NetworkNode(m_remoteHost.c_str(), m_remotePort );
	m_localNode =  new NetworkNode(m_localHost.c_str(), m_localPort);

	m_networkMessenger = new NetworkMessenger(m_localNode);

	PrintMessageHandler * printMessageHandler = new PrintMessageHandler();
	printMessageHandler->setFrequency(1);

	m_networkMessenger->registerMessageHandler(printMessageHandler);

	DispatchMessageHandler * dispatchMessageHandler = new DispatchMessageHandler();
	dispatchMessageHandler->setLocalAddress( m_localNode->getAddress().getAddress().s_addr );
	dispatchMessageHandler->setLocalPort(m_localPort);
	dispatchMessageHandler->setBattlefield(m_battlefield);
	dispatchMessageHandler->setDataManager(m_DataManager);

	m_networkMessenger->registerMessageHandler(dispatchMessageHandler);

}
