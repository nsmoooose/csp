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
 * @file PacketQueue.h
 *
 */

#ifndef __SIMNET_PACKETQUEUE_H__
#define __SIMNET_PACKETQUEUE_H__

#include <SimData/CircularBuffer.h>

namespace simnet {

/** A single-threaded circular buffer that encapsulates rules for
 *  prioritizing realtime messages.
 *
 *  Provides a subset of the simdata::CircularBuffer interface, and
 *  a simple means for keeping the queue depth in check by removing
 *  a fixed fraction of the oldest messages after each processing
 *  cycle.
 *
 *  First assume that the message receive rate is constant, as is
 *  the rate at which messages are processed.  If the processing
 *  rate exceeds the incoming rate, the queue will be empty after
 *  each processing cycle.  Otherwise, the queue would grow linearly
 *  if left unchecked.
 *
 *  By removing a fixed fraction F of the oldest messages left after
 *  each processing cycle, the queue depth D will saturate when the
 *  drop rate equals the excess inbound message rate.  In the limit
 *  that message processing is completely stalled we have F*D = M,
 *  where M is the number of inbound messages per cycle.  Hence D
 *  is just M/F, so the maximum message age is limited to 1/F times
 *  the processing interval (neglecting transmission latencies).
 *  For example, a processing rate of 50 Hz and a drop fraction of
 *  0.05 gives a maximum message age of 20 ms / 0.05 = 400 ms.
 *
 *  More realistically, both the incoming message rate and the
 *  processing rate will vary over time.  TODO more discussion.
 */
class PacketQueue {
	simdata::CircularBuffer m_Buffer;
	int m_Depth;
	double m_DropFraction;
public:
	PacketQueue(simdata::uint32 size, double drop_percent): m_Buffer(size), m_Depth(0) {
		m_DropFraction = drop_percent * 0.01;
	}

	inline simdata::uint8 *getWriteBuffer(const simdata::uint32 size) {
		return m_Buffer.getWriteBuffer(size);
	}
	inline void commitWriteBuffer(simdata::uint32 size) {
		m_Buffer.commitWriteBuffer(size);
		m_Depth++;
	}
	inline void abandonWriteBuffer() {
		m_Buffer.abandonWriteBuffer();
	}
	inline simdata::uint8* getReadBuffer(simdata::uint32 &size) {
		return m_Buffer.getReadBuffer(size);
	}
	inline void releaseReadBuffer() {
		m_Buffer.releaseReadBuffer();
		m_Depth--;
	}
	inline void replaceReadBuffer() {
		m_Buffer.replaceReadBuffer();
	}
	inline bool isEmpty() {
		return m_Buffer.isEmpty();
	}

	inline int getQueueDepth() {
		return m_Depth;
	}

	inline int estimateExcess() {
		return int(m_Depth * m_DropFraction);
	}

	int dropOldest() {
		int excess = int(m_Depth * m_DropFraction);
		int drop_count = excess;
		simdata::uint32 size;
		while (--drop_count >= 0) {
			m_Buffer.getReadBuffer(size);
			m_Buffer.releaseReadBuffer();
			m_Depth--;
		}
		return excess;
	}

};

} // namespace simnet

#endif // __SIMNET_PACKETQUEUE_H__

