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

	/** Thin wrapper for outbound messages, which keeps a separate record of the
	 *  destination.  This is necessary to allow broadcasting a single message to
	 *  multiple destinations.
	 */
	struct MessageWrapper {
		MessageWrapper(NetworkMessage::Ref const &msg, PeerId dst): message(msg), destination(dst) { }
		MessageWrapper(NetworkMessage::Ref const &msg): message(msg), destination(msg->getDestination()) { }
		NetworkMessage::Ref message;
		PeerId destination;
	};

	/** Internal FIFO queue of outbound messages.
	 */
	std::deque<MessageWrapper> m_Queue;

	simdata::BufferWriter m_Writer;
	simdata::TagWriter m_TagWriter;

	/** When broadcasting a message to multiple destinations, we serialize the
	 *  message once to this cache, and then copy the raw payload data for each
	 *  packet request.  To take advantage of caching, a message must be queued
	 *  sequentially to each destination (other interleaved messages invalidate
	 *  the cache).
	 *
	 *  The cache is currently limited to 4KB, which should be sufficient for
	 *  broadcast messages.
	 */
	char m_PayloadCache[4096];
	simdata::uint32 m_PayloadCacheLength;

public:

	typedef simdata::Ref<MessageQueue> Ref;

	/** Create a new message queue.
	 */
	MessageQueue(): m_TagWriter(m_Writer), m_PayloadCacheLength(0) { }

	/** Add a message to the queue.
	 */
	inline void queueMessage(NetworkMessage::Ref msg) {
		// messsage id 0 is reserved for "unknown" messages, which should never be sent.
		if (msg->getCustomId() > 0) {
			m_Queue.push_back(MessageWrapper(msg));
		} else {
			SIMNET_LOG(PACKET, ERROR, "attempt to send message id=0");
		}
	}

	/** Add a message to the queue.
	 */
	inline void queueMessage(NetworkMessage::Ref msg, PeerId destination) {
		// messsage id 0 is reserved for "unknown" messages, which should never be sent.
		if (msg->getCustomId() > 0) {
			m_Queue.push_back(MessageWrapper(msg, destination));
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
		if (m_Queue.empty()) return false;
		assert(payload_length > 0);
		NetworkMessage::Ref message = m_Queue.front().message;
		PeerId destination = m_Queue.front().destination;
		m_Queue.pop_front();
		SIMNET_LOG(MESSAGE, ALERT, "SENDING MESSAGE " << message->getCustomId() << " " << message->getName());
		header->destination = destination;
		header->message_id = static_cast<simdata::uint16>(message->getCustomId());
		header->routing_type = message->getRoutingType();
		header->routing_data = message->getRoutingData();

		// if the cache isn't empty it will already contain the payload for this
		// message, so copy that directly rather than reserializing the message.
		if (m_PayloadCacheLength > 0) {
			if (m_PayloadCacheLength <= payload_length) {
				memcpy(payload, m_PayloadCache, m_PayloadCacheLength);
				payload_length = m_PayloadCacheLength;
				SIMNET_LOG(MESSAGE, ALERT, "SENDING CACHED PAYLOAD, length = " << payload_length);
			} else {
				SIMNET_LOG(MESSAGE, WARNING, "buffer overflow sending " << message->getName());
			}
		} else {
			// serialize directly into the target buffer
			m_Writer.bind(payload, payload_length);
			// TODO need to catch overflows
			message->serialize(m_TagWriter);
			payload_length = m_Writer.length();
			// invalidate the cache
			m_PayloadCacheLength = 0;
		}

		// if the next message is the same as the current message (ie. it is bound for multiple
		// destinations), cache the payload
		if (!m_Queue.empty() && (m_Queue.front().message == message)) {
			if ((m_PayloadCacheLength == 0) && (payload_length <= sizeof(m_PayloadCache))) {
				memcpy(m_PayloadCache, payload, payload_length);
				m_PayloadCacheLength = payload_length;
			}
		} else {
			// invalidate the cache
			m_PayloadCacheLength = 0;
		}

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
		destination = m_Queue.front().destination;
		priority = m_Queue.front().message->getPriority();
		return true;
	}

	/** Drop the next message (if any) from the queue.
	 */
	virtual void skipPacket() {
		if (m_Queue.size() > 0) {
			NetworkMessage::Ref message = m_Queue.front().message;
			m_Queue.pop_front();
			// if we had a cached payload for the message that was skipped, and the
			// next message is different we need to invalidate the cache.
			if (m_PayloadCacheLength > 0) {
				if (m_Queue.empty() || (m_Queue.front().message != message)) {
					// invalidate the cache
					m_PayloadCacheLength = 0;
				}
			}
		}
	}
};

} // namespace simnet

#endif // __SIMNET_MESSAGEQUEUE_H__

