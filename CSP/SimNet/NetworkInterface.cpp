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
 * @file NetworkInterface.cpp
 *
 */

#include <SimNet/NetworkInterface.h>
#include <SimNet/Sockets.h>
#include <SimNet/NetworkNode.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/ReliablePacket.h>
#include <SimNet/PeerInfo.h>
#include <SimNet/StopWatch.h>
#include <SimNet/NetLog.h>

#include <SimData/TaggedRecordRegistry.h>
#include <SimData/Timing.h>
#include <SimData/Thread.h>

// use this to fix compile problems with mulitple includes of windows and winsock headers.
#define _WINSOCKAPI_
#include <cc++/network.h>

#include <sys/types.h>
#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <queue>


// TODO
//
//   * cache reliable packets in a free list instead of allocating strings?
//
//   * better handling of messages bound for multiple peers --- we don't want
//     to have to serialize the same update record multiple times!  probably
//     easiest to have MessageQueue accept multiple destinations.  if N > 1,
//     cache the serialized payload and reuse it for the other destinations
//     (only need one cache buffer for this).
//
//   * broadcasts/relays: need to extend the packet header to include multiple
//     destinations (add a bit flag and an extended header with room for up
//     to N destinations?).
//


namespace simnet {

// XXX hack
void NetworkInterface::hackBuildMessageIndex() {
	simdata::TaggedRecordRegistry const &registry = simdata::TaggedRecordRegistry::getInstance();
	std::vector<simdata::TaggedRecordFactoryBase *> factories = registry.getFactories();
	for (unsigned i = 0; i < factories.size(); ++i) {
		factories[i]->setCustomId(i);
	}
}


/** Callback for HandlerSet<PacketHandler>::apply, to call the handlePacket
 *  method of each of the handlers in the set.
 */
class PacketHandlerCallback {
	PacketHeader *m_header;
	simdata::uint8 *m_payload;
	simdata::uint32 m_payload_length;

public:
	/** Construct a new callback, storing information about the packet to be
	 *  handled.
	 */
	PacketHandlerCallback(PacketHeader *header, simdata::uint8 *payload, simdata::uint32 payload_length):
		m_header(header), m_payload(payload), m_payload_length(payload_length) { }

