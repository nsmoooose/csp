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
 * @file NetworkMessenger.cpp:
 * @author Scott Flicker (Wolverine)
 */

#include <SimNet/Networking.h>
#include <SimData/LogStream.h>
#include <SimCore/Util/Log.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/NetworkMessenger.h>

NetworkMessenger::NetworkMessenger() {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger.NetworkMessenger()");

	m_UDPReceiverSocket = new ost::UDPSocket();
	m_UDPSenderSocket   = new ost::UDPSocket();
	m_receiverAddr = NULL;
	m_receiverPort = 0;

//	m_messageSocketDuplex = new MessageSocketDuplex();
	m_originatorNode = new NetworkNode();

	m_messageSendArrayMax = 200;
	m_messageSendArrayCount = 0;
	m_messageSendArray.reserve(m_messageSendArrayMax);

	m_messageReceiveArrayMax = 200;
	m_messageReceiveArrayCount = 0;
	m_messageReceiveArray.reserve(m_messageReceiveArrayMax);

//	m_ReceiveHandler = NULL;
}

NetworkMessenger::NetworkMessenger(NetworkNode * originatorNode) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger.NetworkMessenger()");
	m_receiverAddr = new ost::InetAddress();   // this should be to INADDR_ANY
	m_receiverPort = originatorNode->getPort();
	m_UDPReceiverSocket = new ost::UDPSocket(*m_receiverAddr, originatorNode->getPort());
	m_UDPSenderSocket   = new ost::UDPSocket();

//	m_messageSocketDuplex = new MessageSocketDuplex(originatorNode->getPort());
	m_originatorNode = originatorNode;

	m_messageSendArrayMax = 200;
	m_messageSendArrayCount = 0;
	m_messageSendArray.reserve(m_messageSendArrayMax);

	m_messageReceiveArrayMax = 200;
	m_messageReceiveArrayCount = 0;
	m_messageReceiveArray.reserve(m_messageReceiveArrayMax);

//	m_ReceiveHandler = NULL;
}

//NetworkMessenger::NetworkMessenger(ost::InetAddress & addr, Port port) {
//	m_receiverAddr = &addr;
//	m_receiverPort = port;
//	m_UDPReceiverSocket = new ost::UDPSocket(addr, port);
//	m_UDPSenderSocket   = new ost::UDPSocket();
//
//	m_originatorNode = new NetworkNode(addr, port);
//
//	m_messageSendArrayMax = 200;
//	m_messageSendArrayCount = 0;
//	m_messageSendArray.reserve(m_messageSendArrayMax);
//
//	m_messageReceiveArrayMax = 200;
//	m_messageReceiveArrayCount = 0;
//	m_messageReceiveArray.reserve(m_messageReceiveArrayMax);
//
//	m_ReceiveHandler = NULL;
//
//}

void NetworkMessenger::queueMessage(NetworkNode * node, NetworkMessage * message) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::queueMessage() - targetHost " << node->getAddress() << ", targetPort " << node->getPort());
	message->dumpMessageHeader();
	if (m_messageSendArrayCount >= m_messageSendArrayMax) {
		m_messageSendArrayMax += m_messageSendArrayGrow;
		m_messageSendArray.resize(m_messageSendArrayMax);
	}
	m_messageSendArray[m_messageSendArrayCount].m_destinationNode = node;
	m_messageSendArray[m_messageSendArrayCount].m_message = message;
	m_messageSendArrayCount++;
}

void NetworkMessenger::sendQueuedMessages() {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sendQueuedMessages()");

	for(int i=0;i<m_messageSendArrayCount;i++) {
		CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sendQueuedMessage() - Sending Message [" << i << "]");
		//	  m_messageSocketDuplex->sendto(m_messageSendArray[i].m_message,
		//			                m_messageSendArray[i].m_destinationNode);
		sendto(m_messageSendArray[i].m_message,
		m_messageSendArray[i].m_destinationNode);
		freeMessageBuffer(m_messageSendArray[i].m_message);
		m_messageSendArray[i].m_message = NULL;
	}
	m_messageSendArrayCount = 0;
}

void NetworkMessenger::receiveMessages()
{
  receiveMessages(1000);
}

