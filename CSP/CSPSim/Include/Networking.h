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
#include <map>

#include <SimData/Vector3.h>
#include <SimData/String.h>
#include <SimData/Uniform.h>
#include <SimData/Quat.h>
#include <SimData/DataManager.h>
#include <SimData/Date.h>
#include <stdio.h>

#include "DynamicObject.h"
#include "VirtualBattlefield.h"

typedef int SockFd;
typedef simdata::uint16 Port;
class NetworkNode;

const short NETWORK_PACKET_SIZE = 512;

//#ifdef WIN32
#pragma pack(push, 1)
//#endif

                     
// Define some structs that will only used for data transfers.
// These differ from SimData structs in that they have no virtual
// functions and hence have more predictable binary sizes.
struct _Vector3Struct
{
  double x;
  double y;
  double z;
};

struct _QuatStruct
{
  double x;
  double y;
  double z;
  double w;
};



struct MessageHeader
{
  simdata::uint16  m_magicNumber;
  simdata::uint16  m_payloadLen;
  simdata::uint16  m_messageType;
  simdata::uint32  m_ipaddr;
  simdata::uint16  m_port;
  simdata::uint16  m_id;

  void dump()
  {
	  printf("MessageHeader - MagicNumber: %u\n", m_magicNumber);
	  printf("MessageHeader - PayloadLen: %u\n", m_payloadLen);
	  printf("MessageHeader - MessageType: %u\n", m_messageType);
	  printf("MessageHeader - ipaddr: %u\n", m_ipaddr);
	  printf("MessageHeader - port: %u\n", m_port);
	  printf("MessageHeader - id: %u\n", m_id);
  }	 

  void dumpOffsets()
  {
	simdata::uint32 thisAddr = (simdata::uint32)this;
	simdata::uint32 magicNumberAddr = (simdata::uint32)&m_magicNumber;
	simdata::uint32 PayloadLenAddr = (simdata::uint32)&m_payloadLen;
	simdata::uint32 MessageTypeAddr = (simdata::uint32)&m_messageType;
	simdata::uint32 ipaddrAddr = (simdata::uint32)&m_ipaddr;
	simdata::uint32 portAddr = (simdata::uint32)&m_port;
	simdata::uint32 idAddr = (simdata::uint32)&m_id;

	printf("MessageHeader - MagicNumberOffset: %d\n", magicNumberAddr - thisAddr );
	printf("MessageHeader - PayloadLenOffset: %d\n", PayloadLenAddr - thisAddr );
	printf("MessageHeader - MessageTypeOffset: %d\n", MessageTypeAddr - thisAddr );
	printf("MessageHeader - ipOffset: %d\n", ipaddrAddr - thisAddr );
	printf("MessageHeader - portOffset: %d\n", portAddr - thisAddr );
	printf("MessageHeader - idOffset: %d\n", idAddr - thisAddr );
  }
		  
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
	NetworkMessage(NetworkMessage &);

    public:
    
    bool initialize(simdata::uint16 type, simdata::uint16 payloadLength, NetworkNode * senderNode);
    void dumpMessageHeader();

    simdata::uint16 getType();

    void * getPayloadPtr();
    simdata::uint16 getPayloadLen();

    Port getOriginatorPort();
    NetworkNode * getOriginatorNode();
    
    bool isInitialized();
    bool isHeaderValid();
   
};

struct ObjectUpdateMessagePayload 
{

    private:
      ObjectUpdateMessagePayload();

    public:
      unsigned int id;
      unsigned int objectType;
      simdata::SimTime timeStamp;
      _Vector3Struct globalPosition;
      _Vector3Struct linearVelocity;
      _Vector3Struct angularVelocity;
      _QuatStruct attitude;
    
  void dumpOffsets()
  {
	simdata::uint32 thisAddr = (simdata::uint32)this;
	simdata::uint32 idAddr = (simdata::uint32)&id;
	simdata::uint32 typeAddr = (simdata::uint32)&objectType;
	simdata::uint32 timeStampAddr = (simdata::uint32)&timeStamp;
	simdata::uint32 globalPositionAddr = (simdata::uint32)&globalPosition;
	simdata::uint32 linearVelocityAddr = (simdata::uint32)&linearVelocity;
	simdata::uint32 angularVelocityAddr = (simdata::uint32)&angularVelocity;
	simdata::uint32 attitudeAddr = (simdata::uint32)&attitude;

	printf("ObjectUpdateMessagePayload - IDOffset: %d\n", idAddr - thisAddr );
	printf("ObjectUpdateMessagePayload - TypeOffset: %d\n", typeAddr - thisAddr);
	printf("ObjectUpdateMessagePayload - timeStampOffset: %d\n", timeStampAddr - thisAddr );
	printf("ObjectUpdateMessagePayload - globalPositionOffset: %d\n", globalPositionAddr - thisAddr );
	printf("ObjectUpdateMessagePayload - linearVelocityOffset: %d\n", linearVelocityAddr - thisAddr );
	printf("ObjectUpdateMessagePayload - angularVelocityOffset: %d\n", angularVelocityAddr - thisAddr );
	printf("ObjectUpdateMessagePayload - attitudeOffset: %d\n", attitudeAddr - thisAddr );
  }

  void dump()
  {
    printf("ObjectUpdateMessagePayload - id: %d\n", id);
    printf("ObjectUpdateMessagePayload - objectType: %d\n", objectType);
    printf("ObjectUpdateMessagePayload - timestamp: %f\n", timeStamp);
    printf("ObjectUpdateMessagePayload - GlobalPosition [x: %f, y: %f, z: %f]\n", globalPosition.x, globalPosition.y, globalPosition.z);

  }
    
};

