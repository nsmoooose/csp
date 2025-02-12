#pragma once
/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file CircularBuffer.h
 *
 * @brief Circular buffer and ring queue classes for efficient data
 *        transfer between threads.
 */

#include <csp/csplib/util/Uniform.h>
#include <csp/csplib/util/Properties.h>

#include <cassert>
#include <algorithm>

namespace csp {


/** A simple ring queue class for passing arbitrary types between
 *  threads.
 *
 *  TODO This implementation is fairly primitive, provides no support
 *  for stl algorithms, and has not been extensively tested.
 */
template <typename TYPE>
class RingQueue: public NonCopyable {

	static const int SIZE = sizeof(TYPE);

	TYPE* m_buffer;
	volatile uint32_t m_read;
	volatile uint32_t m_write;
	uint32_t m_count;
	uint32_t m_mask;

public:

	/** Create a new ring queue.
	 *
	 *  @param count the maximum number of elements in the queue; must be a power of 2.
	 */
	RingQueue(uint32_t count) {
		m_mask = count - 1;
		m_count = count;
		assert((m_count & m_mask) == 0);
		m_read = 0;
		m_write = 0;
		m_buffer = new TYPE[count];
	}

	uint32_t getCount() const { return m_count; }

	~RingQueue() {
		delete[] m_buffer;
	}

	inline TYPE *getElementForWrite() {
		if (isFull()) return 0;
		return &(m_buffer[m_write]);
	}

	inline bool isFull() const {
		if (m_write == m_mask) {
			if (m_read == 0) return true;
		} else {
			if (m_write + 1 == m_read) return true;
		}
		return false;
	}

	inline bool isEmpty() const {
		return (m_read == m_write);
	}

	inline void commitElement() {
		m_write = (m_write + 1) & m_mask;
	}

	inline bool write(TYPE const &value) {
		TYPE* element = getElementForWrite();
		if (!element) return false;
		*element = value;
		commitElement();
		return true;
	}

	inline TYPE* getElementForRead() {
		if (isEmpty()) return 0;
		return &(m_buffer[m_read]);
	}

	inline void releaseElement() {
		m_read = (m_read + 1) & m_mask;
	}

	inline bool read(TYPE &value) {
		TYPE* element = getElementForRead(element);
		if (!element) return false;
		value = *element;
		releaseElement();
		return true;
	}

};


/** A circular byte buffer that can be used as a FIFO queue to send
 *  arbitrary messages between threads without locking overhead.
 *
 *  A contiguous, fixed size buffer is allocated on construction.  A
 *  single writer thread allocate a variable size block from this
 *  buffer, copies data to the allocated block, and commits the block.
 *  The writer repeats this, obtaining new blocks and commiting data
 *  as long as free space remains in the buffer.  Simultaneously, a
 *  reader thread requests a pointer to the oldest data block, processes
 *  the data, and releases the block.  No internal synchronization is
 *  required, so the circular buffer incurs far less overhead than
 *  other techniques such as thread-safe queues.  All memory blocks
 *  are obtained from a pre-allocated pool, further reducing overhead
 *  and eliminating memory fragmentation.  The primary limitation is
 *  that there can be only one reader and one writer thread.  If one
 *  thread needs to communicate with multiple threads, multiple
 *  circular buffers can be used.  For general multi-producer,
 *  multi-consumer problems, consider using a thread-safe queue.
 *
 *  Example use:
 *
 *  @code
 *  // reads messages from an external source, puts them into a
 *  // circular buffer shared with thread2
 *  void thread1_run(CircularBuffer &buffer) {
 *    const uint32_t timeout = 100; // milliseconds
 *    while (true) {
 *      if (messagePending(timeout)) {
 *        uint32_t size = getMessageSize();
 *        uint8_t *block = 0;
 *        while (!block) {
 *          block = buffer.getWriteBuffer(size);
 *          // if the buffer is full, yield to allow thread2 to free
 *          // up some space (important!)
 *          if (!block) thread::yield();
 *        }
 *        readMessage(block, size);
 *        buffer.commitWriteBuffer();
 *      }
 *    }
 *  }
 *
 *  // processes messages placed into a circular buffer by thread1
 *  void thread2_run(CircularBuffer &buffer) {
 *    while (true) {
 *      while (buffer.isEmpty()) {
 *        // wait for new messages by yielding (important!)
 *        thread::yield();
 *      }
 *      uint32_t size;
 *      uint8_t *block = buffer.getReadBuffer(size);
 *      processMessage(block, size);
 *      buffer.releaseReadBuffer();
 *  }
 *  @endcode
 */
class CircularBuffer: public NonCopyable {