void NetworkMessenger::receiveMessages(int max) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::receiveMessages()");
	//  NetworkMessage * networkMessageHandle;
	//  m_messageSocketDuplex->recvfrom(&networkMessageHandle);

	// receive up to a maximum messages or return if no
	// available messages.
	for (int i=0;i<max;i++) {
		NetworkMessage * networkMessage = receiveMessage();
		if ( networkMessage ) {
			CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::ReceiveMessages() - Received Message");
			for_each( m_MessageHandlerList.begin(), m_MessageHandlerList.end(), CallHandler(networkMessage));
			freeMessageBuffer(networkMessage);
			//std::list<NetworkMessageHandler *>::iterator iter = m_ReceiveHandlerList.begin();
			//std::list<NetworkMessageHandler *>::const_iterator end = m_ReceiveHandlerList.end();
			//for (;iter != end ; ++iter) {
			//	NetworkMessageHandler * handler = (NetworkMessageHandler*)(*iter);
			//	if (handler != NULL) {
			//		CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::ReceiveMessages() - Dispatching Message");	
			//		handler->process(networkMessage);
			//	}
			//}
		} else {
			return;
		}
	}
}

NetworkNode * NetworkMessenger::getOriginatorNode() {
	return m_originatorNode;
}

void NetworkMessenger::setOriginatorNode(NetworkNode * originatorNode) {
	m_originatorNode = originatorNode;
}

void NetworkMessenger::registerMessageHandler(NetworkMessageHandler * handler) {
	m_MessageHandlerList.push_back(handler);
}


// don't create NetworkMessage Objects directly. Instead create a fixed length binary
// buffer then cast it to a NetworkMessage pointer.
// Returns a initialized message buffer.
NetworkMessage * NetworkMessenger::allocMessageBuffer(int messageType, int payloadLen) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::allocMessageBuffer() - CurrentPoolSize " << m_messagePool.size());

	if (m_messagePool.empty()) {
		for(int i=0;i<100;i++) {
			simdata::uint8 * buffer  = new simdata::uint8[NETWORK_PACKET_SIZE];
			NetworkMessage * message = (NetworkMessage*)buffer;
			m_messagePool.push_back(message);
		}
	}
	NetworkMessage * message = m_messagePool.front();
	m_messagePool.pop_front();
	memset(message, 0x00, NETWORK_PACKET_SIZE);
	message->initialize( messageType, payloadLen, m_originatorNode);
	return message;
}

// return a zeroed message buffer. Noninitialized
NetworkMessage * NetworkMessenger::allocMessageBuffer() {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::allocMessageBuffer() - CurrentPoolSize " << m_messagePool.size());

	if (m_messagePool.empty()) {
		for(int i=0;i<100;i++) {
			simdata::uint8 * buffer  = new simdata::uint8[NETWORK_PACKET_SIZE];
			NetworkMessage * message = (NetworkMessage*)buffer;
			m_messagePool.push_back(message);
		}
	}
	NetworkMessage * message = m_messagePool.front();
	m_messagePool.pop_front();
	memset(message, 0x00, NETWORK_PACKET_SIZE);
	return message;
}

// cast the NetworkMessage pointer back to a binary buffer then free the buffer.
void NetworkMessenger::freeMessageBuffer(NetworkMessage * message) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::freeMessageBuffer() - CurrentPoolSize " << m_messagePool.size());

	memset(message, 0xFF, NETWORK_PACKET_SIZE);
	m_messagePool.push_back(message);
}


int NetworkMessenger::sendto(NetworkMessage * message, ost::InetHostAddress * remoteAddress, Port * remotePort) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sentto(message,addr,port)");
	message->dumpMessageHeader();

	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sentto(message,addr,port) - Setting Remote Peer");
	m_UDPSenderSocket->setPeer(*remoteAddress, *remotePort);

	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sentto(message,addr,port) - Sending Network Packet");
#ifdef _MSC_VER
	return m_UDPSenderSocket->send((const char *)message, NETWORK_PACKET_SIZE);
#else
	return m_UDPSenderSocket->send((const void *)message, NETWORK_PACKET_SIZE);
#endif

	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sendto(message,addr,port) - exiting");
}

