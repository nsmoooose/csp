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

#ifndef __SIMNET_NETWORKINTERFACE_H__
#define __SIMNET_NETWORKINTERFACE_H__

#include <SimData/ScopedPointer.h>
#include <SimData/Ref.h>

#include <SimNet/NetBase.h>
#include <SimNet/HandlerSet.h>

#include <map>


namespace simnet {

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


class NetworkInterface: public simdata::Referenced {
	friend class ActivePeerList;

	// reserved message ids
	static const simdata::uint16 PingID = 65535;

	// max number of peers
	static const simdata::uint32 PeerIndexSize = 4096;

	// this is a somewhat arbitrary limit, which probably should be configurable.
	static const simdata::uint32 MaxPayloadLength = 1024;

	// shorthand notation
	static const simdata::uint32 HeaderSize;
	static const simdata::uint32 ReceiptHeaderSize;

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

	// A simple lookup table from peer id to PeerInfo.
	simdata::ScopedArray<PeerInfo> m_PeerIndex;

	// A list containing only active peers, which also tracks the total bandwidth
	// that all peers would like to send to this host in the absence of throttling
	// and other constraints.
	simdata::ScopedPointer<ActivePeerList> m_ActivePeers;

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

	// Our connection to the outside world
	simdata::ScopedPointer<DatagramReceiveSocket> m_Socket;

	simdata::ScopedPointer<NetworkNode> m_ServerNode;
	simdata::ScopedPointer<NetworkNode> m_LocalNode;

	// NetworkInterface supports multiple packet handlers, each of which
	// receives every inbound packet.  Typically only one packet handlers
	// is required.
	typedef HandlerSet<PacketHandler> PacketHandlerSet;
	simdata::ScopedPointer<PacketHandlerSet> m_PacketHandlers;

	// NetworkInterface supports only one packet source for outbound packets.
	simdata::Ref<PacketSource> m_PacketSource;

	// Stats for diagnostics and bandwidth throttling.
	simdata::uint32 m_OutputStalls;
	simdata::uint32 m_SentPackets;
	simdata::uint32 m_ReceivedPackets;
	simdata::uint32 m_BadPackets;
	simdata::uint32 m_DroppedPackets;
	simdata::uint32 m_ThrottledPackets;

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
	 *  @param id The peer id (must not active)
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

	/** Get the peer info for a given peer id.
	 */
	PeerInfo *getPeer(PeerId id);

	// used by ActivePeerList  TODO document
	bool pingPeer(PeerInfo *peer);
	bool handleDeadPeer(PeerInfo *peer);
	void resend(simdata::Ref<ReliablePacket> &packet);

public:

    // Id used by clients prior to connecting to the server.
	static const PeerId InitialClientId = 0;

	// Id used by the server.
	static const PeerId ServerId = 1;

	typedef simdata::Ref<NetworkInterface> Ref;

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
     */
	void initialize(NetworkNode const &local_node, bool isServer);

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

	/** Called by a server to disconnect from a client.  This is a local
	 *  change only.  The caller is responsible for notifying the client
	 *  if appropriate.
	 *
	 *  @param id The id of the client to disconnect.
	 */
	void disconnectClient(PeerId id);

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
	void addPacketHandler(simdata::Ref<PacketHandler> const &handler);

	/** Remove a packet handler.
	 */
	void removePacketHandler(simdata::Ref<PacketHandler> const &handler);

	/** Set the source for all outbound packets.
	 */
	void setPacketSource(simdata::Ref<PacketSource> const &source);

};

} // namespace simnet

#endif // __SIMNET_NETWORKINTERFACE_H__
