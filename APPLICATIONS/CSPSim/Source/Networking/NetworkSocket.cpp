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
#include "Networking.h"

NetworkSocket::NetworkSocket(NetworkNode * node, short port)
{
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    m_servaddr = new sockaddr_in;
 
    printf("Opening socket to %s on port %d\n", node->getNetworkName(), port);
    bzero( m_servaddr, sizeof (struct sockaddr_in));
    m_servaddr->sin_family = AF_INET;
    m_servaddr->sin_port = htons( port );
    inet_pton(AF_INET, node->getNetworkName(), (void*)&m_servaddr->sin_addr);
    
}

int NetworkSocket::sendto(NetworkMessage * message)
{
    printf("Sending Network Packet\n");   
    return ::sendto(m_sockfd, (const void *)message->getBufferPtr(), message->getBufferLen(), 0, (const struct sockaddr *)m_servaddr, sizeof(sockaddr_in));
}
