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
 * @file NetBase.h
 *
 */

#ifndef __SIMNET_NETBASE_H__
#define __SIMNET_NETBASE_H__

#include <SimData/Endian.h>
#include <SimData/Uniform.h>
#include <cassert>
#include <ostream>


namespace simnet {

/** UDP/TCP port number.
 */
typedef simdata::uint16 Port;

/** Peer id, used as an index to the PeerInfo table.
 */
typedef simdata::uint16 PeerId;

/** Network message id, used as an index to the NetworkMessage table
 *  and cached in the custom id field of each NetworkMessage subclass.
 */
typedef simdata::uint16 MessageId;

/** A unique object id.  (24-bits; 16 million ids)
 */
typedef simdata::uint32 ObjectId;

/** Routing type, which is typically used as the first discriminator
 *  for message dispatch.
 */
typedef simdata::uint8 RoutingType;

/** An id number assigned to reliable messages and used to confirm
 *  receipt of such messages.  Ids are generated sequentially, per
 *  peer, and roll over at 65535.  Id 0 is reserved for "no id".
 */
typedef simdata::uint16 ConfirmationId;

/** Shorthand for NetworkNode for use as a std::map key type.
 */
typedef std::pair<simdata::uint32, Port> ConnectionPoint;

/** Header attached to all transmitted network packets.  This is the
 *  short form of the header, used for unreliable transmissions.  If
 *  reliable is set to 1, the longer PacketReceiptHeader is used.
 *  The latter subclasses PacketHeader, providing the same fields
 *  (at the same offsets) as well as extra fields for implementing
 *  reliable udp.
 */
#pragma pack(push, 1)
class PacketHeader {

	// NOTE: all integer fields are stored and transmitted with little-endian byte order.
	// Yes, that isn't network byte order.  Deal.

	// flags bits:
	//     0  reliable: if true, header is actually a PacketReceiptHeader
	//   1-2  priority: 0=non-realtime, 1=realtime, lopri, 2=realtime, hipri, 3=reliable
	//   3-4  reserved for future use
	//     5  statmode: 0=desired send rate, 1=fractional allocation
	//  6-15  connstat: if statmode == 0, the desired send rate as a fraction of the
	//                  nominal receiver bandwidth: C * BW / 100
	//                  if statmode == 1, the fraction of the sender's inbound bandwidth
	//                  allocated to the receiving peer (0->BW)
	simdata::uint16 _flags;
	simdata::uint16 _source;        // id of the sender
	simdata::uint16 _destination;   // id of the intended receiver
	simdata::uint16 _message_id;    // id of the message
	simdata::uint32 _routing;       // routing type (e.g. object update, server handshake, etc.)
	                                // is stored in the low 8 bits; routing data (e.g. ObjectId
	                                // for object update routing) is stored in the high 24 bits.

public:

	inline bool reliable() const { return (SIMDATA_UINT16_FROM_LE(_flags) & 1) != 0; }
	inline int priority() const { return (SIMDATA_UINT16_FROM_LE(_flags) >> 1) & 3; }
	inline int statmode() const { return (SIMDATA_UINT16_FROM_LE(_flags) >> 5) & 1; }
	inline int connstat() const { return (SIMDATA_UINT16_FROM_LE(_flags) >> 6); }

	inline void setReliable(bool reliable) {
		simdata::uint16 f = SIMDATA_UINT16_FROM_LE(_flags);
		if (reliable) f |= 1; else f &= ~1;
		_flags = SIMDATA_UINT16_TO_LE(f);
	}
	inline void setPriority(int priority) {
		simdata::uint16 f = SIMDATA_UINT16_FROM_LE(_flags);
		f = (f & ~0x06) | static_cast<simdata::uint16>((priority & 3) << 1);
		_flags = SIMDATA_UINT16_TO_LE(f);
	}
	inline void setStatMode(int statmode) {
		simdata::uint16 f = SIMDATA_UINT16_FROM_LE(_flags);
		f = (f & ~0x20) | static_cast<simdata::uint16>((statmode & 1) << 5);
		_flags = SIMDATA_UINT16_TO_LE(f);
	}
	inline void setConnStat(int connstat) {
		simdata::uint16 f = SIMDATA_UINT16_FROM_LE(_flags);
		f = (f & 0x3f) | static_cast<simdata::uint16>(connstat << 6);
		_flags = SIMDATA_UINT16_TO_LE(f);
	}

