#pragma once
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
 * @file NetworkInterface.h
 *
 */

#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/util/Ref.h>

#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/HandlerSet.h>

#include <map>
#include <deque>


namespace csp {

// forward declarations
class NetworkNode;
class DatagramReceiveSocket;
class DatagramTransmitSocket;
class ReliablePacket;
class PacketHandler;
class PacketQueue;
class PacketSource;
class PeerInfo;
class ActivePeerList;


/** Low level interface for sending and receiving packets over UDP.
 *
 *  This class, together with supporting classes, provides both reliable and
 *  unreliable message transport over UDP.  Reliable packets are resent until
 *  confirmation is received (confirmation ids ride piggyback on normal traffic
 *  and/or ping messages to conserve bandwidth).  Duplicate reliable packets
 *  are filtered internally, but delivery order is currently _not_ guaranteed.
 *
 *  Messages are segregated into four different priority classes, based on
 *  importance and longevity.  Each priority class uses dedicated inbound and
 *  outbound queues.  Unreliable outbound packets may be throttled based on
 *  priority and the dynamic bandwidth allocation set by each remote host.
 *
 *  Network interface is largely focused on peer-peer communication, but does
 *  provide minimal support for a client-server model.  The primary distinction
 *  between a server and a client NetworkInterface is that the former accepts
 *  provisional connections from unknown remote hosts (i.e. NetworkInterfaces
 *  using the default id of zero).  This allows a client to connect directly to
 *  the server, which assigns a unique id to the client and can negotiate peer
 *  to peer contacts as needed.  Note that there can be only one server on a
 *  network (the server id is always one).
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT NetworkInterface: public Referenced {
	friend class ActivePeerList;

	// reserved message ids
	static const uint16 PingID = 65535;

	// max number of peers
	static const uint32 PeerIndexSize = 4096;

	// this is a somewhat arbitrary limit, which probably should be configurable.
	static const uint32 MaxPayloadLength = 1024;

	// shorthand notation
	static const uint32 HeaderSize;
	static const uint32 ReceiptHeaderSize;

	// The receive and transmit queues for raw packets.  The four queues
	// are (roughly speaking):
	//
	//   @li 0 = non-realtime, low-priority, unreliable
	//   @li 1 = realtime, low-priority, unreliable
	//   @li 2 = realtime, unreliable
	//   @li 3 = high-priority, reliable
	//
	// The difference between realtime and non-realtime messages is the maximum
	// age of a packet (in the event of a backlog) before it is discarded.  For
	// realtime queues this threshold is shorter.  Unreliable packets are send
	// and pray, where as reliable packets require a confirmation from the
	// receiver and will be resent periodically until that confirmation arrives.
	PacketQueue *m_RxQueues[4];
	PacketQueue *m_TxQueues[4];

	// The peer id of this host/client/server.
	PeerId m_LocalId;

	// The last peer id assigned when establishing a connection.
	PeerId m_LastAssignedPeerId;

	// A simple lookup table from peer id to PeerInfo.
	ScopedArray<PeerInfo> m_PeerIndex;

	// A list containing only active peers, which also tracks the total bandwidth
	// that all peers would like to send to this host in the absence of throttling
	// and other constraints.
	ScopedPointer<ActivePeerList> m_ActivePeers;

	// A reverse index from ip address / port to peer id.
	typedef std::map<ConnectionPoint, PeerId> IpPeerMap;
	IpPeerMap m_IpPeerMap;

	/** Look up the peer id for a given ip/port.
	 *
	 *  @param point The ip address and port number
	 *  @return The peer id, or zero on failure.
	 */
	PeerId getSourceId(ConnectionPoint const &point);

	// Flag used by servers to indicate that connections from unknown
	// peers (clients) should be accepted.
	bool m_AllowUnknownPeers;

	// Indicates whether initialize() has been called.
	bool m_Initialized;

	// Counter indicating whether timing data should be discarded because
	// processIncoming has not been called recently.  Timing data should
	// only be considered valid if m_DiscardTiming is zero.
	int m_DiscardTiming;

	// Nominal bandwidths in bytes/sec.
	int m_IncomingBandwidth;
	int m_OutgoingBandwidth;

	// Our connection to the outside world
	ScopedPointer<DatagramReceiveSocket> m_Socket;

	ScopedPointer<NetworkNode> m_ServerNode;
	ScopedPointer<NetworkNode> m_LocalNode;
	ScopedPointer<NetworkNode> m_ExternalNode;

	// NetworkInterface supports multiple packet handlers, each of which
	// receives every inbound packet.  Typically only one packet handlers
	// is required.
	typedef HandlerSet<PacketHandler> PacketHandlerSet;
	ScopedPointer<PacketHandlerSet> m_PacketHandlers;

	// NetworkInterface supports only one packet source for outbound packets.
	Ref<PacketSource> m_PacketSource;

