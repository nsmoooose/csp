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
 * @file PacketHandler.h
 *
 */

#ifndef __SIMNET_PACKETHANDLER_H__
#define __SIMNET_PACKETHANDLER_H__

#include <SimNet/NetBase.h>
#include <SimData/Ref.h>

namespace simnet {

class PacketHandler: public simdata::Referenced {
	friend class NetworkInterface;
	NetworkInterface *m_network_interface;
	virtual void bind(NetworkInterface* interface) { m_network_interface = interface; }
protected:
	inline NetworkInterface *getNetworkInterface() { return m_network_interface; }
public:
	typedef simdata::Ref<PacketHandler> Ref;
	virtual ~PacketHandler() { }
	virtual bool handlePacket(PacketHeader const *, simdata::uint8 *, simdata::uint32)=0;
};

} // namespace simnet

#endif // __SIMNET_PACKETHANDLER_H__

