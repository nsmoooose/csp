// Combat Simulator Project - FlightSim Dem
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
 * @file NetworkSocket.cpp:
 * @author Scott Flicker (Wolverine)
 */
#include <SimNet/Networking.h>
#include "Log.h"

MessageSocketDuplex::MessageSocketDuplex()
{
  CSP_LOG(APP, DEBUG, "MessageSocketDuplex.MessageSocketDuplex()");
  m_UDPReceiverSocket = new ost::UDPSocket();
  m_UDPSenderSocket   = new ost::UDPSocket();
  m_receiverAddr = NULL;  
  m_receiverPort = 0;
}

MessageSocketDuplex::MessageSocketDuplex(Port port)
{
    // create receiver end point
  CSP_LOG(APP, DEBUG, "MessageSocketDuplex.MessageSocketDuplex(port) - port: " << port);
  m_UDPReceiverSocket = new ost::UDPSocket();
    m_receiverAddr = new ost::InetAddress();   // this should be to INADDR_ANY
    m_receiverPort = port;
    m_UDPReceiverSocket = new ost::UDPSocket(*m_receiverAddr, port);
    m_UDPSenderSocket   = new ost::UDPSocket();
}

MessageSocketDuplex::MessageSocketDuplex(ost::InetAddress & addr, Port port)
{
  CSP_LOG(APP, DEBUG, "MessageSocketDuplex.MessageSocketDuplex(addr,port) - addr: " << addr 
		  << ", port: " << port);
  m_UDPReceiverSocket = new ost::UDPSocket();
    m_receiverAddr = &addr;
    m_receiverPort = port;
    m_UDPReceiverSocket = new ost::UDPSocket(addr, port);
    m_UDPSenderSocket   = new ost::UDPSocket();
	
}

int MessageSocketDuplex::sendto(NetworkMessage * message, ost::InetHostAddress * remoteAddress, Port * remotePort)
{
    CSP_LOG(APP, DEBUG, "MessageSocketDuplex::sentto(message,addr,port)");
    printf("MessageSocketDuplex::sentto(message,addr,port)");
    message->dumpMessageHeader();
    
    CSP_LOG(APP, DEBUG, "MessageSocketDuplex::sentto(message,addr,port) - Setting Remote Peer");
    m_UDPSenderSocket->setPeer(*remoteAddress, *remotePort);
    
    CSP_LOG(APP, DEBUG, "MessageSocketDuplex::sentto(message,addr,port) - Sending Network Packet");
#ifdef _MSC_VER
    return m_UDPSenderSocket->send((const char *)message, NETWORK_PACKET_SIZE);
#else
    return m_UDPSenderSocket->send((const void *)message, NETWORK_PACKET_SIZE);
#endif

    CSP_LOG(APP, DEBUG, "MessageSocketDuplex::sendto(message,addr,port) - exiting");
    printf("MessageSocketDuplex::sendto(message,addr,port) - exiting");
}

int MessageSocketDuplex::recvfrom(NetworkMessage ** message)
{
    CSP_LOG(APP, DEBUG, "MessageSocketDuplex()::recvfrom() - Receving Network Packet");

    if (m_UDPReceiverSocket->isPending(ost::Socket::pendingInput, 0))
    {
	// get addr of next packet
	Port port;
	ost::InetHostAddress addr = m_UDPReceiverSocket->getPeer(&port);
        //printf("MessageSocketDuplex::recvfrom() - port: %d\n", port);
//	printf("MessageSocketDuplex::recvfrom() - hostname: %s\n", addr.getHostname());
	
	// peek at packet to verify this is a valid CSP packet. and if so get the packet type.
	
	int headerlen = 6;
	uint16 headerBuffer[6];
	int numHeaderBytes = m_UDPReceiverSocket->peek(headerBuffer, headerlen);


	// TODO validation of header
	
	simdata::uint8 * buffer = new simdata::uint8[512];
	int maxBufLen = 512;
	
	// get the packet
	int numPacketBytes = m_UDPReceiverSocket->receive((void*)buffer, maxBufLen);
	*message = (NetworkMessage*)buffer;

	return numPacketBytes;
	
    }
    return 0;

}

    
int MessageSocketDuplex::sendto(NetworkMessage * message, NetworkNode * node)
{

  CSP_LOG(APP, DEBUG, "MessageSocketDuplex::sendto(message,node) - Sending Network Packet");
  ost::InetHostAddress address = node->getAddress();
  Port port = node->getPort();
  return sendto(message, &address, &port);
//  return 0;
}


int MessageSocketDuplex::sendto(std::vector<RoutedMessage> * sendArray, int count)
{
	return 0;
}

int MessageSocketDuplex::recvfrom(std::vector<RoutedMessage> * receiveArray, int * count)
{
    CSP_LOG(NETWORK, DEBUG, "Receving Network Packet");

    if (m_UDPReceiverSocket->isPending(ost::Socket::pendingInput, 0))
    {
	// get addr of next packet
	Port port;
	ost::InetHostAddress addr = m_UDPReceiverSocket->getPeer(&port);
        //printf("MessageSocketDuplex::recvfrom() - port: %d\n", port);
//	printf("MessageSocketDuplex::recvfrom() - hostname: %s\n", addr.getHostname());
	
	// peek at packet to verify this is a valid CSP packet. and if so get the packet type.
	
	int headerlen = 6;
	uint16 headerBuffer[6];
	int numHeaderBytes = m_UDPReceiverSocket->peek(headerBuffer, headerlen);


	// TODO validation of header
	
//	NetworkMessage * message = NetworkMessagePool::getPool()->getMessageFromPool();
        NetworkMessage * message = (NetworkMessage*)(new simdata::uint8[512]);
	int maxBufLen = 512;
	
	// get the packet
	int numPacketBytes = m_UDPReceiverSocket->receive((void*)message, maxBufLen);
//	*message = (NetworkMessage*)buffer;

	return numPacketBytes;
	
    }
    return 0;

}
