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
 * @file MessageQueue.h
 *
 */

#ifndef __SIMNET_MESSAGEQUEUE_H__
#define __SIMNET_MESSAGEQUEUE_H__

#include <SimNet/PacketSource.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/NetLog.h>
#include <SimData/TaggedRecordRegistry.h>
#include <deque>

namespace simnet {

/** A packet source that encodes NetworkMessages to binary packets.
 */
class MessageQueue: public PacketSource {
	std::deque<NetworkMessage::Ref> m_Queue;
	simdata::BufferWriter m_Writer;
	simdata::TagWriter m_TagWriter;

public:

	typedef simdata::Ref<MessageQueue> Ref;

	/** Create a new message queue.
	 */
	MessageQueue(): m_TagWriter(m_Writer) { }

	/** Add a message to the queue.
	 */
	inline void queueMessage(NetworkMessage::Ref msg) {
		// messsage id 0 is reserved for "unknown" messages, which should never be sent.
		if (msg->getCustomId() > 0) {
			m_Queue.push_back(msg);
		} else {
			SIMNET_LOG(PACKET, ERROR, "attempt to send message id=0");
		}
	}

	/** Get the number of pending message in the queue.
	 */
	virtual int size() { return m_Queue.size(); }

	/** Test if the queue is empty.
	 */
	virtual bool isEmpty() { return m_Queue.size() == 0; }

	/** Encode the next message in the queue.
	 */
	virtual bool getPacket(PacketHeader *header, simdata::uint8 *payload, simdata::uint32 &payload_length) {
		if (m_Queue.size() == 0) return false;
		assert(payload_length > 0);
		NetworkMessage::Ref message = m_Queue.front();
		m_Queue.pop_front();
		PeerId destination = message->getDestination();
		header->destination = destination;
		header->message_id = message->getCustomId();
		header->routing_type = message->getRoutingType();
		header->routing_data = message->getRoutingData();
		m_Writer.bind(payload, payload_length);
		// XXX need to catch overflows
		message->serialize(m_TagWriter);
		payload_length = m_Writer.length();
		return true;
	}

	/** Test the destination and priority of the next message without retrieving
	 *  (or encoding) it.
	 *
	 *  @param destination the peer to send the next packet to.
	 *  @param priority the priority of the next packet (0-3).
	 *  @return false if no packets are availible.
	 */
	virtual bool peek(PeerId &destination, int &priority) {
		if (m_Queue.size() == 0) return false;
		destination = m_Queue.front()->getDestination();
		priority = m_Queue.front()->getPriority();
		return true;
	}

	/** Drop the next message (if any) from the queue.
	 */
	virtual void skipPacket() {
		if (m_Queue.size() > 0) m_Queue.pop_front();
	}
};

} // namespace simnet

#endif // __SIMNET_MESSAGEQUEUE_H__

