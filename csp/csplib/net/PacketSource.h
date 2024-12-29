#pragma once
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
 * @file PacketSource.h
 *
 */

#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/util/Ref.h>

namespace csp {

class NetworkInterface;

/** Abstract interface for classes that generate packets to be sent
 *  over the network.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT PacketSource: public Referenced {
	friend class NetworkInterface;

	// TODO(os)
	// NetworkInterface calls bind(this) to give PacketSource subclasses direct
	// access to the associated NI.  This is not currently used, and should be
	// removed unless a compelling need is found.
	
	NetworkInterface *m_network_interface;
	virtual void bind(NetworkInterface* network_interface) { m_network_interface = network_interface; }

protected:
	inline NetworkInterface *getNetworkInterface() { return m_network_interface; }

public:
	virtual ~PacketSource() { }

	/** Retrieve the next packet.
	 *
	 *  @param header a packet header; the implementation needs to set the destination, priority,
	 *    message_id, routing_type, and routing_data fields.
	 *  @param payload a buffer to receive the raw message data (excluding the header).
	 *  @param payload_length when called, the amount of space allocated for payload (in bytes);
	 *    on return the implementation must return the actual number of bytes used.
	 *  @return true if a packet was returned, false otherwise.
	 */
	virtual bool getPacket(PacketHeader *header, uint8_t *payload, uint32_t &payload_length)=0;

	/** Test if the source has packets available.
	 */
	virtual bool isEmpty()=0;

	/** Return the number of packets available.
	 */
	virtual int size()=0;

	/** Test the destination and priority of the next packet without retrieving it.
	 *
	 *  @param destination the peer to send the next packet to.
	 *  @param priority the priority of the next packet (0-3).
	 *  @return false if no packets are availible.
	 */
	virtual bool peek(PeerId &destination, int &priority)=0;

	/** Drop the next packet.
	 */
	virtual void skipPacket()=0;
};

} // namespace csp
