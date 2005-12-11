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
 * @file PeerInfo.h
 *
 */

#ifndef __CSPLIB_NET_PEERINFO_H__
#define __CSPLIB_NET_PEERINFO_H__


#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/NetRandom.h>
#include <csp/csplib/net/NetworkNode.h>
#include <csp/csplib/net/ReliablePacket.h>
#include <csp/csplib/net/Sockets.h>
#include <csp/csplib/net/NetLog.h>
#include <csp/csplib/net/Median.h>

#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/ScopedPointer.h>

#include <map>
#include <queue>
#include <vector>
#include <deque>


CSP_NAMESPACE

class NetworkInterface;


/** PeerInfo encapsulates the state of a remote connection.  It records
 *  connection statistics, manages bandwidth and packet throttling,
 *  and tracks reliable packet transmission and receipts.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT PeerInfo: public NonCopyable {
	friend class ActivePeerList;

	PeerId m_id;
	bool m_active;
	bool m_provisional;
	double m_lifetime;
	NetworkNode m_node;

	typedef std::map<ConfirmationId, Ref<ReliablePacket> > ReliablePacketMap;
	typedef std::vector<Ref<ReliablePacket> > ReliablePacketVector;
	typedef std::priority_queue<Ref<ReliablePacket>, ReliablePacketVector, ReliablePacket::Order> ReliablePacketQueue;
	ReliablePacketMap m_reliable_packet_map;
	ReliablePacketQueue m_reliable_packet_queue;

	typedef std::deque<ConfirmationId> ConfirmationQueue;
	ConfirmationQueue m_confirmation_queue;

	ConfirmationId m_next_confirmation_id;

	uint32 *m_duplicate_filter;
	bool m_duplicate_filter_low;

	bool m_statmode_toggle;
	uint32 m_throttle_threshold;

	// number of bytes for the ip + udp headers
	static const uint32 UDP_OVERHEAD = 24;

	// these are the total number of packets and bytes sent or received
	// during the current cycle
	uint32 m_packets_peer_to_self;
	uint32 m_packets_self_to_peer;
	uint32 m_bytes_peer_to_self;
	uint32 m_bytes_self_to_peer;

	double m_average_outgoing_packet_size;
	uint32 m_packets_throttled;

	uint32 m_desired_rate_self_to_peer;
	uint32 m_desired_rate_peer_to_self;
	uint32 m_allocation_peer_to_self;
	uint32 m_allocation_self_to_peer;

	// NB: all bandwidths are bytes per second

	// bandwidth that would be required for packets sent from the peer to
	// this host, in the absence of any bandwidth restrictions.
	double m_desired_bandwidth_peer_to_self;

	// bandwidth that would be required to send all packets scheduled for
	// this peer, in the absence of any bandwidth restrictions.
	double m_desired_bandwidth_self_to_peer;

	// actual bandwidth used by the peer to send packets to this host,
	// as measured by received packets (ie. neglecting any dropped in
	// transit or due to saturation of this host's incoming bandwidth).
	double m_measured_bandwidth_peer_to_self;

	// actual bandwidth used to send packets to the peer from this host,
	// as measured by sent packets (ie. neglecting any dropped in transit).
	double m_measured_bandwidth_self_to_peer;

	// total (nominal) peer bandwidth.  these values are configured
	// manually by each host, and exchanged during the initial handshake.
	double m_total_peer_incoming_bandwidth;
	double m_total_peer_outgoing_bandwidth;

	// time skew of the peer relative to local time (positive if the peer is ahead),
	// in ms.
	double m_time_skew;

	// median round-trip latency, in ms.
	double m_roundtrip_latency;

	// transmission time (in ms) of the last ping received from this peer
	int m_last_ping_latency;

	// track timing data over successive pings
	Median9History<int> m_time_skew_history;
	Median9History<int> m_roundtrip_latency_history;
	double m_time_filter;

	// the elapsed time since the connection was established.
	double m_connect_time;

	// the elapsed time since the last ping was sent.
	double m_ping_time;

	// a measure of the time between packets sent to this peer that is
	// used to determine when additional pings need to be sent.
	double m_quiet_time;

	// the time elapsed since the last packet received from this peer.
	double m_dead_time;

	// the last time this connection was deactivated, used to inhibit recycling
	// recently used connections.
	double m_last_deactivation_time;

	ScopedPointer<DatagramTransmitSocket> m_socket;

	/** Update packet throttling parameters based on data received in the last batch
	 *  of packets from this peer.
	 *
	 *  @param dt The elapse time (in seconds) since the last call to update.
	 *  @param scale_desired_rate_to_self A scaling factor used to set connstat(peer_to_self).
	 *    TODO: need to write up a more detailed description of connstat and throttling.
	 */
	void update(double dt, double scale_desired_rate_to_self);

	/** Register a reliable packet, and assign a confirmation id.  PeerInfo will resend this packet
	 *  periodically until the corresponding confirmation id is received.
	 */
	void registerConfirmation(PacketReceiptHeader *receipt, const uint32 payload_length);