//#ifdef WIN32
#pragma pack(pop)
//#endif


class NetworkMessagePool
{
//	private:
//	    static NetworkMessage * g_messagePool;
// 
//	public:
//	    NetworkMessage * getMessageFromPool(int type, int payloadLen);
//	    NetworkMessage * getMessageFromPool();
//	    void returnMessageToPool(NetworkMessage * message);
//	    static NetworkMessagePool * getPool();

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


struct RoutedMessage
{
	NetworkNode * m_destinationNode;
	NetworkMessage * m_message;
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

    int sendto(std::vector<RoutedMessage> * sendArray, int count);
    int recvfrom(std::vector<RoutedMessage> * receiveArray, int * count);
 
    ost::InetAddress * getReceiverAddress() { return m_receiverAddr; }
    Port getReceiverPort() { return m_receiverPort; }

};

class DynamicObject;

class RemoteObjectWrapper 
{
  public:
   RemoteObjectWrapper(simdata::Ref<DynamicObject> & object) { m_Object = object; }
   NetworkNode * getObjectHost() { return m_Host; }
   simdata::Ref<DynamicObject> getWrappedObject() { return m_Object; }
  private:
   simdata::Ref<DynamicObject> m_Object;
   NetworkNode   * m_Host;
	 

};

class RemoteObjectTable
{
  public:
  RemoteObjectWrapper * getRemoteObject(int ipaddr, short port, int id)
  {
    return m_table[ipaddr][port][id];
  }

  void putRemoteObject(int ipaddr, short port, int id, RemoteObjectWrapper * object)
  {
    m_table[ipaddr][port][id] = object;
  }

  private:
    std::map< int , std::map < short, std::map < short, RemoteObjectWrapper * > > > m_table;

};

class NetworkMessageHandler;

class NetworkMessenger
{
   private: 
//    MessageSocketDuplex * m_messageSocketDuplex;
    std::vector<RoutedMessage> m_messageSendArray;
    std::vector<RoutedMessage> m_messageReceiveArray;
    int m_messageSendArrayMax;
    int m_messageSendArrayCount;
    int m_messageSendArrayGrow;
    int m_messageReceiveArrayMax;
    int m_messageReceiveArrayCount;
    int m_messageReceiveArrayGrow;
    NetworkNode * m_originatorNode;
    std::list<NetworkMessage*> m_messagePool;
    std::list<NetworkMessageHandler *> m_ReceiveHandlerList;
      
    ost::UDPSocket * m_UDPReceiverSocket;
    ost::UDPSocket * m_UDPSenderSocket;	
   
    ost::InetAddress * m_receiverAddr;
    Port m_receiverPort;
   public:
      
    NetworkMessenger();
    NetworkMessenger(NetworkNode * orginatorNode);
//    NetworkMessenger(ost::InetAddress & addr, Port port);

    void queueMessage(NetworkNode * remoteNode, NetworkMessage * message);
    void sendQueuedMessages();
    void receiveMessages();

    NetworkNode * getOriginatorNode();
    void setOriginatorNode(NetworkNode * orginatorNode);

    NetworkMessage * allocMessageBuffer(int type, int payloadLen);
    NetworkMessage * allocMessageBuffer();
    void freeMessageBuffer(NetworkMessage * message);
    
    void registerReceiveHandler(NetworkMessageHandler * handler);
    
    int sendto(NetworkMessage * message, ost::InetHostAddress * remoteAddress, Port * remotePort);   
    int sendto(NetworkMessage * message, NetworkNode * node);   
    
    int recvfrom(NetworkMessage ** message);

    int sendto(std::vector<RoutedMessage> * sendArray, int count);
    int recvfrom(std::vector<RoutedMessage> * receiveArray, int * count);

    NetworkMessage * receiveMessage();
    
    ost::InetAddress * getReceiverAddress() { return m_receiverAddr; }
    Port getReceiverPort() { return m_receiverPort; }

};


class NetworkMessageHandler
{
	public:
	virtual void process(NetworkMessage * message, NetworkMessenger * messenger) = 0;
};

class EchoMessageHandler : public NetworkMessageHandler
{

	public: 
		EchoMessageHandler();
		virtual void process(NetworkMessage * message, NetworkMessenger * messenger);
		virtual ~EchoMessageHandler();
	
};

class PrintMessageHandler : public NetworkMessageHandler
{
	protected:
		int m_frequency;
		int m_count;
	public:
		PrintMessageHandler(); 
		virtual void process(NetworkMessage * message, NetworkMessenger * messenger);
		virtual ~PrintMessageHandler();

		void setFrequency(int frequency) { m_frequency = frequency; }
		int getFrequency() { return m_frequency; }
};

class DispatchMessageHandler : public NetworkMessageHandler
{
  public:
    DispatchMessageHandler();
    virtual void process(NetworkMessage * message, NetworkMessenger * messenger);
    virtual ~DispatchMessageHandler();
    void setLocalAddress(unsigned int addr) { _addr = addr; }
    void setLocalPort(unsigned short port) { _port = port; }
    void setVirtualBattlefield(VirtualBattlefield * battlefield) { _virtualBattlefield = battlefield; }
    void setDataManager(simdata::DataManager & dataManager) { _dataManager = dataManager; }
    
  protected:
    bool isLocal(unsigned int addr, unsigned short port);
    simdata::Ref<DynamicObject> addRemoteObject( NetworkMessage * message );
    
  private:
	  
    RemoteObjectTable m_RemoteObjectTable;
    unsigned int _addr;
    unsigned short _port; 
    VirtualBattlefield * _virtualBattlefield;
    simdata::DataManager _dataManager;

};

// not currently using these below

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

