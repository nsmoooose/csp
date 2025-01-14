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
 * @file ThreadQueue.h
 * @brief Thread-safe queue classes for efficiently sharing data
 *   between threads.
 */

#include <csp/csplib/util/Properties.h>
#include <csp/csplib/thread/Synchronization.h>

#include <queue>


namespace csp {

/** Base class for implementing a thread-safe queue that supports multiple
 *  producers and consumers.
 */
template <typename TYPE>
class ThreadSafeQueueBase: public NonCopyable {
public:
	typedef TYPE ValueType;

	/** Exception class used to signal that the queue is full.
	 */
	class Full: public std::exception { };

	/** Exception class used to signal that the queue is empty.
	 */
	class Empty: public std::exception { };

	/** Return the number of elements in the queue.
	 */
	virtual int size() const = 0;

	/** Test if the queue is empty.
	 */
	bool empty() const { return size() == 0; }

	/** Test if the queue is full.
	 */
	bool full() const { return m_maxsize > 0 && size() >= m_maxsize; }

	/** Put an element into the queue.
	 */
	void put(TYPE item, bool block = true) {
		m_notFull.lock();
		while (full()) {
			if (!block) {
				m_notFull.unlock();
				throw Full();
			}
			m_notFull.wait(0, true);
		}
		pushItem(item);
		m_notFull.unlock();
		m_notEmpty.signal();
	}

	/** Retrieve (and remove) the element at the front of the queue.
	 *
	 *  @param item (output) item retrieved from the queue.
	 *  @param block if true, this method will wait until an element is
	 *    available; otherwise it will return immediately.
	 *  @return true if an item was retrieved, false otherwise.
	 */
	bool get(TYPE &item, bool block = true) {
		m_notEmpty.lock();
		while (empty()) {
			if (!block) {
				m_notEmpty.unlock();
				return false;
			}
			m_notEmpty.wait(0, true);
		}
		item = popItem();
		m_notEmpty.unlock();
		m_notFull.signal();
		return true;
	}

	/** Retrieve (and remove) the element at the front of the queue.
	 *
	 *  @param item (output) item retrieved from the queue.
	 *  @param timeout the maximum time (in seconds) to wait for an item to
	 *    retrieve.
	 *  @return true if an item was retrieved, false otherwise.
	 */
	bool get(TYPE &item, double timeout) {
		m_notEmpty.lock();
		while (empty()) {
			if (!m_notEmpty.wait(timeout, true)) {
				m_notEmpty.unlock();
				return false;
			}
		}
		item = popItem();
		m_notEmpty.unlock();
		m_notFull.signal();
		return true;
	}

protected:
	/** Construtor.
	 *
	 *  @param maxsize the upper limit on the size of the queue; or zero for no limit.
	 */
	ThreadSafeQueueBase(int maxsize): m_maxsize(maxsize) {}

	/** Destructor.
	 */
	virtual ~ThreadSafeQueueBase() { }

	/** Add an item to the queue.
	 */
	virtual void pushItem(TYPE &item) = 0;

	/** Retrieve and remove an item from the queue.
	 */
	virtual TYPE popItem() = 0;

private:
	Conditional m_notEmpty;
	Conditional m_notFull;
	int m_maxsize;
};


/** A thread-safe FIFO queue that supports multiple producers and consumers.
 */
template <typename TYPE>
class ThreadSafeQueue: public ThreadSafeQueueBase<TYPE> {
public:
	/** Create a new thread-safe queue.
	 *
	 *  @param maxsize the upper limit on the size of the queue; or zero for no limit.
	 */
	ThreadSafeQueue(int maxsize = 0): ThreadSafeQueueBase<TYPE>(maxsize) {}

	/** Return the number of elements in the queue.
	 */
	virtual int size() const { return m_queue.size(); }

protected:
	/** Add an item to the bottom of the queue.
	 */
	virtual void pushItem(TYPE &item) {
		m_queue.push(item);
	}

	/** Retrieve and remove an item from the top of the queue.
	 */
	virtual TYPE popItem() {
		assert(size() > 0);
		TYPE item = m_queue.front();
		m_queue.pop();
		return item;
	}

private:
	std::queue<TYPE> m_queue;
};


/** A thread-safe priority queue that supports multiple producers and consumers.
 */
template <typename TYPE, typename SEQUENCE = std::vector<TYPE>,
          typename COMPARE = std::less<typename SEQUENCE::value_type> >
class ThreadSafePriorityQueue: public ThreadSafeQueueBase<TYPE> {
public:
	/** Create a new thread-safe priority queue.
	 *
	 *  @param maxsize the upper limit on the size of the queue; or zero for no limit.
	 */
	ThreadSafePriorityQueue(int maxsize = 0): ThreadSafeQueueBase<TYPE>(maxsize) {}

	/** Return the number of elements in the queue.
	 */
	virtual int size() const { return m_queue.size(); }

protected:
	/** Add an item to the bottom of the queue.
	 */
	virtual void pushItem(TYPE &item) {
		m_queue.push(item);
	}

	/** Retrieve and remove an item from the top of the queue.
	 */
	virtual TYPE popItem() {
		assert(size() > 0);
		TYPE item = m_queue.front();
		m_queue.pop();
		return item;
	}

private:
	std::priority_queue<TYPE, SEQUENCE, COMPARE> m_queue;
};


} // namespace csp