	uint8_t* m_buffer;
	uint32_t m_size;
	volatile uint32_t m_read;
	volatile uint32_t m_write;
	volatile uint32_t m_limit;
	uint32_t m_allocated;
	uint32_t m_next_read;
	uint32_t m_next_write;
	uint32_t *m_start_write;

	// space for caching the block size at the start of each block
	static const uint32_t RESERVE = 4;

public:

	/** Create a new circular buffer.
	 *
	 *  @param size the maximum capacity of the buffer (in bytes)
	 */
	CircularBuffer(uint32_t size) {
		m_size = size + RESERVE;
		m_read = 0;
		m_write = 0;
		m_limit = m_size;
		m_next_read = 0;
		m_next_write = 0;
		m_allocated = 0;
		m_start_write = 0;
		m_buffer = new uint8_t[size];
	}

	/** Get the maximum capacity of the buffer when completely empty (in bytes).
	 */
	inline uint32_t capacity() const { return m_size - RESERVE; }

	/** Destroy the buffer, freeing the internal memory pool.
	 */
	~CircularBuffer() {
		delete[] m_buffer;
	}

	/** Allocate a block of memory from the buffer for writing.
	 *
	 *  Each successful call to getWriteBuffer must be followed
	 *  by a call to either commitWriteBuffer or abandonWriteBuffer.
	 *  Calling getWriteBuffer twice without calling either of these
	 *  methods an error.  In debug mode this error is trapped by
	 *  an assertion, but it may cause silent data corruption in
	 *  release mode.
	 *
	 *  If getWriteBuffer fails (ie. returns NULL) there is no
	 *  need to call commitWriteBuffer or abandonWriteBuffer,
	 *  but it is safe to do so.
	 *
	 *  @param size The size of the requested block (in bytes).
	 *  @return A pointer to a memory block of the requested size
	 *    on success, or NULL if there is insufficient space to
	 *    allocate the requested block.
	 */
	inline uint8_t *getWriteBuffer(const uint32_t size) {
		// check that there aren't any allocated but uncommitted blocks
		assert(m_write == m_next_write);
		if (size == 0 || getMaximumAllocation() < size) return 0;
		uint32_t offset = m_write;
		// if no room at the end of the buffer; allocate from the start
		if (m_size - m_write < size + RESERVE) {
			offset = 0;
			m_limit = m_write;
		}
		m_allocated = size;
		// cache the buffer size at the start of the block
		m_start_write = reinterpret_cast<uint32_t*>(&(m_buffer[offset]));
		*m_start_write = size;
		// bump the next_write offset to the end of the block.
		m_next_write = offset + RESERVE + size;
		// return a pointer to the new block (just after the cached
		// block size).
		return (m_buffer + offset + RESERVE);
	}

	/** Returns a measure of the maximum number of bytes that can be
	 *  allocated.  For the writer thread this will be a lower bound
	 *  on the available space (asynchronous reads can increase the
	 *  available space at any time).  For the reader thread this is
	 *  an upper bound (additional space may be consumed by the writer
	 *  thread at any time).
	 */
	inline uint32_t getMaximumAllocation() const {
		uint32_t read = m_read;
		if (read > m_write) {
			if (read - m_write <= RESERVE) return 0;
			return read - m_write - RESERVE - 1;
		}
		uint32_t space = std::max<uint32_t>(read, m_size - m_write);
		if (space > RESERVE) return space - RESERVE - 1;
		return 0;
	}

	/** Returns a measure of the number of bytes in the buffer that are
	 *  allocated.  Note that getAllocatedBytes() + getMaximumAllocation()
	 *  will generally be less than capacity due to wasted space at the
	 *  end of the buffer.
	 */
	inline uint32_t getAllocatedSpace() const {
		uint32_t read = m_read;
		uint32_t write = m_write;
		uint32_t space;
		if (read > write) {
			space = (m_size + write) - read;
		} else {
			space = write - read;
		}
		if (space > RESERVE) return space - RESERVE;
		return 0;
	}

