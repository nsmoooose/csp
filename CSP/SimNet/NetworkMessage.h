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
 * @file NetworkMessage.h
 *
 */

#ifndef __SIMNET_NETWORKMESSAGE_H__
#define __SIMNET_NETWORKMESSAGE_H__


#include <SimNet/NetBase.h>
#include <SimData/TaggedRecord.h>

namespace simnet {


/** TaggedRecord subclass used as a base class for all network messages.
 *
 *  Real messages should subclass NetworkMessage, by specifying
 *  BASE=simnet::NetworkMessage in the tag descriptor (see SimData for
 *  details on using TaggedRecords and the tagged record compiler).
 *
 *  By convention, tag descriptors that subclass NetworkMessage are
 *  stored in files ending with ".net".
 */
class NetworkMessage: public simdata::TaggedRecord {
	PeerId m_destination;
	unsigned char m_priority;

public:
	typedef simdata::Ref<NetworkMessage> Ref;

	/** Default constructor.  Initializes to a non-reliable, low-priority
	 *  message.
	 */
	NetworkMessage(): m_destination(0), m_priority(0) { }

	/** Get the peer id of the destination host.
	 */
	PeerId getDestination() const { return m_destination; }

	/** Set the destination of this message.
	 *
	 *  @param destination the peer id of the destination host.
	 */
	void setDestination(PeerId destination) { m_destination = destination; }

	/** Set the message to be "reliable."  Reliable messages require a confirmation
	 *  of receipt from the destination host.  The message will be resent periodically
	 *  until confirmation in received (or the peer is dropped).  Reliable messages
	 *  have the highest priority, and should have the greatest chance of reaching
	 *  the distination on the first try.  If not, the retry latency starts at one
	 *  second and grows with each retry.
	 */
	void setReliable() { m_priority = 3; }

	/** Test if this message is marked as "reliable;" see setReliable for details.
	 */
	bool isReliable() const { return m_priority == 3; }

	/** Set the priority of this message.  There are four priority levels, interpreted
	 *  roughly as follows:
	 *    0 : low priority, non-realtime
	 *    1 : low priority, realtime
	 *    2 : realtime
	 *    3 : high priority, reliable
	 *  Setting the priority to 3 automatically selects reliable transmission.
	 */
	void setPriority(unsigned int priority) {
		assert(priority <= 3);
		m_priority = static_cast<unsigned char>(priority);
	}

	/** Get the priority of this message.  See setPriority for details.
	 */
	int getPriority() const { return m_priority; }
};


} // namespace simnet

#endif // __SIMNET_NETWORKMESSAGE_H__

