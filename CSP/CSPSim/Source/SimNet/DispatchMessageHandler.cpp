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
 * @file ObjectUpdateMessage.cpp
 * @author Scott Flicker (Wolverine)
 */

#include <SimNet/Networking.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/NetworkMessageHandler.h>
#include <SimNet/DispatchMessageHandler.h>

#include "DynamicObject.h"
#include "Config.h"

#include <SimCore/Battlefield/Battlefield.h>
#include <SimData/FileUtility.h>

extern SimpleConfig g_Config;

DispatchMessageHandler::DispatchMessageHandler() {
  _addr = 0;
  _port = 0;
}

DispatchMessageHandler::~DispatchMessageHandler() {
}

void DispatchMessageHandler::process(NetworkMessage * message) {
	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::process()");
	NetworkNode * node = message->getOriginatorNode();
	unsigned int addr = node->getAddress().getAddress().s_addr;
	unsigned short port = node->getPort();
	if (isLocal(addr, port)) return;
	//simdata::uint16 messageType = message->getType();

	void * ptr = message->getPayloadPtr();
	ObjectUpdateMessagePayload * messagePayload = (ObjectUpdateMessagePayload*)ptr;

	unsigned int objectID = messagePayload->id;
	//unsigned int objectType = messagePayload->id;
	RemoteObjectKey key(addr, port, objectID);

	simdata::Ref<DynamicObject> obj = m_RemoteObjectTable.getRemoteObject( key );

	if (!obj) {
		CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::process() - object not found in database so adding it");
		// TODO create new object and add it to table.
		//
		// set remoteObjectWrapper to value of new object.
		obj = addRemoteObject(message);	
		CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::process() - wrapping new object");
		// TODO add new object to wrapper object
		CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::process() - putting new object in remote object table");
		m_RemoteObjectTable.putRemoteObject( key, obj );
	}
	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::process() - updating object with message");
	//  obj->putUpdateMessage(message);
	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::process() - finished updating object with message");
}

bool DispatchMessageHandler::isLocal(unsigned int addr, unsigned short port)
{
  return ((_addr == addr) && (_port == port));
}


simdata::Ref<DynamicObject> DispatchMessageHandler::addRemoteObject( NetworkMessage * message ) {
	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::addRemoteObject() - adding new object");

	// use hard coded mirage for now.
	std::string vehicle = g_Config.getPath("Testing", "Vehicle", "sim:vehicles.aircraft.m2k", false);
	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::addRemoteObject() - calling dataManager using path: "
	<< vehicle.c_str());

	simdata::Ref<DynamicObject> ao = _dataManager.getObject(vehicle.c_str());
	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::addRemoteObject() - asserting validity of new object");
	assert(ao.valid());

	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::addRemoteObject() - sending update message to new object");
	//  ao->putUpdateMessage(message);

	CSP_LOG(NETWORK, DEBUG, "DispatchMessageHandler::addRemoteObject() - adding new object to battlefield");
	// FIXME human or agent?
	_battlefield->addUnit(ao, true /* human */);
	return ao;
}
