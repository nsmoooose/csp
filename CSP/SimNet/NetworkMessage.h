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

class NetworkMessage: public simdata::TaggedRecord {
	PeerId m_destination;
	bool m_reliable;
	unsigned char m_priority;
public:
	NetworkMessage(): m_destination(0), m_reliable(false), m_priority(0) { }
	typedef simdata::Ref<NetworkMessage> Ref;
	PeerId getDestination() const { return m_destination; }
	void setDestination(PeerId destination) { m_destination = destination; }
	void setReliable() { m_reliable = true; }
	bool isReliable() const { return m_reliable; }
	void setPriority(int priority) { m_priority = static_cast<unsigned char>(priority); }
	int getPriority() const { return m_priority; }
};

} // namespace simnet

#endif // __SIMNET_NETWORKMESSAGE_H__

