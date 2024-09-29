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
 * @file PacketHandler.h
 *
 */

#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/util/Ref.h>

namespace csp {

/** Abstract interface for handling incoming packets.
 *
 *  Subclasses must implement handlePacket().  PacketHandlers must be registered
 *  with a NetworkInterface.  All registered handlers are called to handle each
 *  incoming packet.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT PacketHandler: public Referenced {
	friend class NetworkInterface;

	// TODO(os)
	// NetworkInterface calls bind(this) to give PacketHandler subclasses direct
	// access to the associated NI.  This is not currently used, and should be
	// removed unless a compelling need is found.
	
	NetworkInterface *m_network_interface;
	virtual void bind(NetworkInterface* network_interface) { m_network_interface = network_interface; }

protected:
	inline NetworkInterface *getNetworkInterface() { return m_network_interface; }

public:
	/** Destructor.
	 */
	virtual ~PacketHandler() { }

	/** Implement this method to process incoming packets.
	 *
	 *  @param header a pointer to the packet header.
	 *  @param payload a buffer containing the payload data.
	 *  @param payload_length the size of the payload, in bytes.
	 */
	virtual void handlePacket(PacketHeader const *header, uint8 *payload, uint32 payload_length)=0;
};

} // namespace csp
