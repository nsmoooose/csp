/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
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
 * @file BufferPool.h
 * @brief A fixed size, fixed capacity buffer pool.
 *
 * Pool allocation and deallocation of memory buffers is roughly 10
 * times faster then new[] and delete[], and 7 times faster than
 * malloc and free (benchmarked with gcc -O2).  Pools also help to
 * prevent memory fragmentation.
 *
 */


#ifndef  __SIMDATA_BUFFERPOOL_H__
#define  __SIMDATA_BUFFERPOOL_H__

#include <SimData/Uniform.h>
#include <SimData/ThreadUtil.h>
#include <cassert>

NAMESPACE_SIMDATA



/** A fixed size, fixed capacity memory buffer pool.
 *
 *  BufferPool provides high performance allocation and deallocation of
 *  fixed size buffers.  When using BufferPools, care must be taken to
 *  properly manage the raw buffer pointers.  In particular, buffers
 *  allocated from a pool must be returned to the pool, either by calling
 *  the release() method of the pool instance, or the static free() method
 *  of the BufferPool class.  Under no circumstances should the C++ new or
 *  delete operators be used with pool buffers (same goes for malloc/free).
 *
 *  Buffers that are not released will result in a memory leak.  As long
 *  as tryAllocate is used for allocation, the leaked buffers be reclaimed
 *  when the pool is destroyed.  If allocate() is used, however, it is
 *  possible that some of the buffers will be allocated individually from
 *  the free store, and these cannot be reclaimed by deleting the pool.
 *
 *  In general, you should try to keep the code that allocates and releases
 *  buffers as localized as possible.  The ScopedBuffer class can help with
 *  this, by providing automatic buffer release the end of the scope in
 *  which it is defined.  Using ScopedBuffers also ensures that buffers are
 *  properly released in the event an exception is thrown.
 */
template <class MUTEX=NoMutex>
class BufferPool: public NonCopyable {

	typedef BufferPool* Prefix;
	static const int OFFSET = sizeof(Prefix);

	uint8* m_buffer;
	uint8** m_index;
	uint32 m_count;
	uint32 m_size;
	MUTEX m_mutex;

public:

	/** Create a new pool.
	 *
	 *  @param size the size of each buffer
	 *  @param count the number of buffers in the pool.
	 */
	BufferPool(uint32 size, uint32 count): m_buffer(0), m_index(0), m_count(count), m_size(size) {
		const int bsize = size + OFFSET;
		m_buffer = new uint8[n * bsize];
		m_index = new (uint8*)[n];
		uint8* binit = m_buffer;
		for (uint32 i = 0; i < n; i++, binit += bsize) {
			m_index[i] = binit + OFFSET;
			*(reinterpret_cast<Prefix*>(binit)) = this;
		}
	}

	uint32 size() const { return m_size; }
	uint32 count() const { return m_count; }

	~BufferPool() {
		delete[] m_index;
		delete[] m_buffer;
	}

	inline uint8 *tryAllocate() {
		ScopedLock<MUTEX> lock(m_mutex);
		return (m_count) ? m_index[--m_count] : 0;
	}

	inline uint8 *allocate() {
		ScopedLock<MUTEX> lock(m_mutex);
		if (m_count) {
			return m_index[--m_count];
		}
		uint8 *new_buffer = new uint8[m_size];
		*(reinterpret_cast<Prefix*>(new_buffer)) = 0;
		return new_buffer + OFFSET;
	}

	inline uint8 *allocateAndClear() {
		uint8 *buffer = allocate();
		memset(buffer, 0, m_size);
		return buffer;
	}

	inline void release(uint8 *buffer) {
		BufferPool::Prefix *base = reinterpret_cast<BufferPool::Prefix*>(buffer) - 1;
		assert(*base == this);
		ScopedLock<MUTEX> lock(m_mutex);
		m_index[m_count++] = buffer;
	}

	static inline void free(uint8 *buffer) {
		if (buffer == 0) return;
		BufferPool::Prefix *base = reinterpret_cast<BufferPool::Prefix*>(buffer) - 1;
		if (*base) {
			(*base)->_release(buffer);
		} else {
			delete[] reinterpret_cast<uint8*>(base);
		}
	};

private:
	inline void _release(uint8 *buffer) {
		ScopedLock<MUTEX> lock(m_mutex);
		m_index[m_count++] = buffer;
	}

};


class ScopedBuffer: public NonCopyable {
	uint8 *m_buffer;
public:
	ScopedBuffer(uint8* buffer): m_buffer(buffer) { }
	~ScopedBuffer() { BufferPool::free(m_buffer); }
	inline uint8 *get() const { return m_buffer; }
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_BUFFERPOOL_H__
