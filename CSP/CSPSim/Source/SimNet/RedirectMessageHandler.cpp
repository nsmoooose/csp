
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
 * @file RedirectMessageHandler.cpp:
 * @author Scott Flicker (Wolverine)
 *
 * Class for Handling Network Messages.
 * Redirects messages to other clients.
 */


#include <SimData/LogStream.h>
#include "Log.h"
#include <SimNet/Networking.h>

RedirectMessageHandler::RedirectMessageHandler() {
	m_messenger = NULL;
}

RedirectMessageHandler::~RedirectMessageHandler() {
}

void RedirectMessageHandler::process(NetworkMessage * message) {
	CSP_LOG(NETWORK, DEBUG, "RedirectMessageHandler::process()\n");

	NetworkNode * originatorNode = message->getOriginatorNode();
	CSP_LOG(NETWORK, DEBUG, "RedirectMessageHandler::process() - Sending back to host: " << originatorNode->getHostname() << ", port: " << originatorNode->getPort());

	// must make a copy of the message since the buffer in the parameter will be returned
	// to the pool after this function exists.
	NetworkMessage * messageCopy = m_messenger->allocMessageBuffer();
	memcpy((void*)messageCopy, (void*)message, 512);

	// MessageHeader * header = (MessageHeader*)messageCopy;
	// ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)messageCopy->getPayloadPtr();

	m_messenger->queueMessage(originatorNode, messageCopy);
}


