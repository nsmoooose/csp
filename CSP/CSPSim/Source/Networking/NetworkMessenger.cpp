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
 * @file NetworkMessenger.cpp:
 * @author Scott Flicker (Wolverine)
 */

#include <SimData/LogStream.h>
#include "Log.h"
#include "Networking.h"

NetworkMessenger::NetworkMessenger()
{
  m_messageSocketDuplex = new MessageSocketDuplex();
  m_orginatorNode = new NetworkNode();
  m_messageArrayMax = 200;
  m_messageArrayCount = 0;
  m_messageArray.reserve(m_messageArrayMax);
}

NetworkMessenger::NetworkMessenger(NetworkNode * orginatorNode)
{
  m_messageSocketDuplex = new MessageSocketDuplex(orginatorNode->getPort());
  m_orginatorNode = orginatorNode;
  m_messageArrayMax = 200;
  m_messageArrayCount = 0;
  m_messageArray.reserve(m_messageArrayMax);
}

void NetworkMessenger::queueMessage(NetworkNode * node, NetworkMessage * message)
{
  CSP_LOG(APP, DEBUG, "NetworkMessenger::queueMessage()");
  if (m_messageArrayCount >= m_messageArrayMax)
  {
    m_messageArrayMax += m_messageArrayGrow;
	m_messageArray.resize(m_messageArrayMax);
  }
  m_messageArray[m_messageArrayCount].m_destinationNode = node;
  m_messageArray[m_messageArrayCount].m_message = message;
  m_messageArrayCount++;
}

void NetworkMessenger::sendMessages()
{
  CSP_LOG(APP, DEBUG, "NetworkMessenger::sendMessages()");

  for(int i=0;i<m_messageArrayCount;i++) {
	  m_messageSocketDuplex->sendto(m_messageArray[i].m_message, 
			                          m_messageArray[i].m_destinationNode);
	  returnMessageToPool(m_messageArray[i].m_message);
	  m_messageArray[i].m_message = NULL;
    }
    m_messageArrayCount = 0;
	    	
}

void NetworkMessenger::receiveMessages()
{
  CSP_LOG(APP, DEBUG, "NetworkMessenger::ReceiveMessage()");
  NetworkMessage * networkMessageHandle;
  m_messageSocketDuplex->recvfrom(&networkMessageHandle);
}

NetworkNode * NetworkMessenger::getOrginatorNode()
{
  return m_orginatorNode;
}

void NetworkMessenger::setOrginatorNode(NetworkNode * orginatorNode)
{
  m_orginatorNode = orginatorNode;
}

// don't create NetworkMessage Objects directly. Instead create a fixed length binary 
// buffer then cast it to a NetworkMessage pointer.
NetworkMessage * NetworkMessenger::getMessageFromPool(int messageType, int payloadLen)
{
  //printf("NetworkMessenger::getMessageFromPool() - CurrentPoolSize: %d\n", m_messagePool.size());
  if (m_messagePool.empty())
  {
    for(int i=0;i<100;i++)
    {
      simdata::uint8 * buffer  = new simdata::uint8[NETWORK_PACKET_SIZE]; 
      NetworkMessage * message = (NetworkMessage*)buffer;
      m_messagePool.push_back(message);
    }
  }
  NetworkMessage * message = m_messagePool.front();
  m_messagePool.pop_front();
  memset(message, 0x00, NETWORK_PACKET_SIZE);
  message->initialize( messageType, payloadLen, m_orginatorNode);
  return message;
}

// cast the NetworkMessage pointer back to a binary buffer then free the buffer.
void NetworkMessenger::returnMessageToPool(NetworkMessage * message)
{
//  printf("NetworkMessenger::returnMessageToPool() - CurrentPoolSize: %d\n", m_messagePool.size());
  m_messagePool.push_back(message);	
}


