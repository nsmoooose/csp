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

NetworkNode * g_node;

NetworkMessenger::NetworkMessenger()
{
  m_messageSocketDuplex = new MessageSocketDuplex();
  m_orginatorNode = new NetworkNode();
}

NetworkMessenger::NetworkMessenger(NetworkNode * orginatorNode)
{
  m_messageSocketDuplex = new MessageSocketDuplex(orginatorNode->getPort());
  m_orginatorNode = orginatorNode;
}

void NetworkMessenger::queueMessage(NetworkNode * node, NetworkMessage * message)
{
  CSP_LOG(APP, DEBUG, "NetworkMessenger::queueMessage()");
  m_messageList.push_back(message);
  g_node = node;
}

void NetworkMessenger::sendMessages()
{
  CSP_LOG(APP, DEBUG, "NetworkMessenger::sendMessages()");
  std::list<NetworkMessage*>::iterator i = m_messageList.begin();
  std::list<NetworkMessage*>::const_iterator end = m_messageList.end();
  for ( ; i != end ; ++i )
  {
	m_messageSocketDuplex->sendto((*i), g_node);
  }
	
}

void NetworkMessenger::receiveMessages()
{
  CSP_LOG(APP, DEBUG, "NetworkMessenger::ReceiveMessage()");

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
  simdata::uint8 * buffer  = new simdata::uint8[NETWORK_PACKET_SIZE]; 
  NetworkMessage * message = (NetworkMessage*)buffer;
  message->initialize( messageType, payloadLen, m_orginatorNode);
  return message;
}

// cast the NetworkMessage pointer back to a binary buffer then free the buffer.
void NetworkMessenger::returnMessageToPool(NetworkMessage * message)
{
  simdata::uint8 * buffer = (simdata::uint8 *)message;
  delete buffer;
}

