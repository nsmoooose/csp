// Combat Simulator Project
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
 * @file ClientServer.h
 *
 */

#ifndef __SIMNET_CLIENTSERVER_H__
#define __SIMNET_CLIENTSERVER_H__


#include <SimNet/NetBase.h>

// TODO forward declare
#include <SimNet/ClientServerMessages.h>
#include <SimNet/DispatchHandler.h>
#include <SimNet/DispatchManager.h>
#include <SimNet/MessageHandler.h>
#include <SimNet/MessageQueue.h>
#include <SimNet/NetworkInterface.h>
#include <SimNet/NetLog.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/PacketDecoder.h>
#include <SimNet/RoutingHandler.h>

#include <SimData/Ref.h>
#include <SimData/Timing.h>


namespace simnet {


class ClientServerBase: public simdata::Referenced {
	virtual void onConnectionRequest(simdata::Ref<ConnectionRequest> const &, simdata::Ref<MessageQueue> const &) {}
	virtual void onConnectionResponse(simdata::Ref<ConnectionResponse> const &, simdata::Ref<MessageQueue> const &) {}
	virtual void onAcknowledge(simdata::Ref<Acknowledge> const &, simdata::Ref<MessageQueue> const &) {}
	virtual void onDisconnect(simdata::Ref<Disconnect> const &, simdata::Ref<MessageQueue> const &) {}
	virtual void onUnknownMessage(simdata::Ref<NetworkMessage> const &, simdata::Ref<MessageQueue> const &) {}

	NetworkNode m_LocalNode;

protected:
	ClientServerBase(NetworkNode const &bind, bool isServer, int inbound_bw, int outbound_bw);
	virtual ~ClientServerBase() {}

	simdata::Ref<NetworkInterface> m_NetworkInterface;
	simdata::Ref<MessageQueue> m_MessageQueue;
	simdata::Ref<PacketDecoder> m_PacketDecoder;
	simdata::Ref<RoutingHandler> m_RoutingHandler;
	simdata::Ref<DispatchHandler> m_HandshakeDispatch;
	simdata::Ref<DispatchManager> m_DispatchManager;

public:

	inline PacketDecoder::Ref decoder() { return m_PacketDecoder; }
	inline RoutingHandler::Ref router() { return m_RoutingHandler; }
	inline MessageQueue::Ref queue() { return m_MessageQueue; }
	inline DispatchManager::Ref dispatch_manager() { return m_DispatchManager; }

	void processIncoming(double timeout);
	void processOutgoing(double timeout);
	void processTraffic(double read_timeout, double write_timeout);
	void processAndWait(double read_timeout, double write_timeout, double timeout);
	void routeToHandler(RoutingType routing_type, simdata::Ref<MessageHandler> const &handler);

	// convenience method for adding callback handlers
	template <class CLASS>
	void routeToCallback(RoutingType routing_type, CLASS *instance, void (CLASS::*method)(NetworkMessage::Ref const &)) {
		routeToHandler(routing_type, MessageHandler::Callback(instance, method));
	}

	void addDispatchManager(int cache_size);
	bool dispatch(DispatchTarget *target, NetworkMessage::Ref const &msg);
	void queueMessage(NetworkMessage::Ref const &msg);

	NetworkNode const &getLocalNode() const { return m_LocalNode; }
};

class Server: public ClientServerBase {

	virtual void onConnectionRequest(simdata::Ref<ConnectionRequest> const &msg, simdata::Ref<MessageQueue> const &queue);
	virtual void onAcknowledge(simdata::Ref<Acknowledge> const &msg, simdata::Ref<MessageQueue> const &queue);
	virtual void onDisconnect(simdata::Ref<Disconnect> const &msg, simdata::Ref<MessageQueue> const &queue);

public:
	Server(NetworkNode const &bind, int inbound_bw, int outbound_bw);
};

class Client: public ClientServerBase {
	bool m_Connected;
	virtual void onConnectionResponse(simdata::Ref<ConnectionResponse> const &msg, simdata::Ref<MessageQueue> const &queue);

public:
	Client(NetworkNode const &bind, int inbound_bw, int outbound_bw);
	~Client();
	bool connectToServer(NetworkNode const &server, double timeout);
	void disconnectFromServer(bool immediate=false);
	bool isConnected() const { return m_Connected; }
	void sendToServer(NetworkMessage::Ref const &msg);
};


} // namespace simnet

#endif // __SIMNET_CLIENTSERVER_H__
