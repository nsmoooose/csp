
// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file PringMessageHandler.cpp:
 * @author Scott Flicker (Wolverine)
 *
 * Class for Handling Network Messages.
 * Prints the contents of the message. 
 */


#include <SimData/LogStream.h>
#include "Log.h"
#include <SimNet/Networking.h>
#include <stdio.h>

PrintMessageHandler::PrintMessageHandler()
{ 
  m_frequency = 1; 
  m_count = 0;
}

PrintMessageHandler::~PrintMessageHandler()
{

}

void PrintMessageHandler::process(NetworkMessage * message)
{
  CSP_LOG(APP, DEBUG, "PrintMessageHandler::process()" );
  if (m_count % m_frequency == 0)
  {
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Printing Message" );
    NetworkNode * node = message->getOriginatorNode();
    MessageHeader * header = (MessageHeader*)message;
    header->dumpOffsets();
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Counter: " << m_count );
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Received Data From Remote Node" );
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - Remote addr: " << node->getHostname());
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - Remote port: " << node->getPort());
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - MagicNumber: 0x" << std::hex << header->m_magicNumber );
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - PayloadLen: " << header->m_payloadLen );
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - MessageType: " << header->m_messageType);
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - IPAddr: " << header->m_ipaddr);
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - Port: " << header->m_port);
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Header Info - Node ID: " << header->m_id);
			  
    ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Payload Info - ID: " << ptrPayload->id);
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Payload Info - TYPE: " << ptrPayload->objectType);
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Payload Info - TimeStamp: " << ptrPayload->timeStamp);
	  
    CSP_LOG(APP, DEBUG, "PrintMessageHandler::process() - Payload Info - " <<
		    "  PosX: " << ptrPayload->globalPosition.x << 
		    ", PosY: " << ptrPayload->globalPosition.y << 
		    ", PosZ: " << ptrPayload->globalPosition.z); 
//    printf("LVeloX: %f, LVeloY: %f, LVeloZ: %f\n", 
//			ptrPayload->linearVelocity.x,
//			ptrPayload->linearVelocity.y,
//			ptrPayload->linearVelocity.z);
//    printf("AngVeloX: %f, AngVeloY: %f, AngVeloZ: %f\n", 
//			ptrPayload->angularVelocity.x,
//			ptrPayload->angularVelocity.y,
//			ptrPayload->angularVelocity.z);
//    printf("AttX: %f, AttY: %f, AttZ: %f, AttW: %f\n", 
//			ptrPayload->attitude.x,
//			ptrPayload->attitude.y,
//			ptrPayload->attitude.z,
//			ptrPayload->attitude.w);
  }
  m_count++;
}


