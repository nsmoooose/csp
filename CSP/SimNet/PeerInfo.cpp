// Combat Simulator Project - CSPSim
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
 *  @file PeerInfo.cpp
 *
 */

#include <SimNet/PeerInfo.h>
#include <SimNet/NetLog.h>
#include <SimNet/NetworkInterface.h>
#include <SimData/Timing.h>
#include <SimData/Verify.h>

#include <vector>
#include <algorithm>

namespace simnet {


template <unsigned int MILLISECONDS>
inline double lowpass(const double dt, const double x, const double y) {
	double f = std::min(1.0, dt * (1000.0/MILLISECONDS));
	return x * (1.0 - f) + y * f;
}

SIMDATA_STATIC_CONST_DEF(simdata::uint32 PeerInfo::UDP_OVERHEAD);
static int DEBUG_connection_display_loop = 0;

PeerInfo::PeerInfo():
	m_id(0),
	m_active(false),
	m_provisional(false),
	m_lifetime(0.0),
	m_next_confirmation_id(1000),
	m_duplicate_filter_low(true),
	m_statmode_toggle(false),
	m_throttle_threshold(0),
	m_packets_peer_to_self(0),
	m_packets_self_to_peer(0),
	m_bytes_peer_to_self(0),
	m_bytes_self_to_peer(0),
	m_average_outgoing_packet_size(100.0),
	m_packets_throttled(0),
	m_desired_rate_self_to_peer(100),
	m_desired_rate_peer_to_self(100),
	m_allocation_peer_to_self(100),  // ~10% of inbound bandwidth initially
	m_allocation_self_to_peer(100),  // ~10% of inbound bandwidth initially
	m_desired_bandwidth_peer_to_self(0.0),
	m_desired_bandwidth_self_to_peer(0.0),
	m_measured_bandwidth_peer_to_self(0.0),
	m_measured_bandwidth_self_to_peer(0.0),
	m_total_peer_incoming_bandwidth(0.0),
	m_total_peer_outgoing_bandwidth(0.0),
	m_time_skew(0.0),
	m_roundtrip_latency(0.0),
	m_last_ping_latency(0),
	m_time_filter(0.1),
	m_connect_time(0.0),
	m_ping_time(0.0),
	m_quiet_time(0.0),
	m_dead_time(0.0),
	m_last_deactivation_time(0.0),
	m_socket(0)
{
	memset(m_duplicate_filter, 0, sizeof(m_duplicate_filter));
}

void PeerInfo::update(double dt, double scale_desired_rate_to_self) {

	// update time averages with stats from the latest cycle
	const double new_psbw = double(m_bytes_peer_to_self) / dt;
	m_measured_bandwidth_peer_to_self = lowpass<2000>(dt, m_measured_bandwidth_peer_to_self, new_psbw);
	const double new_spbw = double(m_bytes_self_to_peer) / dt;
	m_measured_bandwidth_self_to_peer = lowpass<2000>(dt, m_measured_bandwidth_self_to_peer, new_spbw);
	if (m_packets_peer_to_self) {
		m_dead_time = 0.0;
	} else {
		m_dead_time += dt;
	}
	m_connect_time += dt;
	m_ping_time += dt;
	if (m_packets_self_to_peer > 0) {
		m_quiet_time = 0.0;
		const double outgoing_packet_size = (double(m_bytes_self_to_peer) / m_packets_self_to_peer);
		m_average_outgoing_packet_size = lowpass<2000>(dt, m_average_outgoing_packet_size, outgoing_packet_size);
	} else {
		// ping much more slowly if we are neither receiving nor sending.
		if (m_packets_peer_to_self > 0) {
			m_quiet_time += dt;
		} else {
			m_quiet_time += dt * 0.1;
		}
	}

	// update the desired bandwidth from self to peer
	const simdata::uint32 attempted_packets = m_packets_self_to_peer + m_packets_throttled;
	const simdata::uint32 attempted_bytes = static_cast<simdata::uint32>(attempted_packets * m_average_outgoing_packet_size);
	const double desired_bandwidth = double(attempted_bytes) / dt;
	m_desired_bandwidth_self_to_peer = lowpass<2000>(dt, m_desired_bandwidth_self_to_peer, desired_bandwidth);
	const double desired_rate = 100.0 * m_desired_bandwidth_self_to_peer / m_total_peer_incoming_bandwidth;
	m_desired_rate_self_to_peer = std::max(1U, std::min(static_cast<simdata::uint32>(desired_rate), 1023U));

	m_allocation_peer_to_self = static_cast<simdata::uint32>(m_desired_rate_peer_to_self * scale_desired_rate_to_self);

	// limit the inbound bandwidth we allocate for messages from this peer to between 1% and 100% of our
	// total inbound bandwidth.  the lower limit is important to prevent the peer from throttling all
	// packets to us, thereby preventing us from knowing that it needs more allocation!
	m_allocation_peer_to_self = std::min(1023U, std::max(10U, m_allocation_peer_to_self));

	// translate our allocated bandwidth to this peer into a throttling threshold
	const double allocated_bandwidth = m_allocation_self_to_peer * m_total_peer_incoming_bandwidth * (1.0 / 1024.0);
	const double throttle_fraction = std::min(0.99, std::max(0.0, 1.0 - (allocated_bandwidth / desired_bandwidth)));
	m_throttle_threshold = static_cast<simdata::uint32>(0xfffffffful * throttle_fraction);

	if (((DEBUG_connection_display_loop % 100) == 0)) {
		if (m_packets_self_to_peer > 0) {
			std::cout << "outgoing to " << m_id << ":\n";
			std::cout << "  avg packet size    : " << m_average_outgoing_packet_size << "\n";
			std::cout << "  outgoing bandwidth : " << m_measured_bandwidth_self_to_peer << "\n";
			std::cout << "  peer inc bandwidth : " << m_total_peer_incoming_bandwidth << "\n";
			std::cout << "  peer alloc connstat: " << m_allocation_self_to_peer << "\n";
			std::cout << "  allocated bandwidth: " << allocated_bandwidth << "\n";
			std::cout << "  desired bandwidth  : " << m_desired_bandwidth_self_to_peer << "\n";
			std::cout << "  throttle fraction  : " << throttle_fraction << "\n";
			std::cout << "  throttle threshold : " << m_throttle_threshold << "\n";
			std::cout << "  scale drate to self: " << scale_desired_rate_to_self << "\n";
			std::cout << "  drate peer to self : " << m_desired_rate_peer_to_self << "\n";
			std::cout << "  alloc peer to self : " << m_allocation_peer_to_self << "\n";
		}
		if (m_packets_peer_to_self > 0) {
			std::cout << "incoming from " << m_id << ":\n";
			std::cout << "  incoming bandwidth : " << m_measured_bandwidth_peer_to_self << "\n";
			std::cout << "  peer out bandwidth : " << m_total_peer_outgoing_bandwidth << "\n";
			std::cout << "  peer alloc commstat: " << m_allocation_peer_to_self << "\n";
			std::cout << "  peer desired rate  : " << m_desired_rate_peer_to_self << "\n";
			std::cout << "  peer rate scale    : " << scale_desired_rate_to_self << "\n";
			std::cout << "  roundtrip latency  : " << m_roundtrip_latency << " ms\n";
			std::cout << "  clock skew         : " << m_time_skew << " ms\n";
		}
	}

	// reset accumulators for the next cycle
	m_packets_self_to_peer = 0;
	m_bytes_self_to_peer = 0;
	m_packets_peer_to_self = 0;
	m_bytes_peer_to_self = 0;
	m_packets_throttled = 0;

	if (m_provisional) {
		m_lifetime -= dt;
	}
}


ReliablePacket::Ref PeerInfo::getNextResend(double now) {
	while (!m_reliable_packet_queue.empty()) {
		ReliablePacket::Ref packet = m_reliable_packet_queue.top();
		if (packet->isConfirmed()) {
			m_reliable_packet_queue.pop();
			ReliablePacketMap::iterator iter = m_reliable_packet_map.find(packet->getId());
			if (iter != m_reliable_packet_map.end()) m_reliable_packet_map.erase(iter);
			continue;
		}
		if (packet->nextTime() > now) return 0;
		m_reliable_packet_queue.pop();
		packet->updateAttempt(now);
		m_reliable_packet_queue.push(packet);
		return packet;
	}
	return 0;
}


void PeerInfo::updateBandwidth(double incoming, double outgoing) {
	m_total_peer_incoming_bandwidth = incoming;
	m_total_peer_outgoing_bandwidth = outgoing;
}


void PeerInfo::setNode(NetworkNode const &node, double incoming, double outgoing) {
	m_node = node;
	m_socket.reset(new DatagramTransmitSocket());
	m_socket->connect(node.getAddress(), node.getPort());
	m_total_peer_incoming_bandwidth = incoming;
	m_total_peer_outgoing_bandwidth = outgoing;
	m_active = true;
}


void PeerInfo::registerConfirmation(PacketReceiptHeader *receipt, const simdata::uint32 payload_length) {
	ConfirmationId id = m_next_confirmation_id;
	receipt->setId0(id);
	if (++m_next_confirmation_id == 0) m_next_confirmation_id = 1;  // 0 reserved for 'no id'

	// there are 65534 sequential ids, so it's very unlikely we would wrap
	// around and hit an old id.  if so, just log an error and move on.
	if (m_reliable_packet_map.find(id) != m_reliable_packet_map.end()) {
		m_reliable_packet_map[id]->confirm();
		SIMNET_LOG(PEER, WARNING, "reassigning to active confirmation id");
	}

	// save the packet data in case we need to resend it
	const simdata::uint32 packet_length = payload_length + sizeof(PacketReceiptHeader);
	ReliablePacket::Ref rpacket = new ReliablePacket();
	rpacket->assign(id, receipt, packet_length);
	m_reliable_packet_map[id] = rpacket;
	m_reliable_packet_queue.push(rpacket);
}


void PeerInfo::setReceipt(PacketReceiptHeader *receipt, bool reliable, simdata::uint32 payload_length) {
	// clear all the extra ids first (important!)
	receipt->setId1(0);
	receipt->setId2(0);
	receipt->setId3(0);

	// if it is a reliable packet, we save it the packet in m_reliable_packet_map for
	// retransmission if we don't get a timely confirmation of receipt.   we store
	// pending confirmations of reliable packets we received in the remaining slots.
	if (reliable) {
		registerConfirmation(receipt, payload_length);
	} else {
		// we shouldn't be calling setReceipt if there aren't any pending confirmations
		assert(hasPendingConfirmations());
		receipt->setId0(m_confirmation_queue.front());
		m_confirmation_queue.pop_front();
		SIMNET_LOG(PACKET, DEBUG, "sending receipt " << receipt->id0());
	}

	if (!hasPendingConfirmations()) return;
	receipt->setId1(m_confirmation_queue.front());
	m_confirmation_queue.pop_front();
	SIMNET_LOG(PACKET, DEBUG, "sending receipt " << receipt->id1());

	if (!hasPendingConfirmations()) return;
	receipt->setId2(m_confirmation_queue.front());
	m_confirmation_queue.pop_front();
	SIMNET_LOG(PACKET, DEBUG, "sending receipt " << receipt->id2());

	if (!hasPendingConfirmations()) return;
	receipt->setId3(m_confirmation_queue.front());
	m_confirmation_queue.pop_front();
	SIMNET_LOG(PACKET, DEBUG, "sending receipt " << receipt->id3());
}

bool PeerInfo::isDuplicate(const ConfirmationId id) {
	const simdata::uint32 mask = 1 << (id & 31);
	const int index = id/32;
	const simdata::uint32 value = m_duplicate_filter[index];
	const bool duplicate = ((value & mask) != 0);
	m_duplicate_filter[index] = value | mask;
	if (m_duplicate_filter_low && (id >= 65536/32*3/4)) {
		m_duplicate_filter_low = false;
		memset(reinterpret_cast<char*>(m_duplicate_filter), 0, 65536/2);
	} else if (!m_duplicate_filter_low && (id >= 65536/32/4)) {
		m_duplicate_filter_low = true;
		memset(reinterpret_cast<char*>(m_duplicate_filter) + 65536/2, 0, 65536/2);
	}
	return duplicate;
}

void PeerInfo::disable() {
	if (m_active) {
		m_last_deactivation_time = simdata::get_realtime();
	}
	m_active = false;
	if (m_socket.valid()) m_socket->disconnect();
}

void PeerInfo::updateTiming(int ping_latency, int last_ping_latency) {
	int roundtrip_latency = (ping_latency + last_ping_latency);
	roundtrip_latency = m_roundtrip_latency_history.add(roundtrip_latency);
	double latency_filter = 0.8;
	if (m_roundtrip_latency > 1.25 * roundtrip_latency) latency_filter = 0.0;
	if (m_roundtrip_latency < 0.80 * roundtrip_latency) latency_filter = 0.0;
	m_roundtrip_latency = m_roundtrip_latency * latency_filter + roundtrip_latency * (1.0 - latency_filter);

	int correction = (ping_latency - last_ping_latency) / 2;
	correction = m_time_skew_history.add(correction);
	m_time_skew = m_time_skew * m_time_filter + correction * (1.0 - m_time_filter);
	m_last_ping_latency = ping_latency;
	if (m_time_filter < 0.9999) {
		m_time_filter += (1.0 - m_time_filter) * 0.1;
	}
	if (m_time_filter > 0.9999) {
		m_time_filter = 0.9999;
	}
	//std::cout << "clock skew = " << m_time_skew << "\n";
	//std::cout << "round trip = " << m_roundtrip_latency << "\n";
}

void ActivePeerList::update(double dt, NetworkInterface *ni) {
	SIMDATA_ASSERT_GE(dt, 0.0);
	SIMDATA_ASSERT_LT(dt, 1000.0);

	m_ElapsedTime += dt;
	if (m_ElapsedTime < 0.1) return;
	double now = simdata::get_realtime();
	simdata::uint32 desired_rate_to_self = 0;
	double scale_desired_peer_to_self = 1024.0 / std::max(1U, m_DesiredRateToSelf);
	std::vector<PeerInfo*> remove_peers;
	for (PeerList::iterator iter = m_ActivePeers.begin(); iter != m_ActivePeers.end(); ++iter) {
		PeerInfo *peer = (*iter);
		// we use the total desired rate to self from the previous update to avoid making
		// two passes through the active peers.  the value will be slightly stale, but it
		// should be changing slowly enough (due to lowpass filtering at each peer) that
		// this lag won't have a large effect.
		peer->update(m_ElapsedTime, scale_desired_peer_to_self);
		desired_rate_to_self += peer->getDesiredRatePeerToSelf();
		if (peer->needsPing()) {
			ni->pingPeer(peer);
		}
		for(;;) {
			ReliablePacket::Ref packet = peer->getNextResend(now);
			if (!packet) break;
			ni->resend(packet);
		}
		bool remove = false;
		if (peer->getDeadTime() > 30.0) {
			SIMNET_LOG(PEER, ALERT, "dead time expired for peer " << peer->getId() << " (" << peer->getDeadTime() << " s)");
			if (ni->handleDeadPeer(peer)) remove = true;
		}
		if (peer->isProvisionalExpired()) {
			SIMNET_LOG(PEER, ALERT, "provisional time expired for peer " << peer->getId());
			remove = true;
		}
		if (remove) remove_peers.push_back(peer);
	}
	for (unsigned i=0; i < remove_peers.size(); ++i) {
		// note that there's a slight race condition here, in that we may expire
		// a provisional connection just as the other side receives the connection
		// response. if this turns out to be a problem, we can send a "cease
		// & desist" packet back to the errant peer.
		ni->removePeer(remove_peers[i]->getId());
	}
	m_DesiredRateToSelf = desired_rate_to_self;
	m_ElapsedTime = 0.0;
	DEBUG_connection_display_loop++;
}


void ActivePeerList::addPeer(PeerInfo *peer) {
	assert(peer->isActive());
	m_ActivePeers.push_back(peer);
}


void ActivePeerList::removePeer(PeerInfo *peer) {
	PeerList::iterator iter = std::find(m_ActivePeers.begin(), m_ActivePeers.end(), peer);
	m_ActivePeers.erase(iter);
}

} // namespace simnet

