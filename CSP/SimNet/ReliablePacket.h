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
 * @file ReliablePacket.h
 *
 */

#ifndef __SIMNET_RELIABLEPACKET_H__
#define __SIMNET_RELIABLEPACKET_H__

#include <SimNet/NetBase.h>
#include <SimData/Ref.h>
#include <SimData/Timing.h>

#include <string>
#include <algorithm>


namespace simnet {

class ReliablePacket: public simdata::Referenced {
	ConfirmationId m_Id;
	bool m_Confirmed;
	int m_Attempts;
	double m_NextTime;
	std::string m_PacketData;
public:
	typedef simdata::Ref<ReliablePacket> Ref;
	ReliablePacket(): m_Id(0), m_Confirmed(true), m_Attempts(0), m_NextTime(0.0) { }
	virtual ~ReliablePacket() {
		std::cout << "RELIABLE PACKET " << m_Id << ": DESTROYED\n";
	}
	void assign(ConfirmationId id, PacketReceiptHeader* receipt, simdata::uint32 packet_size) {
		std::cout << "RELIABLE PACKET " << id << "\n";
		m_Id = id;
		m_Confirmed = false;
		m_Attempts = 0;
		m_NextTime = simdata::get_realtime() + 1.0;
		m_PacketData.assign(reinterpret_cast<char*>(receipt), packet_size);
	}
	void updateAttempt(double now) {
		std::cout << "RELIABLE PACKET " << m_Id << ": RETRY " << (m_Attempts+1) << "\n";
		double delay = ++m_Attempts * 1.0;
		m_NextTime = now + std::min(8.0, delay);
	}
	inline void copy(simdata::uint8 *ptr) {
		std::cout << "RELIABLE PACKET " << m_Id << ": SENDING\n";
		memcpy(ptr, m_PacketData.data(), m_PacketData.size());
	}
	inline simdata::uint32 size() const { return m_PacketData.size(); }
	inline ConfirmationId getId() const { return m_Id; }
	inline double nextTime() { return m_NextTime; }
	inline bool isConfirmed() const { return m_Confirmed; }
	inline void confirm() {
		std::cout << "RELIABLE PACKET " << m_Id << ": CONFIRMED\n";
		m_Confirmed = true;
	}
	struct Order : public std::binary_function<Ref, Ref, bool> {
		inline bool operator()(const Ref& __x, const Ref& __y) const { return __x->nextTime() > __y->nextTime(); }
	};
};

} // namespace simnet

#endif // __SIMNET_RELIABLEPACKET_H__

