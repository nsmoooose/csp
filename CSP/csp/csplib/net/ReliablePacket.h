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
 * @file ReliablePacket.h
 *
 */

#ifndef __CSPLIB_NET_RELIABLEPACKET_H__
#define __CSPLIB_NET_RELIABLEPACKET_H__

#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/NetLog.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Timing.h>

#include <string>
#include <algorithm>


CSP_NAMESPACE

/** Storage class used by PeerInfo to keep track of reliable packets
 *  until confirmation in received.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT ReliablePacket: public Referenced {
	ConfirmationId m_Id;
	bool m_Confirmed;
	int m_Attempts;
	double m_NextTime;
	std::string m_PacketData;

public:
	typedef Ref<ReliablePacket> Ref;

	/** Construct an empty instance; use assign to initialize it.
	 */
	ReliablePacket(): m_Id(0), m_Confirmed(true), m_Attempts(0), m_NextTime(0.0) { }

	/** Destructor (releases packet buffer).
	 */
	virtual ~ReliablePacket() {
		SIMNET_LOG(DEBUG, PACKET) << "reliable packet " << m_Id << ": destroyed";
	}

	/** Iniitalize this instance.
	 *
	 *  Resets the retry attempt count to zero, marks the packet as unconfirmed,
	 *  and stores a copy of the packet buffer for later retransmission.
	 *
	 *  @param id the id number of the packet needing confirmation.
	 *  @param receipt a pointer to the packet header (and payload data).
	 *  @param packet_size the full packet size in bytes (header + payload).
	 */
	void assign(ConfirmationId id, PacketReceiptHeader* receipt, uint32 packet_size) {
		SIMNET_LOG(DEBUG, PACKET) << "creating reliable packet " << id;
		m_Id = id;
		m_Confirmed = false;
		m_Attempts = 0;
		m_NextTime = get_realtime() + 1.0;
		m_PacketData.assign(reinterpret_cast<char*>(receipt), packet_size);
	}

	/** Record an attempted retransmission of the packet.
	 *
	 *  Increments the retry attempt count and schedules the next retry attempt.
	 *
	 *  @param now the current system time (in seconds).
	 */
	void updateAttempt(double now) {
		double delay = std::min(8.0, ++m_Attempts * 1.0);
		m_NextTime = now + delay;
		SIMNET_LOG(DEBUG, PACKET) << "reliable packet " << m_Id << " retry #" << m_Attempts << "; next retry in " << delay << " s";
	}

	/** Copy the stored packet data (header + payload) to a buffer.
	 *
	 *  It is the callers responsibility to ensure that ptr is valid and has
	 *  sufficient space to store the packet data (see size()).
	 *
	 *  @param ptr the buffer to receive the packet data.
	 */
	inline void copy(uint8 *ptr) {
		memcpy(ptr, m_PacketData.data(), m_PacketData.size());
	}
	
	/** Get the size of the packet data in bytes (header + payload)
	 */
	inline uint32 size() const { return m_PacketData.size(); }

	/** Get the reliable packet confirmation id.
	 */
	inline ConfirmationId getId() const { return m_Id; }

	/** Get the system time of the next retry attempt.
	 */
	inline double nextTime() { return m_NextTime; }

	/** Test if the remote peer has already confirmed receipt of this packet.
	 */
	inline bool isConfirmed() const { return m_Confirmed; }

	/** Mark this packet as confirmed.
	 */
	inline void confirm() {
		SIMNET_LOG(DEBUG, PACKET) << "reliable packet " << m_Id << " confirmed";
		m_Confirmed = true;
	}

	/** Functor used to order reliable packets in a priority queue by the
	 *  scheduled time for the next retry attempt.
	 */
	struct Order : public std::binary_function<Ref, Ref, bool> {
		inline bool operator()(const Ref& __x, const Ref& __y) const {
			return __x->nextTime() > __y->nextTime();
		}
	};
};

CSP_NAMESPACE_END

#endif // __CSPLIB_NET_RELIABLEPACKET_H__

