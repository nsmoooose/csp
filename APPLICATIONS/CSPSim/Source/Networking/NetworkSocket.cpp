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
 * @file NetworkSocket.cpp:
 * @author Scott Flicker (Wolverine)
 */
#if _MSC_VER
#include <WinSock2.h>
#endif

#include "Networking.h"

NetworkSocket::NetworkSocket(NetworkNode * node, short port)
{
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    m_servaddr = new sockaddr_in;
    struct in_addr * addrptr = &m_servaddr->sin_addr;
 
    printf("Opening socket to %s on port %d\n", node->getNetworkName(), port);
    memset( m_servaddr, 0,  sizeof (struct sockaddr_in));
    m_servaddr->sin_family = AF_INET;
    m_servaddr->sin_port = htons( port );
    addrptr->s_addr = inet_addr( node->getNetworkName() );
}

int NetworkSocket::sendto(NetworkMessage * message)
{
    printf("Sending Network Packet\n"); 
#ifdef _MSC_VER
    return ::sendto(m_sockfd, (const char *)message->getBufferPtr(), message->getBufferLen(), 0, (const struct sockaddr *)m_servaddr, sizeof(sockaddr_in));
#else
	return ::sendto(m_sockfd, (const void *)message->getBufferPtr(), message->getBufferLen(), 0, (const struct sockaddr *)m_servaddr, sizeof(sockaddr_in));
#endif
}