	// Stats for diagnostics and bandwidth throttling.
	uint32 m_OutputStalls;
	uint32 m_SentPackets;
	uint32 m_ReceivedPackets;
	uint32 m_BadPackets;
	uint32 m_DuplicatePackets;
	uint32 m_DroppedPackets;
	uint32 m_ThrottledPackets;

	// tracks time between calls to processIncoming (used to queue reliable
	// packet retransmissions).
	double m_LastUpdate;

	/** Internal method for shuttling data to a socket.
	 *  @param timeout The maximum time to spend sending packets (seconds).
	 */
	void sendPackets(double timeout);

	/** Internal method for shuttling data from a socket.
	 *  @param timeout The maximum time to spend receiving packets (seconds).
	 */
	int receivePackets(double timeout);

	/** Add a peer to the set of active peers.
	 *  @param id The peer id (must not be active)
	 *  @param remote_node The ip address and port of the peer
	 *  @param provisional Set true to indicate an initial (as yet incomplete) connection.
	 *  @param incoming_bw Peer incoming bandwidth (bytes per second)
	 *  @param outgoing_bw Peer outgoing bandwidth (bytes per second)
	 */
	void addPeer(PeerId id, NetworkNode const &remote_node, bool provisional, double incoming_bw, double outgoing_bw);

	/** Remove a peer from the set of active peers.
	 *  @param id The peer id to remove (must be active).
	 */
	void removePeer(PeerId id);

	/** Get the (non-const) peer info for a given peer id.
	 */
	PeerInfo *getPeer(PeerId id);

	/** Send a ping (heartbeat) packet to the specified peer.  Pings are sent at
	 *  a very low rate during normal communications, and slightly faster when
	 *  no other traffic is occuring.  These messages serve two purposes.  First
	 *  they act as a heartbeat, as the server will drop a peer if it receives
	 *  no packets for a while.  Second, they can carry receipts for reliable
	 *  messages sent from A to B, even when no regular messages are being sent
	 *  from B to A.  Note that pings will be sent more aggressively Whenever
	 *  there is a backlog of pending receipts.
	 *
	 *  This method is called by ActivePeerList.
	 */
	bool pingPeer(PeerInfo *peer);

	/** Called by ActivePeerList when no packets have been received from a peer
	 *  for a period of time (currently 30 sec).  If handleDeadPeer returns
	 *  true, the peer is dropped.  This is the default implementation.
	 *
	 *  Note that if this method returns false without resetting the timeout
	 *  (e.g. by calling peer->resetDeadTime()), it will be called at each
	 *  subsequent update until a packet is received.
	 *
	 *  @param peer The peer that is "dead".
	 *  @return true to drop the peer, false to ignore the timeout.
	 */
	bool handleDeadPeer(PeerInfo *peer);

	/** Add a peer to the disconnected peer queue, accessible via nextDisconnectedPeerId().
	 */
	void notifyPeerDisconnect(PeerId id);

	// Queue of dead peers.  See nextDisconnectedPeerId().
	std::deque<PeerId> m_DisconnectedPeerQueue;

	/** Called by ActivePeerList to resend a reliable packet that has not been
	 *  confirmed by the destination host.
	 */
	void resend(Ref<ReliablePacket> &packet);

public:

    // Id used by clients prior to connecting to the server.
	static const PeerId InitialClientId = 0;

	// Id used by the server.
	static const PeerId ServerId = 1;

	/** Construct a new NetworkInterface.  The interface must be initialized
	 *  and configured before use.
	 */
	NetworkInterface();

	~NetworkInterface();

	/** Initialize the interface.
	 *  @param local_node The ip address and port to bind to for inbound and
	 *    outbound communications.
	 *  @param isServer If true, the network interface will be configured to
	 *    accept connections from unknown peers.  Otherwise the interface will
	 *    be initialized with id = 0, and will need to connect to a server to
	 *    receive an id assignment before communicating with other peers.
	 *  @param incoming_bw The server incoming bandwidth (bytes per second)
	 *  @param outgoing_bw The server outgoing bandwidth (bytes per second)
     */
	void initialize(NetworkNode const &local_node, bool isServer, int incoming_bw, int outgoing_bw);

	/** Specify an alternate ip address and port that should be used to reach
	 *  this host from an external network.  For example, local_node specified
	 *  in initialize() may refer to an unroutable LAN address (e.g. 192.168.x.x)
	 *  while external_node refers to the ip address of the external interface
	 *  of a firewall/router providing NAT service and port forwarding for the
	 *  LAN.  The external ip address does not directly affect the behavior of
	 *  NetworkInterface, but can be used by higher level protocols to help
	 *  manage LAN/internet routing.  See getExternalNode().
	 */
	void setExternalNode(NetworkNode const &external_node);

	/** Get the external node specified by setExternalNode(), or the local node
	 *  if no external node was set.
	 */
	const NetworkNode &getExternalNode() const;

	/** Get the local node specified by initialize() used to receive packets.
	 */
	const NetworkNode &getLocalNode() const;