	/** Returns true if the buffer is empty (no blocks to read).
	 */
	inline bool isEmpty() const {
		return (m_read == m_write);
	}

	/** Commit the last block allocated by getWriteBuffer.
	 *
	 *  Once committed it will be immediately available to the reader thread,
	 *  so the writer thread should make no further changes to the data.
	 *  This method is idempotent, and can be safely called even if
	 *  getWriteBuffer fails.
	 */
	inline void commitWriteBuffer() {
		m_write = m_next_write;
		m_allocated = 0;
	}

	/** Commit the last block allocated by getWriteBuffer, but with a reduced
	 *  the size.  The extra space remains available for subsequent
	 *  allocations.  This can be useful when the required block size is not
	 *  known at the time of allocation.  Request extra space from
	 *  getWriteBuffer, then specify the actual block size when committing.
	 *
	 *  Note that on average, half of the mean allocated block size will be
	 *  wasted in the circular buffer, so excessively large allocations
	 *  should be avoided.  This waste is independent of how the blocks are
	 *  trimmed when committing.
	 *
	 *  This method is idempotent, and can be safely called even if
	 *  getWriteBuffer fails.  Calling with size == 0 is equivalent to
	 *  calling abandonWriteBuffer.
	 *
	 *  @param size The new size of the committed block; must be less than
	 *    or equal to the original size.  If zero, the block is abandoned.
	 */
	inline void commitWriteBuffer(uint32_t size) {
		if (size == 0) { abandonWriteBuffer(); return; }
		if (m_write == m_next_write) return;
		assert(size <= m_allocated);
		assert(m_next_write + size > m_allocated + RESERVE);
		m_next_write = (m_next_write + size) - m_allocated;
		*m_start_write = size;
		commitWriteBuffer();
	}

	/** Abandon the last block allocated by getWriteBuffer.
	 *
	 *  Abandoned blocks are returned to the pool and will not be made
	 *  available to the reader thread.  This method is idempotent, and can
	 *  be safely called even if getWriteBuffer fails.
	 */
	inline void abandonWriteBuffer() {
		m_next_write = m_write;
		m_allocated = 0;
	}

	/** Retrieve the next block from the buffer for reading.
	 *
	 *  Successful calls to getReadBuffer return a block of memory that has
	 *  been previously committed to the buffer by the writer thread.  After
	 *  data has been extracted from the block, call releaseReadBuffer to
	 *  release the block for use by the writer thread.  Once released, the
	 *  block should no longer be used.
	 *
	 *  It is an error to call getReadBuffer again without calling
	 *  releaseReadBuffer.  This error is trapped by an assertion in debug
	 *  mode, but can lead to silent data corruption in release mode.
	 *
	 *  @param size Returns the size of the block (in bytes).
	 *  @return A pointer to the next block, or NULL if the buffer
	 *    is empty.
	 */
	inline uint8_t* getReadBuffer(uint32_t &size) {
		assert(m_read == m_next_read);
		if (isEmpty()) {
			size = 0;
			return 0;
		}
		assert(m_read <= m_limit);
		if (m_read == m_limit) {
			m_read = 0;
			m_limit = m_size;
		}
		size = *(reinterpret_cast<uint32_t*>(m_buffer + m_read));
		m_next_read = m_read + size + RESERVE;
		return (m_buffer + m_read + RESERVE);
	}

	/** Release the last block read by getReadBuffer.
	 *
	 *  Once released the block will be immediately available to the writer
	 *  thread, so the reader thread should not access the data any more.
	 *  This method is idempotent, and can be safely called even if
	 *  getReadBuffer fails.
	 */
	inline void releaseReadBuffer() {
		assert(m_next_read <= m_limit);
		m_read = m_next_read;
	}

	/** Place the last block returned by getReadBuffer back into the buffer,
	 *  as though getReadBuffer had never been called.
	 *
	 *  This method is idempotent, and can be safely called even if
	 *  getReadBuffer fails.
	 */
	inline void replaceReadBuffer() {
		assert(m_next_read <= m_limit);
		m_next_read = m_read;
	}

};


} // namespace csp