	inline simdata::uint16 source() const { return SIMDATA_UINT16_FROM_LE(_source); }
	inline simdata::uint16 destination() const { return SIMDATA_UINT16_FROM_LE(_destination); }
	inline simdata::uint16 messageId() const { return SIMDATA_UINT16_FROM_LE(_message_id); }
	inline simdata::uint32 routingType() const { return SIMDATA_UINT32_FROM_LE(_routing) & 0xff; }
	inline simdata::uint32 routingData() const { return SIMDATA_UINT32_FROM_LE(_routing) >> 8; }

	inline void setSource(PeerId source) {
		_source = SIMDATA_UINT16_TO_LE(source);
	}

	inline void setDestination(PeerId destination) {
		_destination = SIMDATA_UINT16_TO_LE(destination);
	}

	inline void setMessageId(simdata::uint16 message_id) {
		_message_id = SIMDATA_UINT16_TO_LE(message_id);
	}

	inline void setRouting(simdata::uint32 routing_type, simdata::uint32 routing_data) {
		assert(routing_type < 0x100);
		assert(routing_data < 0x1000000);
		routing_data = (routing_data << 8) | routing_type;
		_routing = SIMDATA_UINT32_TO_LE(routing_data);
	}

};
#pragma pack(pop)


/** Used in place of PacketHeader when reliable is set to 1.  If
 *  priority is 3, id0 will be the confirmation id for this packet.
 *  All other non-zero ids are confirmations of past messages that
 *  have been successfully received from the destination host.
 */
#pragma pack(push, 1)
class PacketReceiptHeader: public PacketHeader {
	simdata::uint16 _id0;
	simdata::uint16 _id1;
	simdata::uint16 _id2;
	simdata::uint16 _id3;
public:
	inline ConfirmationId id0() const { return SIMDATA_UINT16_FROM_LE(_id0); }
	inline ConfirmationId id1() const { return SIMDATA_UINT16_FROM_LE(_id1); }
	inline ConfirmationId id2() const { return SIMDATA_UINT16_FROM_LE(_id2); }
	inline ConfirmationId id3() const { return SIMDATA_UINT16_FROM_LE(_id3); }
	inline void setId0(ConfirmationId id) { _id0 = SIMDATA_UINT16_TO_LE(id); }
	inline void setId1(ConfirmationId id) { _id1 = SIMDATA_UINT16_TO_LE(id); }
	inline void setId2(ConfirmationId id) { _id2 = SIMDATA_UINT16_TO_LE(id); }
	inline void setId3(ConfirmationId id) { _id3 = SIMDATA_UINT16_TO_LE(id); }
};
#pragma pack(pop)


/** Helper class for debugging.  Dumps a packet header to an output stream.
 */
inline std::ostream &operator <<(std::ostream &os, PacketHeader const &header) {
	return os << (header.reliable() ? 'R' : 'U') << header.priority() << ':' << header.statmode()
	          << "*" << header.connstat() << ':' << header.source() << '>' << header.destination()
	          << ':' << header.messageId() << ":"
	          << header.routingType() << "-" << header.routingData();
}


/** Helper class for debugging.  Dumps a packet receipt header to an output stream.
 */
inline std::ostream &operator <<(std::ostream &os, PacketReceiptHeader const &header) {
	if (header.reliable()) {
		return os << reinterpret_cast<PacketHeader const &>(header) << "#" << header.id0() << ","
		          << header.id1() << "," << header.id2() << "," << header.id3();
	} else {
		return os << reinterpret_cast<PacketHeader const &>(header);
	}
}


} // namespace simnet

#endif // __SIMNET_NETBASE_H__