	/** Reset diagnostic statistics.  Does not affect bandwidth throttling
	 *  statistics.
     */
	void resetStats();

	/** Set the server node.  This method can only be called after initialization,
	 *  and currently should not be called more than once.
	 *
     *  TODO either prevent multiple calls or properly clean up state.
	 *
	 *  @param server_node The ip address and port of the central server.
	 *  @param incoming_bw The server incoming bandwidth (bytes per second)
	 *  @param outgoing_bw The server outgoing bandwidth (bytes per second)
	 */
	void setServer(NetworkNode const &server_node, double incoming_bw, double outgoing_bw);

	/** Called by a server to establish a permanent connection with a new
	 *  client.
	 *
	 *  @param id The id of the client.
     *  @param incoming_bw The client incoming bandwidth (bytes per second)
	 *  @param outgoing_bw The client outgoing bandwidth (bytes per second)
	 */
	void establishConnection(PeerId id, double incoming_bw, double outgoing_bw);

	/** Add a peer to the set of active peers.  This method can be used to register
	 *  peers without a direct handshake, as for example when a central server
	 *  introduces two peers.
	 *
	 *  @param id The peer id (must not be active)
	 *  @param remote_node The ip address and port of the peer
	 *  @param incoming_bw Peer incoming bandwidth (bytes per second)
	 *  @param outgoing_bw Peer outgoing bandwidth (bytes per second)
	 */
	void addPeer(PeerId id, NetworkNode const &remote_node, double incoming_bw, double outgoing_bw);

	/** Called by a server to disconnect from a client, or by a client to
	 *  disconnect from a peer.  This is a local change only.  The caller
	 *  is responsible for notifying the peer if appropriate.
	 *
	 *  @param id The id of the peer to disconnect.
	 */
	void disconnectPeer(PeerId id);

	/** Used by servers to indicate that connections from unknown peers will be
	 *  accepted.
	 */
	void setAllowUnknownPeers(bool allow);

	/** Called by the client to set the id assigned by the server.
	 *
	 *  @param id The new id of this client.
	 */
	void setClientId(PeerId id);

	/** Process incoming packets.  This method spends up to 60% of its alloted timeout
	 *  reading packets from the socket.  The balance of the time is spend decoding the
	 *  raw packets into messages, and dispatching those messages to the appropriate
	 *  handlers.  Processing stops when either all queued packets have been processed
	 *  or the timeout expires.  The timeout can be exceeded if any message handlers
	 *  are slow to return (i.e. dispatch blocks the message processing loop).
	 *
	 *  @param timeout The maximum time (in seconds) to spend processing inbound packets.
	 */
	void processIncoming(double timeout);

	/** Process outgoing packets.  This method spends up to 50% of its alloted timeout
	 *  converting outgoing messages into raw packets.  The balance of the time is spent
	 *  writing the packets to the socket.  Processing stops when either all the queued
	 *  packets have been sent or the timeout expires.
	 *
	 *  @param timeout The maximum time (in seconds) to spend processing inbound packets.
	 */
	void processOutgoing(double timeout);

	/** Wait for incoming packets if all incoming and outgoing queues are empty.
	 *  @param timeout the maximum time to wait, in seconds.
	 *  @return false if the timeout expired (ie. no pending incoming packets and all queues empty).
	 */
	bool waitPending(double timeout);

	/** Add a handler that will receive all inbound packets.  Any number of handlers
	 *  can be added, although in practice one is usually sufficient.
	 */
	void addPacketHandler(Ref<PacketHandler> const &handler);

	/** Remove a packet handler.
	 */
	void removePacketHandler(Ref<PacketHandler> const &handler);

	/** Set the source for all outbound packets.
	 */
	void setPacketSource(Ref<PacketSource> const &source);

	/** Get the peer info for a given peer id.
	 */
	PeerInfo const *getPeer(PeerId id) const;

	/** Returns true if one or more peers have disconnected, but have
	 *  not yet been processed by nextDisconnectedPeer.
	 */
	inline bool hasDisconnectedPeers() const { return !m_DisconnectedPeerQueue.empty(); }

	/** Get the id of the next peer that has disconnected, or zero if
	 *  no disconnection notifications are pending.  This routine can
	 *  be used to cleanup after peers disconnect, but it should be
	 *  called frequently so that reassigned ids aren't mistaken as
	 *  recently disconnected.
	 */
	PeerId nextDisconnectedPeerId();

	/** Get the nominal (max) incoming bandwidth, in bytes per second.  This is
	 *  the value specified when the network interface was initialized.
	 */
	inline int getNominalIncomingBandwidth() const { return m_IncomingBandwidth; }

	/** Get the nominal (max) ougoing bandwidth, in bytes per second.  This is
	 *  the value specified when the network interface was initialized.
	 */
	inline int getNominalOutgoingBandwidth() const { return m_OutgoingBandwidth; }
};

} // namespace csp
