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
 * Provides convenience wrappers around NetworkInterface for creating
 * client-server (and client-client) applications.
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


class PeerInfo;


/** Command base class for clients and servers.
 */
class ClientServerBase: public simdata::Referenced {

	// client-server message handlers.
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

	/** Accessor for the incoming packet decoder.
	 */
	inline PacketDecoder::Ref decoder() { return m_PacketDecoder; }

	/** Accessor for the incoming packet router.
	 */
	inline RoutingHandler::Ref router() { return m_RoutingHandler; }

	/** Accessor for the outgoing message queue.
	 */
	inline MessageQueue::Ref queue() { return m_MessageQueue; }

	/** Accessor for the incoming mesage dispath manager.
	 */
	inline DispatchManager::Ref dispatch_manager() { return m_DispatchManager; }

	/** Get the peer info for the given peer id, or 0 if the id is not known.
	 */
	PeerInfo const *getPeer(PeerId id) const;

	/** Add a peer to the internal list of active peers.  The client-server connection
	 *  handshake establishes the client id, but this method also allows clients to
	 *  recognize peers as introduced by the server without a formal handshake.
	 *
	 *  @param id The id of the peer to add.
	 *  @param remote_node The network node of the peer to add.
	 *  @param incoming_bw The maximum incoming bandwidth (bytes/sec) of the peer.
	 *  @param outgoing_bw The maximum outgoing bandwidth (bytes/sec) of the peer.
	 */
	void addPeer(PeerId id, NetworkNode const &remote_node, double incoming_bw, double outgoing_bw);

	/** Process incoming packets (see NetworkInterface::processIncoming).
	 */
	void processIncoming(double timeout);

	/** Process outgoing packets (see NetworkInterface::processOutgoing).
	 */
	void processOutgoing(double timeout);

	/** Convenience method to process incoming and outgoing packets at once.
	 *
	 *  @param read_timeout The maximum time (in seconds) to spend processing incoming packets.
	 *  @param write_timeout The maximum time (in seconds) to spend processing outgoing packets.
	 */
	void processTraffic(double read_timeout, double write_timeout);

	/** Convenience method to process incoming and outgoing packets at once.
	 *
	 *  @param read_timeout The maximum time (in seconds) to spend processing incoming packets.
	 *  @param write_timeout The maximum time (in seconds) to spend processing outgoing packets.
	 *  @param timeout The maximum time (in seconds) to wait for incoming or outgoing packets
	 *    before processing.
	 */
	void processAndWait(double read_timeout, double write_timeout, double timeout);

	/** Convenience method for adding message handlers based on routing type.
	 */
	void routeToHandler(RoutingType routing_type, simdata::Ref<MessageHandler> const &handler);

	/** Convenience method for adding callback methods to handle messages based on routing type.
	 */
	template <class CLASS>
	void routeToCallback(RoutingType routing_type, CLASS *instance, void (CLASS::*method)(NetworkMessage::Ref const &)) {
		routeToHandler(routing_type, MessageHandler::Callback(instance, method));
	}

	/** Add a dispatch manager to the network interface.
	 *
	 *  @param cache_size The maximum number of entries in the MRU dispatch cache (see DispatchManager).
	 */
	void addDispatchManager(int cache_size);

	/** Dispatch a message to the specified target.  This method should only be called if a dispatch
	 *  manager has been added using addDispatchManager.
	 *
	 *  @param target The disptach target to receive the message.
	 *  @param msg The message being sent to the target.
	 *  @return true If the message was handled by the target.
	 */
	bool dispatch(DispatchTarget *target, NetworkMessage::Ref const &msg);

	/** Queue a message for transmission.
	 */
	void queueMessage(NetworkMessage::Ref const &msg);

	/** Get the network node of this client or server.
	 */
	NetworkNode const &getLocalNode() const { return m_LocalNode; }

	/** Returns true if one or more peers have disconnected, but have
	 *  not yet been processed by nextDeadPeer.
	 */
	inline bool hasDisconnectedPeers() const { return m_NetworkInterface->hasDisconnectedPeers(); };

