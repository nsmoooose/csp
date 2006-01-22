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
 * @file PacketQueue.h
 *
 */

#ifndef __CSPLIB_NET_PACKETQUEUE_H__
#define __CSPLIB_NET_PACKETQUEUE_H__

#include <csp/csplib/util/CircularBuffer.h>

CSP_NAMESPACE


/** A single-threaded circular buffer that encapsulates rules for
 *  prioritizing realtime messages.
 *
 *  Provides a subset of the CircularBuffer interface, and
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
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT PacketQueue {
	CircularBuffer m_Buffer;
	int m_Depth;
	double m_DropFraction;
public:

	/** Construct a new packet queue.
	 *
	 *  @param size the number of bytes reserved for the queue.
	 *  @param drop_percent the percentage of packets to drop when dropOldest
	 *                      is called.
	 */
	PacketQueue(uint32 size, double drop_percent): m_Buffer(size), m_Depth(0) {
		m_DropFraction = drop_percent * 0.01;
	}

	/** Allocate a new buffer for writing packet data.  You must
	 *  call either commitWriteBuffer or abandonWriteBuffer after
	 *  calling this method (unless the return value is NULL).
	 *
	 *  See CircularBuffer for details.
	 *
	 *  @param size the number of bytes to allocate.
	 *  @return a pointer to a writeable buffer, or NULL if insufficient
	 *    space was available.
	 */
	inline uint8 *getWriteBuffer(const uint32 size) {
		return m_Buffer.getWriteBuffer(size);
	}

	/** Store a buffer previously allocated with getWriteBuffer.
	 *
	 *  See CircularBuffer for details.
	 *
	 *  @param size the number of bytes to store; if smaller than the
	 *    number allocated, the extra bytes are released and made available
	 *    for subsequent allocations.
	 */
	inline void commitWriteBuffer(uint32 size) {
		m_Buffer.commitWriteBuffer(size);
		m_Depth++;
	}

	/** Abandon a buffer previously allocated with getWriteBuffer.
	 *
	 *  See CircularBuffer for details.
	 */
	inline void abandonWriteBuffer() {
		m_Buffer.abandonWriteBuffer();
	}

	/** Get the next packet buffer from the queue.
	 *  Either releaseReadBuffer or replaceReadBuffer must be called
	 *  after calling this method and receiving a non-NULL pointer.
	 *
	 *  See CircularBuffer for details.
	 *
	 *  @param size the size of the packet buffer, in bytes (output-only)
	 *  @return a pointer to the packet buffer, or null if the queue is
	 *  empty.
	 */
	inline uint8* getReadBuffer(uint32 &size) {
		return m_Buffer.getReadBuffer(size);
	}
	
	/** Release a packet buffer previously retrieved by getReadBuffer.
	 *
	 *  See CircularBuffer for details.
	 */
	inline void releaseReadBuffer() {
		m_Buffer.releaseReadBuffer();
		m_Depth--;
	}

	/** Return a packet buffer previously retrieved by getReadBuffer to
	 *  the queue (as though getReadBuffer had not been called).
	 *
	 *  See CircularBuffer for details.
	 */
	inline void replaceReadBuffer() {
		m_Buffer.replaceReadBuffer();
	}

	/** Test if the packet queue is empty.
	 */
	inline bool isEmpty() {
		return m_Buffer.isEmpty();
	}

	/** Get the number of packets waiting in the queue.
	 */
	inline int getQueueDepth() {
		return m_Depth;
	}

	/** Get an estimate of the number of packets that would be
	 *  dropped by calling dropOldest.
	 */
	inline int estimateExcess() {
		return int(m_Depth * m_DropFraction);
	}

	/** Drop excess packets.  Typically called after processing packets in
	 *  the queue, up to some timeout.  Packets that remain unprocessed are
	 *  then dropped (oldest first), to reduce the size of the queue.  The
	 *  fraction of packets dropped is set when the packet queue is created.
	 *  In steady state when the rate at which packets are added to the
	 *  queue exceeds the rate at which packets are processed, this process
	 *  limits the depth of the queue (and hence the maximum age of the
	 *  packets).
	 */
	int dropOldest() {
		int excess = int(m_Depth * m_DropFraction);
		int drop_count = excess;
		uint32 size;
		while (--drop_count >= 0) {
			m_Buffer.getReadBuffer(size);
			m_Buffer.releaseReadBuffer();
			m_Depth--;
		}
		return excess;
	}

};

CSP_NAMESPACE_END

#endif // __CSPLIB_NET_PACKETQUEUE_H__

