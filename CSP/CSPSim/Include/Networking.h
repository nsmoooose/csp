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
#include <list>

#include <SimData/Vector3.h>
#include <SimData/String.h>
#include <SimData/Uniform.h>

typedef int SockFd;
typedef simdata::uint16 Port;

class NetworkMessage
{
   
    protected: 
    
    simdata::uint8 * m_Buf;
    simdata::uint8 * m_PayloadBuf;
    simdata::uint16 m_BufferLen;
    simdata::uint16 m_MessageType;
    simdata::uint16 m_PayloadLen;
    bool m_Initialized;

    static unsigned short magicNumber;
    static unsigned short m_HeaderLen;
                
    public:

    NetworkMessage();
    virtual ~NetworkMessage();

    bool initialize(simdata::uint16 type, simdata::uint16 payloadLength);
   
    simdata::uint16 getType();

    void * getBufferPtr();
    void * getPayloadPtr();

    simdata::uint16 getBufferLen();
    simdata::uint16 getPayloadLen();
    simdata::uint16 getHeaderLen();

    bool isInitialized();
    bool isHeaderValid();
   
};

class NetworkMessagePool
{


};
                                            
class ObjectUpdateMessage : public NetworkMessage
{

    public:
    ObjectUpdateMessage(simdata::uint16 id, simdata::Vector3 position, simdata::Vector3 velocity);
    
    
};


class NetworkNode
{
  private:
    uint32 m_node_id;
    ost::InetHostAddress m_addr;
    Port m_port;
    
  public:
    NetworkNode();
    NetworkNode(int node_id, ost::InetHostAddress addr, Port port);

    void setAddress(ost::InetHostAddress addr);
    void setPort(Port port);
    void setId(int node_id);

    int getId();
    Port getPort();
    ost::InetHostAddress getAddress();
    
};

class MessageSocketDuplex
{
    ost::UDPSocket * m_UDPReceiverSocket;
    ost::UDPSocket * m_UDPSenderSocket;	
   
    /*
    SockFd m_receiverSockFd;
    SockFd m_senderSockFd;
    */
    
    /*
    struct sockaddr_in m_receiverSocketAddress;
    NetworkAddress * m_receiverAddress;
    */
    
    ost::InetAddress * m_receiverAddr;
    Port m_receiverPort;
    
    public:
    MessageSocketDuplex();                                     // set the listener port to unbound.
    MessageSocketDuplex(ost::InetAddress & Address, Port port);     // set the bound address and port.
    MessageSocketDuplex(Port port);                            // set the bound port.
    
    /*
    void bind(NetworkAddress * address, Port port);                    // binds the listener port.
    */
    
    int sendto(NetworkMessage & message, ost::InetHostAddress * remoteAddress, Port * remotePort);   
    int recvfrom(NetworkMessage & message, ost::InetHostAddress * remoteAddress=NULL, Port * remotePort=NULL);

    int sendto(NetworkMessage & message, NetworkNode * node);   
    int recvfrom(NetworkMessage & message, NetworkNode * node);
 
    ost::InetAddress * getReciverAddress() { return m_receiverAddr; }
    Port getReceiverPort() { return m_receiverPort; }

};


class NetworkMessenger
{
   private: 
      MessageSocketDuplex * m_messageSocketDuplex;
      std::list<NetworkMessage*> m_messageList;

   public:
      
    NetworkMessenger();
    NetworkMessenger(Port port);

    void queueMessage(NetworkNode * node, NetworkMessage * message);
    void sendMessages();
    void receiveMessages();
    
};

class NetworkBroadcaster
{
    public:
   NetworkBroadcaster();
    
};

class NetworkListener
{
    
    public:
    
    NetworkListener();
    
};


class NetworkSocket
{

  public:
    NetworkSocket();
};


class NetworkAddress
{
    private:
      struct in_addr m_addr;
      simdata::String m_name;
      simdata::String m_IPAddress;
    
    public:
      NetworkAddress();
      NetworkAddress(const simdata::String name);
      NetworkAddress(NetworkAddress & addr);
    
      simdata::String getNetworkName();
      simdata::String getNetworkIP();
      void setByNetworkName(const simdata::String & name);
      void setByNetworkIP(const simdata::String & address);
    
};


#endif

