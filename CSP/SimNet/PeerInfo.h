// Combat Simulator Project - FlightSim Demo
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

#ifndef __SIMNET_PEERINFO_H__
#define __SIMNET_PEERINFO_H__


#include <SimNet/NetBase.h>
#include <SimNet/NetRandom.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/ReliablePacket.h>
#include <SimNet/Sockets.h>
#include <SimNet/NetLog.h>

#include <SimData/Properties.h>
#include <SimData/ScopedPointer.h>

#include <map>
#include <queue>
#include <vector>
#include <deque>


namespace simnet {

class NetworkInterface;


template <unsigned int MILLISECONDS>
inline double lowpass(const double dt, const double x, const double y) {
	double f = std::min(1.0, dt * (1000.0/MILLISECONDS));
	return x * (1.0 - f) + y * f;
}


class PeerInfo: public simdata::NonCopyable {
	friend class ActivePeerList;

	PeerId m_id;
	bool m_active;

	typedef std::map<ConfirmationId, ReliablePacket::Ref> ReliablePacketMap;
	typedef std::vector<ReliablePacket::Ref> ReliablePacketVector;
	typedef std::priority_queue<ReliablePacket::Ref, ReliablePacketVector, ReliablePacket::Order> ReliablePacketQueue;
	ReliablePacketMap m_reliable_packet_map;
	ReliablePacketQueue m_reliable_packet_queue;

	typedef std::deque<ConfirmationId> ConfirmationQueue;
	ConfirmationQueue m_confirmation_queue;

	ConfirmationId m_next_confirmation_id;

	bool m_statmode_toggle;
	simdata::uint32 m_throttle_threshold;

	static const simdata::uint32 UDP_OVERHEAD = 24;

	// these are the total number of packets and bytes sent or received
	// during the current cycle
	simdata::uint32 m_packets_peer_to_self;
	simdata::uint32 m_packets_self_to_peer;
	simdata::uint32 m_bytes_peer_to_self;
	simdata::uint32 m_bytes_self_to_peer;

	double m_average_outgoing_packet_size;
	simdata::uint32 m_packets_throttled;

	simdata::uint32 m_desired_rate_self_to_peer;
	simdata::uint32 m_desired_rate_peer_to_self;
	simdata::uint32 m_allocation_peer_to_self;
	simdata::uint32 m_allocation_self_to_peer;

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

	double m_quiet_time;

	simdata::ScopedPointer<DatagramTransmitSocket> m_socket;

	void update(double dt, double scale_desired_rate_to_self);

public:

	PeerInfo():
		m_id(0),
		m_active(false),
		m_statmode_toggle(false),
		m_next_confirmation_id(1000),
		m_throttle_threshold(0),
		m_packets_peer_to_self(0),
		m_packets_self_to_peer(0),
		m_bytes_peer_to_self(0),
		m_bytes_self_to_peer(0),
		m_average_outgoing_packet_size(100.0),
		m_packets_throttled(0),
		m_desired_rate_self_to_peer(0),
		m_desired_rate_peer_to_self(0),
		m_allocation_peer_to_self(100),  // ~10% of inbound bandwidth initially
		m_allocation_self_to_peer(100),  // ~10% of inbound bandwidth initially
		m_desired_bandwidth_peer_to_self(0.0),
		m_desired_bandwidth_self_to_peer(0.0),
		m_measured_bandwidth_peer_to_self(0.0),
		m_measured_bandwidth_self_to_peer(0.0),
		m_total_peer_incoming_bandwidth(0.0),
		m_total_peer_outgoing_bandwidth(0.0),
		m_quiet_time(0.0),
		m_socket(0)
		{ }


	void setId(PeerId id) {
		assert(m_id == 0);
		m_id = id;
	};

	inline PeerId getId() const { return m_id; }

	inline bool needsPing() const {
		double limit = (hasPendingConfirmations() ? 0.0 : 0.5);
		return m_quiet_time > limit;
	}

	inline simdata::uint32 getDesiredRatePeerToSelf() const {
		return m_desired_rate_peer_to_self;
	}

	inline void tallyReceivedPacket(simdata::uint32 bytes) {
		m_packets_peer_to_self++;
		m_bytes_peer_to_self += bytes + UDP_OVERHEAD;
	}

	inline void tallySentPacket(simdata::uint32 bytes) {
		m_packets_self_to_peer++;
		m_bytes_self_to_peer += bytes + UDP_OVERHEAD;
		m_quiet_time = 0.0;
	}

	inline void tallyThrottledPacket() {
		m_packets_throttled++;
	}

	ReliablePacket::Ref getNextResend(double now);

	void setNode(NetworkNode const &node, double incoming, double outgoing);

	inline void getConnStat(PacketHeader const *header) {
		if (header->statmode == 1) {
			m_desired_rate_peer_to_self = header->connstat;
		} else {
			m_allocation_self_to_peer = header->connstat;
		}
	}

	inline bool hasPendingConfirmations() const {
		return !m_confirmation_queue.empty();
	}

	void registerConfirmation(PacketReceiptHeader *receipt, const simdata::uint32 payload_length);

	void setReceipt(PacketReceiptHeader *receipt, bool reliable, simdata::uint32 payload_length);

	// peer is asking us to confirm that we received this id.  queue it, to be
	// sent back in the receipt headers of packets we send to this peer.
	inline void pushConfirmation(ConfirmationId id) {
		std::cout << "PEER REQUESTS CONFIRMATION OF " << id << "\n";
		m_confirmation_queue.push_back(id);
	}

	// peer has confirmed receiving this id from us
	inline void popConfirmation(ConfirmationId id) {
		ReliablePacketMap::iterator iter = m_reliable_packet_map.find(id);
		if (iter != m_reliable_packet_map.end()) {
			iter->second->confirm();
			m_reliable_packet_map.erase(iter);
		} else {
			SIMNET_LOG(PEER, WARNING, "Received confirmation that we did not request");
		}
	}

	inline void setConnStat(PacketHeader *header) {
		if (m_statmode_toggle) {
			header->statmode = 1;
			header->connstat = m_desired_rate_self_to_peer;
		} else {
			header->statmode = 0;
			header->connstat = m_allocation_peer_to_self;
		}
		m_statmode_toggle = !m_statmode_toggle;
	}

	bool disable();

	inline bool isActive() const { return m_active; }

	inline bool throttlePacket(int priority) {
		return (m_throttle_threshold > 0) ? (NetRandom::random() < m_throttle_threshold) : false;
	}

	inline DatagramTransmitSocket *getSocket() { return m_active ? m_socket.get() : 0; }
};


class ActivePeerList: public simdata::NonCopyable {
	double m_ElapsedTime;
	simdata::uint32 m_DesiredRateToSelf;
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

} // namespace simnet

#endif // __SIMNET_PEERINFO_H__

