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

// use this to fix compile problems with mulitple includes of windows and winsock headers.
#define _WINSOCKAPI_

#include <cc++/network.h>


#include <sys/types.h>
#include <list>

#include <SimData/Vector3.h>
#include <SimData/String.h>
#include <SimData/Uniform.h>
#include <SimData/Quat.h>
#include <SimData/Date.h>

typedef int SockFd;
typedef simdata::uint16 Port;
class NetworkNode;

const short NETWORK_PACKET_SIZE = 512;

struct MessageHeader
{
  simdata::uint16  m_magicNumber;
  simdata::uint16  m_payloadLen;
  simdata::uint16  m_messageType;
  simdata::uint32  m_ipaddr;
  simdata::uint16  m_port;
  simdata::uint16  m_id;
};

class NetworkMessage
{
   
    protected: 
    MessageHeader m_header;
    simdata::uint8 m_payloadBuf[];
    
    static unsigned short m_magicNumber;
    static unsigned short m_HeaderLen;
                
    private:

    NetworkMessage();

    public:
    
    bool initialize(simdata::uint16 type, simdata::uint16 payloadLength, NetworkNode * senderNode);
   
    simdata::uint16 getType();

    void * getPayloadPtr();
    simdata::uint16 getPayloadLen();

    Port getOriginatorPort();
    NetworkNode * getOriginatorNode();
    
    bool isInitialized();
    bool isHeaderValid();
   
};

class NetworkMessagePool
{


};
                                            
class ObjectUpdateMessagePayload 
{

    private:
      ObjectUpdateMessagePayload();

    public:
      unsigned int id;
      simdata::SimTime timeStamp;
      simdata::Vector3 globalPosition;
      simdata::Vector3 linearVelocity;
      simdata::Vector3 angularVelocity;
      simdata::Quat  attitude;
    
    
};


class NetworkNode
{
  private:
    ost::InetHostAddress m_addr;
    Port m_port;
    uint16 m_node_id;
    
  public:
    NetworkNode();
    NetworkNode(int node_id, ost::InetHostAddress addr, Port port);
    NetworkNode(int node_id, simdata::uint32 addr, Port port);
    NetworkNode(int node_id, const char * hostname, Port port);


    void setAddress(ost::InetHostAddress addr);
    void setPort(Port port);
    void setId(short node_id);

    short getId();
    Port getPort();
    ost::InetHostAddress getAddress();
    const char * getHostname();
    
};

class MessageSocketDuplex
{
    ost::UDPSocket * m_UDPReceiverSocket;
    ost::UDPSocket * m_UDPSenderSocket;	
   
    ost::InetAddress * m_receiverAddr;
    Port m_receiverPort;
    
    public:
    MessageSocketDuplex();                                     // set the listener port to unbound.
    MessageSocketDuplex(ost::InetAddress & Address, Port port);     // set the bound address and port.
    MessageSocketDuplex(Port port);                            // set the bound port.
    
    int sendto(NetworkMessage * message, ost::InetHostAddress * remoteAddress, Port * remotePort);   
    int sendto(NetworkMessage * message, NetworkNode * node);   
    
    int recvfrom(NetworkMessage ** message);
 
    ost::InetAddress * getReciverAddress() { return m_receiverAddr; }
    Port getReceiverPort() { return m_receiverPort; }

};


class NetworkMessenger
{
   private: 
      MessageSocketDuplex * m_messageSocketDuplex;
      std::list<NetworkMessage*> m_messageList;
      NetworkNode * m_orginatorNode;
      std::list<NetworkMessage*> m_messagePool;
      
   public:
      
    NetworkMessenger();
    NetworkMessenger(NetworkNode * orginatorNode);

    void queueMessage(NetworkNode * remoteNode, NetworkMessage * message);
    void sendMessages();
    void receiveMessages();

    NetworkNode * getOrginatorNode();
    void setOrginatorNode(NetworkNode * orginatorNode);

    NetworkMessage * getMessageFromPool(int type, int payloadLen);
    void returnMessageToPool(NetworkMessage * message);
    
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

