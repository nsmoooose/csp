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
#ifdef _WIN32
#define _WINSOCKAPI_
#define NOMINMAX
#endif

#include <cc++/network.h>


#include <sys/types.h>
#include <list>
#include <map>
#include <set>

#include <SimData/Vector3.h>
#include <SimData/String.h>
#include <SimData/Uniform.h>
#include <SimData/Quat.h>
#include <SimData/DataManager.h>
#include <SimData/Date.h>
#include <stdio.h>

#include "Log.h"
#include "DynamicObject.h"
#include <SimNet/NetworkMessageHandler.h>

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

	void dumpOffsets() {
		simdata::uint32 thisAddr = (simdata::uint32)this;
		simdata::uint32 idAddr = (simdata::uint32)&id;
		simdata::uint32 typeAddr = (simdata::uint32)&objectType;
		simdata::uint32 timeStampAddr = (simdata::uint32)&timeStamp;
		simdata::uint32 globalPositionAddr = (simdata::uint32)&globalPosition;
		simdata::uint32 linearVelocityAddr = (simdata::uint32)&linearVelocity;
		simdata::uint32 angularVelocityAddr = (simdata::uint32)&angularVelocity;
		simdata::uint32 attitudeAddr = (simdata::uint32)&attitude;

		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - IDOffset: " << (idAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - TypeOffset: " << (typeAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - timeStampOffset: " << (timeStampAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - globalPositionOffset: " << (globalPositionAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - linearVelocityOffset: " << (linearVelocityAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - angularVelocityOffset: " << (angularVelocityAddr - thisAddr));
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - attitudeOffset: " << (attitudeAddr - thisAddr));
	}

	void dump() {
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - id: " << id);
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - objectType: " << objectType);
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - timestamp: " << timeStamp);
		CSP_LOG(NETWORK, DEBUG, "ObjectUpdateMessagePayload - GlobalPosition: [" << globalPosition.x << ", " << globalPosition.y << ", " << globalPosition.z << "]");
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




struct RoutedMessage
{
	NetworkNode * m_destinationNode;
	NetworkMessage * m_message;
};


class DynamicObject;

//class RemoteObjectWrapper
//{
//  public:
//   RemoteObjectWrapper(simdata::Ref<DynamicObject> & object) { m_Object = object; }
//   NetworkNode * getObjectHost() { return m_Host; }
//   simdata::Ref<DynamicObject> getWrappedObject() { return m_Object; }
//  private:
//   simdata::Ref<DynamicObject> m_Object;
//   NetworkNode   * m_Host;
//
//
//};

class RemoteObjectKey
{
protected:
	int m_ipaddr;
	short m_port;
	int m_id;

public:
	RemoteObjectKey() {
		m_ipaddr = 0;
		m_port = 0;
		m_id = 0;
	}

	RemoteObjectKey(const RemoteObjectKey & key) {
		m_ipaddr = key.m_ipaddr;
		m_port = key.m_port;
		m_id = key.m_id;
	}

	RemoteObjectKey(int ipaddr, short port, int id) {
		m_ipaddr = ipaddr;
		m_port = port;
		m_id = id;
	}

#ifndef SWIG
	const RemoteObjectKey & operator=(const RemoteObjectKey & key) {
		m_ipaddr = key.m_ipaddr; m_port = key.m_port; m_id = key.m_id;
		return *this;
	}
#endif // SWIG

	bool operator==(const RemoteObjectKey & key) {
		return ( (m_ipaddr == key.m_ipaddr) && (m_port == key.m_port) && (m_id == key.m_id) );
	}

	bool operator!=(const RemoteObjectKey & key) {
		return ( (m_ipaddr != key.m_ipaddr) || (m_port != key.m_port) || (m_id == key.m_id) );
	}

#ifndef SWIG
	friend bool operator<( const RemoteObjectKey & lhs, const RemoteObjectKey & rhs);
#endif // SWIG

//    bool operator>( RemoteObjectKey & key)
//      {
//	if ( m_ipaddr != key.m_ipaddr )
//	  return ( m_ipaddr > key.m_ipaddr);
//	if ( m_port != key.m_port )
//	  return ( m_port > key.m_port );
//	return ( m_id > key.m_id );
//      }
//    bool operator<=( RemoteObjectKey & key)
//      {
//	if ( m_ipaddr != key.m_ipaddr )
//	  return ( m_ipaddr <= key.m_ipaddr);
//	if ( m_port != key.m_port )
//	  return ( m_port <= key.m_port );
//	return ( m_id <= key.m_id );
//     }
//    bool operator>=( RemoteObjectKey & key)
//      {
//	if ( m_ipaddr != key.m_ipaddr )
//	  return ( m_ipaddr >= key.m_ipaddr);
//	if ( m_port != key.m_port )
//	  return ( m_port >= key.m_port );
//	return ( m_id >= key.m_id );
//      }

};

#ifndef SWIG
bool operator<( const RemoteObjectKey & lhs, const RemoteObjectKey & rhs);
#endif // SWIG

class RemoteObjectTable
{
public:
	simdata::Ref<DynamicObject> getRemoteObject(RemoteObjectKey key) {
		CSP_LOG(NETWORK, TRACE, "RemoteObjectTable::getRemoteObject()");
		return m_table[key];
	}

	void putRemoteObject(RemoteObjectKey key, simdata::Ref<DynamicObject> object) {
		CSP_LOG(NETWORK, TRACE, "RemoteObjectTable::putRemoteObject() - entering");
		m_table[key] = object;
		CSP_LOG(NETWORK, TRACE, "RemoteObjectTable::putRemoteObject() - exiting");
	}

private:
	std::map< RemoteObjectKey, simdata::Ref<DynamicObject> > m_table;

};

class NetworkMessageHandler;

class CallHandler
{
private:
	NetworkMessage* m_NetworkMessage;
public:
	CallHandler(NetworkMessage* network_message): m_NetworkMessage(network_message) {  }
	void operator()(NetworkMessageHandler* network_message_handler) {
		network_message_handler->process(m_NetworkMessage);
	}
};


//        NetworkMessage * message = (NetworkMessage*)(new simdata::uint8[512]);
#endif

