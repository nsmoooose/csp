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
 * @file NetworkMessage.h
 *
 */

#ifndef __CSPLIB_NET_NETWORKMESSAGE_H__
#define __CSPLIB_NET_NETWORKMESSAGE_H__


#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/TaggedRecord.h>

CSP_NAMESPACE


/** TaggedRecord subclass used as a base class for all network messages.
 *
 *  Real messages should subclass NetworkMessage, by specifying
 *  BASE=NetworkMessage in the tag descriptor (see lib/util for
 *  details on using TaggedRecords and the tagged record compiler).
 *
 *  By convention, tag descriptors that subclass NetworkMessage are
 *  stored in files ending with ".net".
 */
class NetworkMessage: public TaggedRecord {
	PeerId m_source;
	PeerId m_destination;
	uint8 m_routing_type;
	uint32 m_routing_data;
	unsigned char m_priority;

public:
	/** Default constructor.  Initializes to a non-reliable, low-priority
	 *  message.
	 */
	NetworkMessage(): m_source(0), m_destination(0), m_routing_type(0), m_routing_data(0), m_priority(0) { }

	/** Get the peer id of the destination host.
	 */
	inline PeerId getDestination() const { return m_destination; }

	/** Set the destination of this message.
	 *
	 *  @param destination the peer id of the destination host.
	 */
	inline void setDestination(PeerId destination) { m_destination = destination; }

	/** Set the message routing data.  Interpretation of this value
	 *  depends on the message routing type.
	 */
	inline void setRoutingData(uint32 data) {
		assert((data & 0xFF00000) == 0);
		m_routing_data = data;
	}

	/** Get the message routing data.  Interpretation of this value
	 *  depends on the message routing type.
	 */
	inline uint32 getRoutingData() const { return m_routing_data; }

	/** Store the id of the peer that sent the message.
	 */
	inline void setSource(PeerId source) {
		m_source = source;
	}

	/** Get the id of the peer that sent the message.
	 */
	inline PeerId getSource() const { return m_source; }

	/** Set the message routing type.  Routing types are used to determine
	 *  how to handle incoming messages.  For example, if the routing type
	 *  corresponds to an object message, the routing data will contain the
	 *  id of the object to receive the message.  This allow the message to
	 *  be dispatched to the appropriate object without requiring the
	 *  routing layer to understand the details of particular message types.
	 */
	inline void setRoutingType(RoutingType type) {
		m_routing_type = type;
	}

	/** Get the message routing type.  See setRoutingType for details.
	 */
	inline RoutingType getRoutingType() const { return m_routing_type; }

	/** Set the message to be "reliable."  Reliable messages require a confirmation
	 *  of receipt from the destination host.  The message will be resent periodically
	 *  until confirmation in received (or the peer is dropped).  Reliable messages
	 *  have the highest priority, and should have the greatest chance of reaching
	 *  the distination on the first try.  If not, the retry latency starts at one
	 *  second and grows with each retry.
	 */
	inline void setReliable() { m_priority = 3; }

	/** Test if this message is marked as "reliable;" see setReliable for details.
	 */
	inline bool isReliable() const { return m_priority == 3; }

	/** Set the priority of this message.  There are four priority levels, interpreted
	 *  roughly as follows:
	 *
	 *   @li  0 : low priority, non-realtime
	 *   @li  1 : low priority, realtime
	 *   @li  2 : realtime
	 *   @li  3 : high priority, reliable
	 *
	 *  Setting the priority to 3 automatically selects reliable transmission.
	 */
	inline void setPriority(unsigned int priority) {
		assert(priority <= 3);
		m_priority = static_cast<unsigned char>(priority);
	}

	/** Get the priority of this message.  See setPriority for details.
	 */
	inline int getPriority() const { return m_priority; }

	/** Reimplementation of TaggedRecord::FastCast with specialization for casts
	 *  to NetworkMessage (which doesn't define _getId).
	 */
	template <class TR>
	static inline Ref<TR> FastCast(Ref<NetworkMessage> const &record) {
		return (record->getId() == TR::_getId() ? static_cast<TR*>(record.get()) : 0);
	}

	/** Address this message to the source of another message.
	 */
	inline void setReplyTo(Ref<NetworkMessage> const &msg) {
		setDestination(msg->getSource());
	}

};


/** Specialized FastCast to/from NetworkMessage (nop).
 */
template <>
inline Ref<NetworkMessage> NetworkMessage::FastCast(Ref<NetworkMessage> const &record) { return record; }


CSP_NAMESPACE_END

#endif // __CSPLIB_NET_NETWORKMESSAGE_H__

