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

#include <SimData/Uniform.h>
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
struct PacketHeader {
	simdata::uint16 reliable:1;      // if true, header is actually a PacketReceiptHeader
	simdata::uint16 reserved:2;      // reserved for future use
	simdata::uint16 priority:2;      // 0 = non-realtime, 1=realtime, lopri, 2=realtime, hipri, 3=reliable
	simdata::uint16 statmode:1;      // 0 = desired send rate, 1 = fractional allocation
	simdata::uint16 connstat:10;     // if statmode == 0, the desired send rate as a fraction of the
	                                 // nominal receiver bandwidth: C * BW / 100
	                                 // if statmode == 1, the fraction of the sender's inbound bandwidth
	                                 // allocated to the receiving peer (0->BW)
	simdata::uint16 source;          // id of the sender
	simdata::uint16 destination;     // id of the intended receiver
	simdata::uint16 message_id;      // id of the message
	simdata::uint32 routing_type:8;  // routing type (e.g. object update, server handshake, etc.)
	simdata::uint32 routing_data:24; // routing data (e.g. ObjectId for object update routing)
};


/** Used in place of PacketHeader when reliable is set to 1.  If
 *  priority is 3, id0 will be the confirmation id for this packet.
 *  All other non-zero ids are confirmations of past messages that
 *  have been successfully received from the destination host.
 */
struct PacketReceiptHeader: public PacketHeader {
	ConfirmationId id0;
	ConfirmationId id1;
	ConfirmationId id2;
	ConfirmationId id3;
};


/** Helper class for debugging.  Dumps a packet header to an output stream.
 */
inline std::ostream &operator <<(std::ostream &os, PacketHeader const &header) {
	return os << (header.reliable ? 'R' : 'U') << header.priority << ':' << header.statmode
	          << "*" << header.connstat << ':' << header.source << '>' << header.destination
	          << ':' << header.message_id;
}


/** Helper class for debugging.  Dumps a packet receipt header to an output stream.
 */
inline std::ostream &operator <<(std::ostream &os, PacketReceiptHeader const &header) {
	return os << reinterpret_cast<PacketHeader const &>(header) << "<" << header.id0 << ","
	          << header.id1 << "," << header.id2 << "," << header.id3;
}


} // namespace simnet

#endif // __SIMNET_NETBASE_H__