	/** Get the id of the next peer that has disconnected, or zero if
	 *  no disconnection notifications are pending.  This routine can
	 *  be used to cleanup after peers disconnect, but it should be
	 *  called frequently so that reassigned ids aren't mistaken as
	 *  recently disconnected.
	 */
	PeerId nextDisconnectedPeerId() { return m_NetworkInterface->nextDisconnectedPeerId(); }
};


/** Convenience class for implementing network servers using SimNet.  There can be
 *  at most on server in a set of connected hosts.  Each client connects to the
 *  server, and the server may then introduce clients for direct peer-peer
 *  communication.  Servers use a fixed peer id (defined by NetworkInterface::ServerId),
 *  and are responsible for assigning unique ids to all clients.
 */
class Server: public ClientServerBase {

	// Callbacks used during the initial client-server connection handshake.
	virtual void onConnectionRequest(simdata::Ref<ConnectionRequest> const &msg, simdata::Ref<MessageQueue> const &queue);
	virtual void onAcknowledge(simdata::Ref<Acknowledge> const &msg, simdata::Ref<MessageQueue> const &queue);
	// Callback when a client sends a disconnect message.
	virtual void onDisconnect(simdata::Ref<Disconnect> const &msg, simdata::Ref<MessageQueue> const &queue);

public:
	/** Construct a new server.
	 *
	 *  @param bind The network address (interface) and port to use for inbound traffic (udp).
	 *  @param inbound_bw The maximum inbound bandwidth for this server (bytes/sec).
	 *  @param outbound_bw The maximum outbound bandwidth for this server (bytes/sec).
	 */
	Server(NetworkNode const &bind, int inbound_bw, int outbound_bw);
};


/** Convenience class for implementing network clients using SimNet.
 */
class Client: public ClientServerBase {
	bool m_Connected;

	// Callback used during the initial client-server connection handshake.
	virtual void onConnectionResponse(simdata::Ref<ConnectionResponse> const &msg, simdata::Ref<MessageQueue> const &queue);

public:
	/** Construct a new client.
	 *
	 *  @param bind The network address (interface) and port to use for inbound traffic (udp).
	 *  @param inbound_bw The maximum inbound bandwidth for this client (bytes/sec).
	 *  @param outbound_bw The maximum outbound bandwidth for this client (bytes/sec).
	 */
	Client(NetworkNode const &bind, int inbound_bw, int outbound_bw);

	virtual ~Client();

	/** Connect to a server.
	 *
	 *  Each client begins life with peer id 0, and must connect to a server before any other
	 *  remote communications can occur.  Once successfully connected, the server will assign
	 *  a permanent id that is unique among all clients connected to the server.
	 *
	 *  @param server The network node (ip address and port) of the server.
	 *  @param timeout The maximum time (in seconds) allow for a connection to be established.
	 */
	bool connectToServer(NetworkNode const &server, double timeout);

	/** Send a disconnect message to the server, and set the client state to disconnected.
	 *
	 *  @param immediate If fales, the disconnect message is queued as a reliable
	 *    packet.  If true, two disconnect message are sent immediately via using
	 *    normal (unreliable) transport.  The latter should only be used if it is
	 *    truly necessary to shut down the connection in a hurry, as there is no
	 *    guarantee that the server will receive the message (although the server
	 *    will eventually detect the disconnect on its own).
	 */
	void disconnectFromServer(bool immediate=false);

	/** Returns true if connectToServer has completed successfully (and disconnectFromServer
	 *  has not been called).
	 */
	bool isConnected() const { return m_Connected; }

	/** Sends a message to the server (if connected).
	 */
	void sendToServer(NetworkMessage::Ref const &msg);

	/** Drop a peer connection.  Further packets from this peer will be disregarded.
	 *  The peer id will NOT be added to the list of disconnected peers accessible
	 *  by hasDisconnectedPeers() and nextDisconnectedPeer().
	 */
	void disconnectPeer(PeerId id);
};


} // namespace simnet

#endif // __SIMNET_CLIENTSERVER_H__