NetworkMessage * NetworkMessenger::receiveMessage() {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::receiveMessage() - Receving Network Packet");

	if (m_UDPReceiverSocket->isPending(ost::Socket::pendingInput, 0)) {
		// get addr of next packet
		Port port;
		ost::InetHostAddress addr = m_UDPReceiverSocket->getPeer(&port);
		//printf("MessageSocketDuplex::recvfrom() - port: %d\n", port);
		//printf("MessageSocketDuplex::recvfrom() - hostname: %s\n", addr.getHostname());
		
		// peek at packet to verify this is a valid CSP packet. and if so get the packet type.
		
		int headerlen = 6;
		uint16 headerBuffer[6];
		//int numHeaderBytes = m_UDPReceiverSocket->peek(headerBuffer, headerlen);
		m_UDPReceiverSocket->peek(headerBuffer, headerlen);


		// TODO validation of header
		
		//simdata::uint8 * buffer = new simdata::uint8[512];
		int maxBufLen = 512;
		NetworkMessage * message = allocMessageBuffer();
		
		// get the packet
		//int numPacketBytes = m_UDPReceiverSocket->receive((void*)message, maxBufLen);
		m_UDPReceiverSocket->receive((void*)message, maxBufLen);

		return message;
	}
	return 0;
}

int NetworkMessenger::recvfrom(NetworkMessage ** message) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::recvfrom() - Receving Network Packet");

	if (m_UDPReceiverSocket->isPending(ost::Socket::pendingInput, 0)) {
		// get addr of next packet
		Port port;
		ost::InetHostAddress addr = m_UDPReceiverSocket->getPeer(&port);
		//printf("MessageSocketDuplex::recvfrom() - port: %d\n", port);
		//printf("MessageSocketDuplex::recvfrom() - hostname: %s\n", addr.getHostname());
		
		// peek at packet to verify this is a valid CSP packet. and if so get the packet type.
		
		int headerlen = 6;
		uint16 headerBuffer[6];
		//int numHeaderBytes = m_UDPReceiverSocket->peek(headerBuffer, headerlen);
		m_UDPReceiverSocket->peek(headerBuffer, headerlen);

		// TODO validation of header
		
		NetworkMessage * buffer = allocMessageBuffer();
		int maxBufLen = 512;
		
		// get the packet
		int numPacketBytes = m_UDPReceiverSocket->receive((void*)buffer, maxBufLen);
		*message = (NetworkMessage*)buffer;

		return numPacketBytes;
	}
	return 0;
}


int NetworkMessenger::sendto(NetworkMessage * message, NetworkNode * node) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sendto(message,node) - Sending Network Packet");
	ost::InetHostAddress address = node->getAddress();
	Port port = node->getPort();
	return sendto(message, &address, &port);
	//  return 0;
}


int NetworkMessenger::sendto(std::vector<RoutedMessage> * sendArray, int count) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::sendto(RoutedMessageArray,Count) - Sending Network Packet");
	return 0;
}

int NetworkMessenger::recvfrom(std::vector<RoutedMessage> * receiveArray, int * count) {
	CSP_LOG(NETWORK, DEBUG, "NetworkMessenger::recvfrom(RoutedMessageArray, count) - Receving Network Packet");

	if (m_UDPReceiverSocket->isPending(ost::Socket::pendingInput, 0)) {
		// get addr of next packet
		Port port;
		ost::InetHostAddress addr = m_UDPReceiverSocket->getPeer(&port);
		//printf("MessageSocketDuplex::recvfrom() - port: %d\n", port);
		//printf("MessageSocketDuplex::recvfrom() - hostname: %s\n", addr.getHostname());
		
		// peek at packet to verify this is a valid CSP packet. and if so get the packet type.
		
		int headerlen = 6;
		uint16 headerBuffer[6];
		//int numHeaderBytes = m_UDPReceiverSocket->peek(headerBuffer, headerlen);
		m_UDPReceiverSocket->peek(headerBuffer, headerlen);

		// TODO validation of header
		
		NetworkMessage * message = allocMessageBuffer();
		int maxBufLen = 512;
		
		// get the packet
		int numPacketBytes = m_UDPReceiverSocket->receive((void*)message, maxBufLen);
		//*message = (NetworkMessage*)buffer;
		// TODO add to receive vector.
		
		return numPacketBytes;
	}
	return 0;
}


bool operator<( const RemoteObjectKey & lhs, const RemoteObjectKey & rhs)
{
	if ( lhs.m_ipaddr != rhs.m_ipaddr ) return ( lhs.m_ipaddr < rhs.m_ipaddr);
	if ( lhs.m_port != rhs.m_port ) return ( lhs.m_port < rhs.m_port );
	return ( lhs.m_id < rhs.m_id );
}