public:

	PeerInfo();
	~PeerInfo();

	/** Initialize the peer id.  Can only be called once.
	 */
	void setId(PeerId id) {
		assert(m_id == 0);
		m_id = id;
	};

	/** Get the id assigned to this peer.
	 */
	inline PeerId getId() const { return m_id; }

	/** Returns true if no packets have been sent to this peer recently, or there is
	 *  a backlog of reliable packet confirmations to send (which can piggyback on
	 *  ping messages).
	 */
	inline bool needsPing() {
		// ping quickly at first to help establish a stable time offset; and at
		// least occasionally after that.
		const double ping_limit = (m_connect_time < 30.0 ? 1.0 : 10.0);
		const double quiet_limit = (hasPendingConfirmations() ? 0.0 : 0.5);
		bool ping = (m_quiet_time > quiet_limit || m_ping_time > ping_limit);
		if (ping) m_ping_time = 0.0;
		return ping;
	}

	/** Returns an estimate of the average transmission rate (in bytes per second) from
	 *  this peer to us that would be sent in the absense of bandwidth constraints.
	 */
	inline uint32 getDesiredRatePeerToSelf() const {
		return m_desired_rate_peer_to_self;
	}

	/** Record a packet received from this peer.  This method updates statistics used
	 *  for packet throttling.
	 */
	inline void tallyReceivedPacket(uint32 bytes) {
		m_packets_peer_to_self++;
		m_bytes_peer_to_self += bytes + UDP_OVERHEAD;
	}

	/** Record a packet sent to this peer.  This method updates statistics used
	 *  for packet throttling.
	 */
	inline void tallySentPacket(uint32 bytes) {
		m_packets_self_to_peer++;
		m_bytes_self_to_peer += bytes + UDP_OVERHEAD;
		m_quiet_time = 0.0;
	}

	/** Record a packet that would have been sent to this peer, but was dropped due to
	 *  bandwidth constraints.
	 */
	inline void tallyThrottledPacket() {
		m_packets_throttled++;
	}

	/** Gets the next reliable packet to be resent, if any.
	 *
	 *  @param now The current time, as returned by get_realtime().
	 *  @return A reliable packet to resend, or a null reference.
	 */
	Ref<ReliablePacket> getNextResend(double now);

	/** Set the maximum incoming and outgoing bandwidths (in bytes/second) of this peer.
	 */
	void updateBandwidth(double incoming, double outgoing);

	/** Set the network node, and incoming/outgoing bandwidths (bytes/sec) of this peer.
	 */
	void setNode(NetworkNode const &node, double incoming, double outgoing);

	/** Get the network node of this peer.
	 */
	NetworkNode const &getNode() const { return m_node; }

	/** Get the maximum inbound bandwidth of this peer, in bytes per second.  This is
	 *  a fixed value specified by the peer (not a measured property of the connection).
	 */
	double incomingBandwidth() const { return m_total_peer_incoming_bandwidth; }

	/** Get the maximum outbound bandwidth of this peer, in bytes per second.  This is
	 *  a fixed value specified by the peer (not a measured property of the connection).
	 */
	double outgoingBandwidth() const { return m_total_peer_outgoing_bandwidth; }

	/** Records throttling data included in packets received from this peer.  See setConnStat
	 *  for details.
	 */
	inline void getConnStat(PacketHeader const *header) {
		resetDeadTime();
		if (header->statmode() == 1) {
			m_desired_rate_peer_to_self = header->connstat();
		} else {
			m_allocation_self_to_peer = header->connstat();
		}
	}

	/** Returns true if we have received reliable packets from this peer that have not
	 *  yet been confirmed.
	 */
	inline bool hasPendingConfirmations() const {
		return !m_confirmation_queue.empty();
	}

	/** Get the elapsed time (in seconds) since receiving the last packet from this peer.
	 */
	inline double getDeadTime() const {
		return m_dead_time;
	}

	/** Reset the dead-time (watchdog) counter for this connection.  This method is
	 *  called whenever a packet is received from this peer.
	 */
	inline void resetDeadTime() {
		m_dead_time = 0.0;
	}

	/** Gets the time (as returned by get_realtime) at which this connection
	 *  was deactivated, or 0 if never deactivated.
	 */
	inline double getLastDeactivationTime() const {
		return m_last_deactivation_time;
	}

	/** Register a reliable packet being sent to this peer and/or confirm receipt of one or more
	 *  reliable packets from this peer.  If reliable is true, the packet will be assigned a
	 *  confirmation id and resent periodically until the corresponding confirmation id is
	 *  received.  In addition, up to three pending confirmation id receipts will be sent to this
	 *  peer.  If reliable is false, this method should only be called if hasPendingConfirmations
	 *  is true (in which case up to four receipts will be sent).  Non-reliable packets should
	 *  use PacketHeader, rather than the extended PacketReceiptHeader, when there are no pending
	 *  confirmations to be sent.
	 */
	void setReceipt(PacketReceiptHeader *receipt, bool reliable, uint32 payload_length);

	/** Called for reliable packets received from this peer.  Adds the confirmation id to a queue
	 *  of ids that are included in extended headers of packets sent to this peer.  Once the
	 *  peer receives the confirmation id, reliable transmission of the packet is complete.
	 */
	inline void pushConfirmation(ConfirmationId id) {
		SIMNET_LOG(DEBUG, PEER) << "Peer requests confirmation of id " << id;
		m_confirmation_queue.push_back(id);
	}

	/** Test if the given confirmation id has already been received from this peer.  Uses a
	 *  sliding window to detect duplicates in the last 16k reliable messages received from
	 *  this peer.
	 */
	bool isDuplicate(const ConfirmationId id);

	/** Called when we receive confirmation of a reliable packet sent to this peer.  Once
	 *  confirmation is received, reliable transmission of the packet is considered to be
	 *  successful and no further resends will occur.
	 */
	inline void popConfirmation(ConfirmationId id) {
		ReliablePacketMap::iterator iter = m_reliable_packet_map.find(id);
		if (iter != m_reliable_packet_map.end()) {
			iter->second->confirm();
			m_reliable_packet_map.erase(iter);
		} else {
			SIMNET_LOG(WARNING, PEER) << "Received confirmation that we did not request " << id;
		}
	}

	/** Send throttling data to this peer.  Each packet contains one of two types of throttling
	 *  data: the desired rate at which we would send data to this peer in the absense of
	 *  bandwidth constraints, or the maximum rate at which this peer should send data to us.
	 *  These values are stored in a single field in the packet header (connstat) with a bit
	 *  (statmode) to indicate the type, which alternates on successive packets.  The connstat
	 *  field is 10-bits, and the values are interpreted in terms of the maximum inbound and
	 *  outbound bandwidths of the two hosts.
	 */
	inline void setConnStat(PacketHeader *header) {
		if (m_statmode_toggle) {
			header->setStatMode(1);
			header->setConnStat(m_desired_rate_self_to_peer);
		} else {
			header->setStatMode(0);
			header->setConnStat(m_allocation_peer_to_self);
		}
		m_statmode_toggle = !m_statmode_toggle;
	}

	/** Update timing stats from ping data.
	 *
	 * @param ping_latency The transit time of a ping received from this peer,
	 *   in ms, using the uncorrected epoch times on the local and remote machines.
	 * @param time_offset The transit time of the last ping sent to this peer.
	 */
	void updateTiming(int ping_latency, int last_ping_latency);

	/** Get the transit time, in ms, of the last ping received from this peer.
	 */
	inline int getLastPingLatency() { return m_last_ping_latency; }

	/** Marks this connection as inactive and releases the outbound socket.
	 */
	void disable();

	/** Returns true if the connection to this peer is active.
	 */
	inline bool isActive() const { return m_active; }

	/** Returns true if a connection with this peer has been initiated but not yet
	 *  completed.  Packet routing is restricted from provisional connections, which
	 *  timeout unless setProvisional(false) is called.
	 */
	inline bool isProvisional() const { return m_provisional; }

	/** Returns true for provisional connections that have exceeded their lifetime.
	 *  See setProvisional for details.
	 */
	inline bool isProvisionalExpired() const { return m_provisional && m_lifetime <= 0.0; }

	/** Set the connection to this peer to be provisional or established.  If provisional,
	 *  lifetime is the time limit (in seconds) for the connection to be established.  If
	 *  provisional is false, the connection is treated as established.
	 */
	void setProvisional(bool provisional=true, double lifetime=10.0) {
		m_provisional = provisional;
		m_lifetime = lifetime;
	}

	/** Bandwidth constraints may require that some packets intended for this peer be
	 *  dropped prior to transmission.  This method rolls the dice to select packets
	 *  to drop, and returns true if a packet should be throttled.  The throttling rate
	 *  is adjusted by the update() method based on feedback received from the peer.
	 */
	inline bool throttlePacket(int /*priority*/) {
		return (m_throttle_threshold > 0) ? (NetRandom::random() < m_throttle_threshold) : false;
	}

	/** Get the dedicated udp socket used to send packets to this peer.
	 */
	inline DatagramTransmitSocket *getSocket() { return m_active ? m_socket.get() : 0; }

	/** Get the time offset of the peer relative to the local machine, in msec.
	 *  This value is filtered increasingly aggressively with each ping received,
	 *  such that the offset will quickly settle down to a stable value. Note that
	 *  this assumes that the remote machine's time runs at very nearly the same rate
	 *  as the local machine.  Otherwise the time skew will grow steadily but may not
	 *  keep pace with the actual time difference.
	 */
	inline double getTimeSkew() const { return m_time_skew; }

	/** Get the roundtrip latency, in msec.  Provides a rough measure of the
	 *  median roundtrip delay over the last several ping packets.  This
	 *  delay includes the time required for the network layer to receive and
	 *  decode the ping packets, but does not include any higher level processing
	 *  time.  If the network interface is driven periodically as part of the
	 *  simulation main loop, this delay will be included in the latency measure.
	 */
	inline double getRoundtripLatency() const { return m_roundtrip_latency; }

};


/**
 * ActivePeerList is a tool for managing active connections.  PeerInfo
 * instances are allocated by NetworkInterface as a linear array to allow
 * fast id to instance lookups.  Typically only a small number of these
 * instances will be active, so iterating over the entire array to update
 * connections is ineffecient.  This class keeps a separate list of
 * pointers to the active PeerInfo instances, and handles periodic
 * connection updates (bandwidth shaping, reliable message retransmission,
 * and heartbeats).
 */
class CSPLIB_EXPORT ActivePeerList: public NonCopyable {
	double m_ElapsedTime;
	uint32 m_DesiredRateToSelf;
	typedef std::vector<PeerInfo *> PeerList;
	PeerList m_ActivePeers;

public:
	ActivePeerList(): m_ElapsedTime(0.0), m_DesiredRateToSelf(0) {}

	// updates are called every cycle, but we accumulate statistics until
	// the elapsed time since the previous full update exceeds 100 ms.
	// we then update the connection statistics and throttling for each
	// active peer.
	void update(double dt, NetworkInterface *ni);

	void addPeer(PeerInfo *peer);
	void removePeer(PeerInfo *peer);
};

CSP_NAMESPACE_END

#endif // __CSPLIB_NET_PEERINFO_H__

