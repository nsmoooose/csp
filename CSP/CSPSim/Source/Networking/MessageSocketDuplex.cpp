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
#include "Networking.h"
#include "Log.h"

MessageSocketDuplex::MessageSocketDuplex()
{
  m_UDPReceiverSocket = new ost::UDPSocket();
  m_UDPSenderSocket   = new ost::UDPSocket();
  m_receiverAddr = NULL;  
  m_receiverPort = 0;
}

MessageSocketDuplex::MessageSocketDuplex(Port port)
{
    // create receiver end point
    m_receiverAddr = new ost::InetAddress();   // this should be to INADDR_ANY
    m_receiverPort = port;
    m_UDPReceiverSocket = new ost::UDPSocket(*m_receiverAddr, port);
    m_UDPSenderSocket   = new ost::UDPSocket();
    
    /*
    m_receiverSockFd = socket(AF_INET, SOCK_DGRAM, 0);
    m_senderSockFd = socket(AF_INET, SOCK_DGRAM, 0);
    */

    /*
    memset( (void*)&m_receiverSocketAddress, 0,  sizeof (struct sockaddr_in));

    m_receiverSocketAddress.sin_family = AF_INET;
    m_receiverSocketAddress.sin_port = htons( port );
    m_receiverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    ::bind(m_receiverSockFd, (struct sockaddr *)&m_receiverSocketAddress, sizeof(m_receiverSocketAddress));
    */
    
    // create sender end point
    
    
}

MessageSocketDuplex::MessageSocketDuplex(ost::InetAddress & addr, Port port)
{
    m_receiverAddr = &addr;
    m_receiverPort = port;
    m_UDPReceiverSocket = new ost::UDPSocket(addr, port);
    m_UDPSenderSocket   = new ost::UDPSocket();
	
}

/*
void MessageSocketDuplex::bind(NetworkAddress * address, Port port)
{

}
*/

int MessageSocketDuplex::sendto(NetworkMessage & message, ost::InetHostAddress * remoteAddress, Port * remotePort)
{
//    CSP_LOG(NETWORK, DEBUG, "Sending Network Packet");

    m_UDPSenderSocket->setPeer(*remoteAddress, *remotePort);
    
#ifdef _MSC_VER
    return m_UDPSenderSocket->send((const char *)message.getBufferPtr(), message.getBufferLen());
#else
    return m_UDPSenderSocket->send((const void *)message.getBufferPtr(), message.getBufferLen());
#endif

    /*

    struct sockaddr_in servaddr;
    memset((void*)&servaddr, 0, sizeof(sockaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(*remotePort);
    servaddr.sin_addr.s_addr = inet_addr(remoteAddress->getNetworkName().c_str());
    
#ifdef _MSC_VER
    return ::sendto(m_senderSockFd, (const char *)message->getBufferPtr(), message->getBufferLen(), 0, (const struct sockaddr *)&servaddr, sizeof(sockaddr));
#else
	return ::sendto(m_senderSockFd, (const void *)message->getBufferPtr(), message->getBufferLen(), 0, (const struct sockaddr *)&servaddr, sizeof(sockaddr));
#endif

    */
	
}

int MessageSocketDuplex::recvfrom(NetworkMessage & message, ost::InetHostAddress * remoteAddress, Port * remotePort)
{
//    CSP_LOG(NETWORK, DEBUG, "Receving Network Packet");

    if (m_UDPReceiverSocket->isPending(ost::Socket::pendingInput, 0))
    {
	// get addr of next packet
	Port port;
	ost::InetHostAddress addr = m_UDPReceiverSocket->getPeer(&port);


	// TODO may need to validate addr, and port as being from a register node.
	if (remotePort)
		*remotePort = port;
	if (remoteAddress)
		*remoteAddress = addr;
	
	// peek at packet to verify this is a valid CSP packet. and if so get the packet type.
	
	int headerlen = 6;
	uint16 headerBuffer[6];
	int numHeaderBytes = m_UDPReceiverSocket->peek(headerBuffer, headerlen);

	// TODO validation of header
	
	// get the packet
	int numPacketBytes = m_UDPReceiverSocket->receive((void*)message.getBufferPtr(), 
			                                 message.getBufferLen());

	return numPacketBytes;
	
    }
    return 0;
		    

    
    /*
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    return ::recvfrom(m_receiverSockFd, (void *)message->getBufferPtr(), message->getBufferLen(), 
		    0, (struct sockaddr *)&cliaddr, &clilen);
		    */

}

    
int MessageSocketDuplex::sendto(NetworkMessage & message, NetworkNode * node)
{

//  CSP_LOG(NETWORK, DEBUG, "Sending Network Packet");
  ost::InetHostAddress address = node->getAddress();
  Port port = node->getPort();
  return sendto(message, &address, &port);
//  return 0;
}


int MessageSocketDuplex::recvfrom(NetworkMessage & message, NetworkNode * node)
{
//  CSP_LOG(NETWORK, DEBUG, "Receiving Network Packet");
  ost::InetHostAddress address = node->getAddress();
  Port port = node->getPort();
  return recvfrom(message, &address, &port);
//    return 0;
}
 