	/** Callback function that calls handler->handlePacket with the current
	 *  packet information.
	 */
	inline void operator()(PacketHandler::Ref handler) {
		handler->handlePacket(m_header, m_payload, m_payload_length);
	}
};


void NetworkInterface::sendPackets(double timeout) {
	static StopWatch::Data swd(0.00001);
	simdata::uint8 *ptr;
	ost::InetHostAddress addr;
	uint32 size;

	int queue_idx = 3;
	int process_count[] = {4, 6, 10, 20};
	int count = process_count[queue_idx];
	PacketQueue *queue = m_TxQueues[queue_idx];

	bool first = true;

	SIMNET_LOG(TIMING, DEBUG, "TRANSMIT " << (timeout * 1000.0) << " ms available to send packets");

	StopWatch watch(timeout, swd);
	watch.start();

	while (true) {

		// TODO abstract the scheduling strategy
		if (queue->isEmpty() || count <= 0) {
			int n = 0;
			do {
				// check if all queues are empty
				queue_idx = (queue_idx - 1) & 3;
				queue = m_TxQueues[queue_idx];
			} while (++n <= 4 && queue->isEmpty());
			if (n > 4) break;  // all empty
			// if only one queue isn't empty then forget about counts
			count = (n==4) ? 100000 : process_count[queue_idx];
		}
		--count;

		// TODO replace asserts with more robust error handling
		ptr = queue->getReadBuffer(size);
		assert(ptr);
		PacketHeader *header = reinterpret_cast<PacketHeader*>(ptr);
		PeerInfo *peer = getPeer(header->destination);
		assert(peer->isActive());
		peer->setConnStat(header);
		DatagramTransmitSocket *socket = peer->getSocket();
		assert(socket);

		// make sure the socket is ready; otherwise if we have already
		// sent some packets then try waiting for one millisecond (approx)
		// and try again.  if it still isn't free, we just give up for
		// this round.
		if (!socket->isOutputReady()) {
			m_OutputStalls++;
			if (first) {
				queue->replaceReadBuffer();
				break;
			}
			// the sleep only has ms granularity, so this is somewhat dubious (XXX)
			double sleep_time = std::min(0.001, timeout);
			simdata::thread::sleep(sleep_time);
			if (!socket->isOutputReady()) {
				queue->replaceReadBuffer();
				break;
			}
			// check our timeout after the next packet is sent, and don't
			// update the check_count
			watch.forceCheck();
		} else {
			first = false;
		}

		if (header->reliable) std::cout << "->SEND REL TO PEER " << peer->getId() << ", SIZE=" << size << "\n";
		int len = socket->transmit((char*)ptr, size);
		queue->releaseReadBuffer();
		m_SentPackets++;
		peer->tallySentPacket(size);

		if (len != int(size)) {
			SIMNET_LOG(PACKET, INFO, "Send underflow! (" << len << " of " << size << " bytes sent)");
		}

		// check the time after several packets are sent, and bail if
		// we're taking too long.
		if (watch.checkExpired()) break;
	}

	SIMNET_LOG(TIMING, DEBUG, "TRANSMIT COMPLETE " << (watch.elapsed() * 1000.0) << " ms used");

	// drop a fraction of the packets we were unable to send
	for (int idx = 0; idx < 4; ++idx) {
		m_TxQueues[idx]->dropOldest();
	}

}


void NetworkInterface::resend(simdata::Ref<ReliablePacket> &packet) {
	const int queue_idx = 3;
	PacketQueue *queue = m_TxQueues[queue_idx];
	simdata::uint32 packet_size = packet->size();
	simdata::uint8 *ptr = queue->getWriteBuffer(packet_size);
	packet->copy(ptr);
	queue->commitWriteBuffer(packet_size);
}


bool NetworkInterface::pingPeer(PeerInfo *peer) {
	const int queue_idx = 2;
	PacketQueue *queue = m_TxQueues[queue_idx];
	bool confirm = peer->hasPendingConfirmations();
	unsigned int packet_size = confirm ? ReceiptHeaderSize : HeaderSize;
	simdata::uint8 *ptr = queue->getWriteBuffer(packet_size);
	if (!ptr) return false;
	PacketHeader *header = reinterpret_cast<PacketHeader*>(ptr);
	header->reliable = 0;
	header->priority = queue_idx;
	header->source = m_ServerId;
	header->destination = peer->getId();
	header->message_id = PingID;
	if (confirm) {
		PacketReceiptHeader *receipt = reinterpret_cast<PacketReceiptHeader*>(ptr);
		header->reliable = 1;
		peer->setReceipt(receipt, false, 0);
	}
	std::cout << "PING " << peer->getId() << " .......\n";
	queue->commitWriteBuffer(packet_size);
	return true;
}


void NetworkInterface::processOutgoing(double timeout) {
	static StopWatch::Data swd(0.00001);

	assert(m_Initialized && m_PacketSource.valid());

	// TODO divide timeout between packet_source and tx based
	// on queue lengths.  for now just split evenly

	SIMNET_LOG(TIMING, DEBUG, "outgoing loop start (" << m_PacketSource->size() << " packets to send)");
	int DEBUG_exitcode = 0;
	double DEBUG_dt[20000];
	int DEBUG_idx = 0;

	StopWatch watch(0.5 * timeout, swd);
	watch.start();
	double t0 = simdata::get_realtime();

	const uint32 allocation_size = MaxPayloadLength + HeaderSize;

	uint32 payload_length;
	int queue_idx;
	PeerId destination;

	// first transfer packets from a packet source to the appropriate tx queue
	while (m_PacketSource->peek(destination, queue_idx)) {
		assert(queue_idx >= 0 && queue_idx < 4);
		PeerInfo *peer = getPeer(destination);
		if (queue_idx < 3) {
			if (peer->throttlePacket(queue_idx)) {
				m_PacketSource->skipPacket();
				peer->tallyThrottledPacket();
				m_ThrottledPackets++;
				continue;
			}
		}

		PacketQueue *queue = m_TxQueues[queue_idx];

		simdata::uint8 *ptr = queue->getWriteBuffer(allocation_size);
		if (!ptr) {
			DEBUG_exitcode = 1;
			break;
		}

		PacketHeader *header = reinterpret_cast<PacketHeader*>(ptr);
		PacketReceiptHeader *receipt = 0;
		int header_size = HeaderSize;

		const bool reliable = (queue_idx == 3);
		if (reliable || peer->hasPendingConfirmations()) {
			std::cout << "SENDING RELIABLE\n";
			header->reliable = 1;
			header_size = ReceiptHeaderSize;
			receipt = reinterpret_cast<PacketReceiptHeader*>(ptr);
		} else {
			header->reliable = 0;
		}
		header->priority = queue_idx;
		header->source = m_ServerId;

		simdata::uint8 *payload = ptr + header_size;
		payload_length = allocation_size - header_size;
		m_PacketSource->getPacket(header, payload, payload_length);

		if (receipt) {
			peer->setReceipt(receipt, reliable, payload_length);
		}

		queue->commitWriteBuffer(header_size + payload_length);

		double t1 = simdata::get_realtime();
		DEBUG_dt[DEBUG_idx++] = t1 - t0;

		if (watch.checkExpired()) {
			DEBUG_exitcode = 2;
			break;
		}
	}

	double DEBUG_elapsed = watch.elapsed();
	timeout -= watch.elapsed();
	//double DEBUG_remaining = timeout;

	// next flush the tx queue to the outgoing sockets
	sendPackets(timeout);

	SIMNET_LOG(TIMING, DEBUG, "outgoing loop end: " << (DEBUG_elapsed * 1000.0) << " ms to queue packets");
	if (DEBUG_exitcode == 1) {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: queue buffer full");
	} else
	if (DEBUG_exitcode == 2) {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: send time expired");
	} else {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: no more packets");
	}
	for (int i=0; i < DEBUG_idx; ++i) {
		SIMNET_LOG(TIMING, DEBUG, "send packet @ " << (DEBUG_dt[i]*1000.0) << " ms");
	}

}


bool NetworkInterface::waitPending(double timeout) {
	for (int queue_idx = 0; queue_idx <= 3; ++queue_idx) {
		if (!m_RxQueues[queue_idx]->isEmpty()) return true;
		if (!m_TxQueues[queue_idx]->isEmpty()) return true;
	}
	return (m_Socket->isPendingReceive(static_cast<int>(timeout * 1000)));
}


static double DEBUG_recvtime;

int NetworkInterface::receivePackets(double timeout) {
	static StopWatch::Data swd(0.000001);
	assert(m_Initialized);

	simdata::uint8 *ptr;
	PacketReceiptHeader header;

	const uint32 allocation_size = MaxPayloadLength + HeaderSize;
	char null[MaxPayloadLength + HeaderSize];
	PacketQueue *queue;

	SIMNET_LOG(TIMING, DEBUG, "receive packets; " << (timeout * 1000.0) << " ms available");
	int DEBUG_exitcode = 0;
	int DEBUG_count = 0;

	int received_packets = 0;
	StopWatch watch(timeout, swd);
	watch.start();

	while (true) {

		if (!m_Socket->isPendingReceive(0)) {
			watch.calibrate();
			break;
		}

		simdata::uint32 peek_bytes = m_Socket->peek(&header, ReceiptHeaderSize);

		if (peek_bytes < HeaderSize) {
			// bad packet, dump it to the bit bucket
			m_Socket->receive((void*)null, sizeof(null));
			m_BadPackets++;
			continue;
		}

		if (header.destination != m_ServerId) {
			SIMNET_LOG(PACKET, ALERT, "received bad packet (wrong destination): " << header);
			m_Socket->receive((void*)null, sizeof(null));
			m_BadPackets++;
			continue;
		}

		PeerInfo *peer = 0;

		simdata::uint16 source = header.source;

		// for initial connection to the index server, source id will be zero.
		// we need to create a new id on the fly and translate the source id until
		// the sender is informed of the correct id to use.
		// TODO whenever source id is zero, routing_data will be set to the receive
		// port number, and routing type will be zero
		if (source == 0 && m_AllowUnknownPeers && header.routing_type == 0) {
			ost::InetHostAddress addr = m_Socket->getSender(0);
			simdata::uint32 ip = addr.getAddress().s_addr;
			ConnectionPoint point(ip, header.routing_data);
			source = getSourceId(point);
		}

		if (source > 0 && source < PeerIndexSize) peer = &(m_PeerIndex[source]);
		if (!peer || !peer->isActive()) {
			SIMNET_LOG(PACKET, ALERT, "received packet from unknown source: " << header);
			m_Socket->receive((void*)null, sizeof(null));
			m_BadPackets++;
			continue;
		}
		peer->getConnStat(&header);

		DEBUG_count++;

		// handle reliable udp encoding
		if (header.reliable) {
			std::cout << "RECEIVED A RELIABLE HEADER, PRI " << header.priority << ", ID0=" << header.id0 << "\n";
			if (peek_bytes >= ReceiptHeaderSize) {
				// if this packet is priority 3, it requires confirmation (id stored in id0)
				// there may also be confirmation receipts in the remaining id slots
				if (header.priority == 3) {
					peer->pushConfirmation(header.id0);
				} else {
					if (header.id0 != 0) peer->popConfirmation(header.id0);
				}
				if (header.id1 != 0) peer->popConfirmation(header.id1);
				if (header.id2 != 0) peer->popConfirmation(header.id2);
				if (header.id3 != 0) peer->popConfirmation(header.id3);
			} else {
				// truncated packet, kill it
				m_Socket->receive((void*)null, sizeof(null));
				m_BadPackets++;
				continue;
			}
		}

		if (header.message_id == PingID) {
			// we've already collected stats from the header (there is no body),
			// so just drop the packet and we're done.
			m_Socket->receive((void*)null, sizeof(null));
			continue;
		}

		int queue_idx = header.priority;
		queue = m_RxQueues[queue_idx];

		ptr = queue->getWriteBuffer(allocation_size);
		if (ptr) {
			simdata::uint32 packet_length = m_Socket->receive((void*)ptr, allocation_size);
			// rewrite the source field, in case we have assigned a new one.
			reinterpret_cast<PacketHeader*>(ptr)->source = source;

			// sanity check
			assert(packet_length >= peek_bytes);

			peer->tallyReceivedPacket(packet_length);
			m_ReceivedPackets++;
			received_packets++;

			queue->commitWriteBuffer(packet_length);

		} else {
			// this is a bad state; we have no room left to receive the incoming packet.
			// for now we just dump it; maybe we can do something smarter eventually.
			// at least reliable packets will be resent.
			simdata::uint32 packet_length = m_Socket->receive((void*)null, sizeof(null));
			peer->tallyReceivedPacket(packet_length);
			m_DroppedPackets++;
			// next packet may be destined for a queue with space left, so keep trying
		}

		if (watch.checkExpired()) {
			DEBUG_exitcode = 1;
			break;
		}
	}

	double DEBUG_elapsed = watch.elapsed();
	DEBUG_recvtime = DEBUG_elapsed;
	SIMNET_LOG(TIMING, DEBUG, "receive loop end: " << (DEBUG_elapsed * 1000.0) << " ms to queue " << DEBUG_count << " packets");
	SIMNET_LOG(TIMING, DEBUG, "receive stats: " << m_BadPackets << " bad, " << m_DroppedPackets << " dropped, " << m_ReceivedPackets << " ok)");
	if (DEBUG_exitcode == 1) {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: recv time expired");
	} else {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: no more packets");
	}

	return received_packets;
}


// processing incoming packets occurs in two steps.  first we
// call receviePackets to read raw packets from the input socket
// and store each in one of four circular buffers based on the
// message priority (2 bits in the packet header).
//
// in the remaining time slice, we read packets from these buffers
// and pass them to all the PacketHandlers that have been added with
// addPacketHandler.  the buffers are accessed using a weighted
// round-robin scheduler that pulls more packets from the higher
// priority queues.  at the end of the timeslice, we drop a fraction
// of the packets that remain in each queue.
//
// typically one of the packet handlers will be responsible for
// deserializing each message to a NetworkMessage sublass, and
// passing those along to one or more MessageHandlers.

void NetworkInterface::processIncoming(double timeout) {
	static StopWatch::Data swd(0.000001);

	SIMNET_LOG(TIMING, DEBUG, "processing incoming packets");

	double start_time = simdata::get_realtime();

	// spend up to 60% of our time slice receiving messages from the
	// socket.   if the socket runs dry early (which it should in most
	// cases), we will have extra time to process those messages.
	double receive_timeout = 0.60 * timeout;
	int received_packets = receivePackets(receive_timeout);
	received_packets += 0; // silence unused variable warning

	double receive_dt = simdata::get_realtime() - start_time;
	receive_dt = DEBUG_recvtime;  // XXX XXX remove me when debug logging is removed from receivePackets
	timeout -= receive_dt;

	SIMNET_LOG(TIMING, DEBUG, "handling packets; " << (timeout * 1000.0) << " ms available");

	StopWatch watch(timeout, swd);
	watch.start();

	int queue_idx = 3;
	int process_count[] = {4, 6, 10, 20};
	int count = process_count[queue_idx];
	PacketQueue *queue = m_RxQueues[queue_idx];
	simdata::uint32 size;

	int DEBUG_exitcode = 0;

	while (true) {

		// TODO abstract the scheduling strategy
		if (queue->isEmpty() || count <= 0) {
			int n = 0;
			do {
				// check if all queues are empty
				queue_idx = (queue_idx - 1) & 3;
				queue = m_RxQueues[queue_idx];
			} while (++n <= 4 && queue->isEmpty());
			if (n > 4) break;  // all empty
			// if only one queue isn't empty then forget about counts
			count = (n==4) ? 100000 : process_count[queue_idx];
		}
		--count;

		simdata::uint8 *ptr = queue->getReadBuffer(size);
		PacketHeader *header = reinterpret_cast<PacketHeader*>(ptr);
		simdata::uint32 header_size = (header->reliable ? ReceiptHeaderSize : HeaderSize);
		simdata::uint8 *payload = ptr + header_size;
		simdata::uint32 payload_length = size - header_size;

		// pass the packet to all handlers
		PacketHandlerCallback callback(header, payload, payload_length);
		m_PacketHandlers.apply(callback);

		queue->releaseReadBuffer();

		// keep an eye on how much time we have left
		if (watch.checkExpired()) {
			DEBUG_exitcode = 1;
			break;
		}
	}

	double DEBUG_elapsed = watch.elapsed();
	SIMNET_LOG(TIMING, DEBUG, "handling loop end: " << (DEBUG_elapsed * 1000.0) << " ms to handle packets");
	if (DEBUG_exitcode == 1) {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: send time expired");
	} else {
		SIMNET_LOG(TIMING, DEBUG, "  exit state: no more packets");
	}

	double now = simdata::get_realtime();
	double update_dt = now - m_LastUpdate;
	m_LastUpdate = now;

	// drop a fraction of the packets remaining in each queue
	int total_excess = 0;
	for (int idx = 0; idx < 4; ++idx) {
		total_excess += m_RxQueues[idx]->dropOldest();
	}

	m_ActivePeers->update(update_dt, this);
}


NetworkInterface::NetworkInterface():
	m_ServerId(0),
	m_PeerIndex(new PeerInfo[PeerIndexSize]),
	m_ActivePeers(new ActivePeerList),
	m_AllowUnknownPeers(false),
	m_Initialized(false)
{
	for (unsigned i = 0; i < PeerIndexSize; ++i) {
		m_PeerIndex[i].setId(static_cast<PeerId>(i));
	}
	resetStats();
}


NetworkInterface::~NetworkInterface() {
	// TODO use ScopedArray
	if (m_PeerIndex) delete[] m_PeerIndex;
	if (m_Initialized) {
		for (int idx = 0; idx < 4; ++idx) {
			delete m_RxQueues[idx];
			delete m_TxQueues[idx];
		}
	}
}


void NetworkInterface::resetStats() {
	m_OutputStalls = 0;
	m_SentPackets = 0;
	m_ReceivedPackets = 0;
	m_BadPackets = 0;
	m_DroppedPackets = 0;
}


void NetworkInterface::initialize(NetworkNode const &local_node) {
	assert(!m_Initialized);
	m_Initialized = true;
	m_Socket.reset(new DatagramReceiveSocket(local_node.getAddress(), local_node.getPort()));
	m_LocalNode.reset(new NetworkNode(local_node));
	double drop_percent[4] = { 0.2, 5.0, 5.0, 0.0 };
	for (int idx = 0; idx < 4; ++idx) {
		// could adjust the sizes based on actual bandwidth (e.g. smaller rx buffers for
		// slow connections).
		m_RxQueues[idx] = new PacketQueue(256000, drop_percent[idx]);
		m_TxQueues[idx] = new PacketQueue(256000, drop_percent[idx]);
	}
}


void NetworkInterface::setServerId(PeerId id) {
	m_ServerId = id;
}


void NetworkInterface::addPeer(PeerId id, NetworkNode const &remote_node, bool provisional, double incoming, double outgoing) {
	PeerInfo *peer = getPeer(id);
	assert(!peer->isActive());
	peer->setNode(remote_node, incoming, outgoing);
	peer->setProvisional(provisional);
	m_ActivePeers->addPeer(peer);
	m_IpPeerMap[remote_node.getConnectionPoint()] = id;
}


void NetworkInterface::removePeer(PeerId id) {
	PeerInfo *peer = getPeer(id);
	assert(peer->isActive());
	ConnectionPoint point = peer->getNode().getConnectionPoint();
	m_IpPeerMap.erase(point);
	peer->disable();
	m_ActivePeers->removePeer(peer);
}


void NetworkInterface::hackPeerIndex(PeerId id, NetworkNode const &remote_node, double incoming, double outgoing) {
	m_RemoteNode.reset(new NetworkNode(remote_node));
	addPeer(id, remote_node, false /*provisional*/, incoming, outgoing);
}


PeerInfo *NetworkInterface::getPeer(PeerId id) {
	assert(id > 0 && id < PeerIndexSize);
	return &(m_PeerIndex[id]);
}


void NetworkInterface::establishConnection(PeerId id, double incoming, double outgoing) {
	PeerInfo *peer = getPeer(id);
	if (peer->isActive() && peer->isProvisional()) {
		peer->updateBandwidth(incoming, outgoing);
		peer->setProvisional(false);
	}
}


void NetworkInterface::addPacketHandler(PacketHandler::Ref handler) {
	m_PacketHandlers.addHandler(handler);
	handler->bind(this);
}


void NetworkInterface::removePacketHandler(PacketHandler::Ref handler) {
	if (m_PacketHandlers.removeHandler(handler)) {
		handler->bind(0);
	}
}


void NetworkInterface::setPacketSource(PacketSource::Ref source) {
	if (m_PacketSource.valid()) m_PacketSource->bind(0);
	m_PacketSource = source;
	m_PacketSource->bind(this);
}


void NetworkInterface::setAllowUnknownPeers(bool allow) {
	m_AllowUnknownPeers = allow;
}


PeerId NetworkInterface::getSourceId(ConnectionPoint const &point) {
	IpPeerMap::iterator iter = m_IpPeerMap.find(point);
	if (iter == m_IpPeerMap.end()) {
		// unknown ip; allocate a new peer id
		for (PeerId id = 2; id < PeerIndexSize; ++id) {
			if (!m_PeerIndex[id].isActive()) {
				// TODO add a callback mechanism for validating ips?
				NetworkNode node(point);
				// restrict to a low initial bandwidth; will be reconfigured once the
				// connection is cemplete.
				addPeer(id, node, true /*provisional*/, 1024 /*inbw*/, 1024 /*outbw*/);
				return id;
			}
		}
		SIMNET_LOG(HANDSHAKE, WARNING, "all peer ids in use; rejecting connection");
		return 0;
	}
	return iter->second;
}


const simdata::uint16 NetworkInterface::PingID;
const simdata::uint32 NetworkInterface::PeerIndexSize;
const simdata::uint32 NetworkInterface::MaxPayloadLength;
const simdata::uint32 NetworkInterface::InfoSize;
const simdata::uint32 NetworkInterface::HeaderSize;
const simdata::uint32 NetworkInterface::ReceiptHeaderSize;

} // namespace simnet
