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
 * @file MessageQueue.h
 *
 */

#ifndef __SIMNET_MESSAGEQUEUE_H__
#define __SIMNET_MESSAGEQUEUE_H__

#include <SimNet/PacketSource.h>
#include <SimNet/NetworkMessage.h>
#include <SimData/TaggedRecordRegistry.h>
#include <deque>

namespace simnet {

class MessageQueue: public PacketSource {
	std::deque<NetworkMessage::Ref> m_Queue;
	simdata::BufferWriter m_Writer;
	simdata::TagWriter m_TagWriter;
public:
	MessageQueue(): m_TagWriter(m_Writer) { }

	inline void queueMessage(NetworkMessage::Ref msg) {
		m_Queue.push_back(msg);
	}

	virtual int size() { return m_Queue.size(); }

	virtual bool isEmpty() { return m_Queue.size() == 0; }

	virtual bool getPacket(PacketHeader *header, simdata::uint8 *payload, simdata::uint32 &payload_length) {
		if (m_Queue.size() == 0) return false;
		assert(payload_length > 0);
		NetworkMessage::Ref message = m_Queue.front();
		m_Queue.pop_front();
		PeerId destination = message->getDestination();
		header->destination = destination;
		header->message_id = message->getCustomId();
		m_Writer.bind(payload, payload_length);
		// XXX need to catch overflows
		message->serialize(m_TagWriter);
		payload_length = m_Writer.length();
		return true;
	}

	virtual bool peekPriority(PeerId &id, int &priority) {
		if (m_Queue.size() == 0) return false;
		id = m_Queue.front()->getDestination();
		priority = m_Queue.front()->getPriority();
		return true;
	}

	virtual void skipPacket() {
		if (m_Queue.size() > 0) m_Queue.pop_front();
	}
};

} // namespace simnet

#endif // __SIMNET_MESSAGEQUEUE_H__
