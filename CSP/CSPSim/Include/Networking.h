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
 * @file Networking.h
 *
 */

#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>

#ifndef _MSC_VER
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <SimData/Vector3.h>

class NetworkMessage
{
   
    protected: 
    
    unsigned char * m_Buf;
    unsigned char * m_PayloadBuf;
    unsigned short m_BufferLen;
    unsigned short m_MessageType;
    unsigned short m_PayloadLen;
    bool m_Initialized;

    static unsigned short magicNumber;
    static unsigned short m_HeaderLen;
                
    public:

    NetworkMessage();
    virtual ~NetworkMessage();

    bool initialize(unsigned short type, unsigned short payloadLength);
   
    unsigned short getType();

    void * getBufferPtr();
    void * getPayloadPtr();

    unsigned short getBufferLen();
    unsigned short getPayloadLen();
    unsigned short getHeaderLen();

    bool isInitialized();
    bool isHeaderValid();
   
};

class NetworkMessagePool
{


};
                                            
class ObjectUpdateMessage : public NetworkMessage
{

    public:
    ObjectUpdateMessage(unsigned int id, simdata::Vector3 position, simdata::Vector3 velocity);
    
    
};

class NetworkNode
{
    
    public:
    NetworkNode();
    
    char * getNetworkName();
    
};

class NetworkSocket
{
    int m_sockfd;
    struct sockaddr_in * m_servaddr;
    
    public:
    NetworkSocket(NetworkNode * node, short port);
    int sendto(NetworkMessage * message);    
};

class NetworkMessenger
{
    NetworkMessenger();
};

class NetworkBroadcaster
{
    short  m_server_port;
    NetworkNode * m_node;
    NetworkSocket * m_socket;
    
    public:
   NetworkBroadcaster();
   
   void sendMessage( int NodeID, NetworkMessage * message);
    
};

class NetworkListener
{
    
    public:
    
    NetworkListener();
    void receiveAvailableMessages();
    
};

#endif

