
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


#include "Networking.h"
#include <stdio.h>

void PrintMessageHandler::process(NetworkMessage * message, NetworkMessenger * messenger)
{
  if (m_count % m_frequency == 0)
  {
    NetworkNode * node = message->getOriginatorNode();
    MessageHeader * header = (MessageHeader*)message;
    header->dumpOffsets();
    std::cout << "Received Data From Client:" << std::endl;
    std::cout << "Client addr: " << node->getHostname() << std::endl;
    std::cout << "Client port: " << node->getPort() << std::endl;
    std::cout << "Header Information" << std::endl;
    std::cout << "MagicNumber: " << std::hex << header->m_magicNumber << std::endl;
    std::cout << "PayloadLen: " << header->m_payloadLen << std::endl;
    std::cout << "MessageType: " << header->m_messageType << std::endl;
    std::cout << "IPAddr: " << header->m_ipaddr << std::endl;
    std::cout << "Port: " << std::hex << header->m_port << std::endl;
    std::cout << "ID: " << header->m_id << std::endl;
			  
    ObjectUpdateMessagePayload * ptrPayload = (ObjectUpdateMessagePayload*)message->getPayloadPtr();
    ptrPayload->dumpOffsets();
    printf("Payload Information\n");
    printf("ID: %u\n", ptrPayload->id);
    printf("TYPE: %u\n", ptrPayload->objectType);
    printf("TimeStamp: %f\n", ptrPayload->timeStamp);
    printf("PosX: %f, PosY: %f, PosZ: %f\n", 
			ptrPayload->globalPosition.x,
			ptrPayload->globalPosition.y,
			ptrPayload->globalPosition.z);
    printf("LVeloX: %f, LVeloY: %f, LVeloZ: %f\n", 
			ptrPayload->linearVelocity.x,
			ptrPayload->linearVelocity.y,
			ptrPayload->linearVelocity.z);
    printf("AngVeloX: %f, AngVeloY: %f, AngVeloZ: %f\n", 
			ptrPayload->angularVelocity.x,
			ptrPayload->angularVelocity.y,
			ptrPayload->angularVelocity.z);
    printf("AttX: %f, AttY: %f, AttZ: %f, AttW: %f\n", 
			ptrPayload->attitude.x,
			ptrPayload->attitude.y,
			ptrPayload->attitude.z,
			ptrPayload->attitude.w);
  }
  m_count++;
}


