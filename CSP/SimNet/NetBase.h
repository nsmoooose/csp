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
 * @file NetBase.h
 *
 */

#ifndef __SIMNET_NETBASE_H__
#define __SIMNET_NETBASE_H__

#include <SimData/Uniform.h>
#include <ostream>

namespace simnet {

typedef simdata::uint16 PeerId;
typedef simdata::uint16 MessageId;
typedef simdata::uint32 Bandwidth;
typedef simdata::uint16 ConfirmationId;


struct PacketHeader {
	simdata::uint16 reliable:1;  // if true, header is actually a PacketReceiptHeader
	simdata::uint16 reserved:2;  // reserved for future use
	simdata::uint16 priority:2;  // 0 = non-realtime, 1=realtime, lopri, 2=realtime, hipri, 3=reliable
	simdata::uint16 statmode:1;  // 0 = desired send rate, 1 = fractional allocation
	simdata::uint16 connstat:10; // if statmode == 0, the desired send rate as a fraction of the
	                             // nominal receiver bandwidth: C * BW / 100
	                             // if statmode == 1, the fraction of the sender's inbound bandwidth
	                             // allocated to the receiving peer (0->BW)
	simdata::uint16 source;      // id of the sender
	simdata::uint16 destination; // id of the intended receiver
	simdata::uint16 message_id;  // id of the message
};

// used in place of PacketHeader when reliable is true
// if priority==3, id0 will be the confirmation id for this packet.
// all other non-zero ids are confirmations of past messages.
struct PacketReceiptHeader: public PacketHeader {
	ConfirmationId id0;
	ConfirmationId id1;
	ConfirmationId id2;
	ConfirmationId id3;
};

inline std::ostream &operator <<(std::ostream &os, PacketHeader const &header) {
	os << (header.reliable ? 'R' : 'U') << header.priority << ':' << header.statmode << "*" << header.connstat << ':'
	   << header.source << '>' << header.destination << ':' << header.message_id;
}

} // namespace simnet

#endif // __SIMNET_NETBASE_H__

