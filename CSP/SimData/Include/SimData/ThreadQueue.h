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
 * @file ThreadQueue.h
 * @brief Thread-safe queue classes for efficiently sharing data
 *   between threads.
 */


#ifndef __SIMDATA_THREADQUEUE_H__
#define __SIMDATA_THREADQUEUE_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/ThreadBase.h>
#include <pthread.h>
#include <queue>


NAMESPACE_SIMDATA

/** Base class for implementing a thread-safe queue that supports multiple
 *  producers and consumers.
 */
template <typename TYPE>
class ThreadSafeQueueBase: public NonCopyable {
public:
	typedef TYPE ValueType;
	class Full: public std::exception { };
	class Empty: public std::exception { };
	virtual int size() const = 0;
	bool empty() const { return size() == 0; }
	bool full() const { return m_maxsize > 0 && size() >= m_maxsize; }
	void put(TYPE item, bool block = true) {
		m_notFull.lock();
		while (full()) {
			if (!block) {
				m_notFull.unlock();
				throw Full();
			}
			m_notFull.wait();
		}
		pushItem(item);
		m_notFull.unlock();
		m_notEmpty.signal();
	}
	TYPE get(bool block = true) {
		m_notEmpty.lock();
		while (empty()) {
			if (!block) {
				m_notEmpty.unlock();
				throw Empty();
			}
			m_notEmpty.wait();
		}
		TYPE item = popItem();
		m_notEmpty.unlock();
		m_notFull.signal();
		return item;
	}
protected:
	ThreadSafeQueueBase(int maxsize)
		: m_mutex(), m_notEmpty(m_mutex), m_notFull(m_mutex), m_maxsize(maxsize) {}
	virtual ~ThreadSafeQueueBase() { }
	virtual void pushItem(TYPE &item) = 0;
	virtual TYPE popItem() = 0;
private:
	ThreadMutex m_mutex;
	ThreadCondition m_notEmpty;
	ThreadCondition m_notFull;
	int m_maxsize;
};


/** A thread-safe FIFO queue that supports multiple producers and consumers.
 */
template <typename TYPE>
class ThreadSafeQueue: public ThreadSafeQueueBase<TYPE> {
public:
	ThreadSafeQueue(int maxsize = 0): ThreadSafeQueueBase<TYPE>(maxsize) {}
	virtual int size() const { return m_queue.size(); }
protected:
	virtual void pushItem(TYPE &item) {
		m_queue.push(item);
	}
	virtual TYPE popItem() {
		TYPE item = m_queue.top();
		m_queue.pop();
		return item;
	}
private:
	std::queue<TYPE> m_queue;
};


/** A thread-safe priority queue that supports multiple producers and consumers.
 */
template <typename TYPE>
class ThreadSafePriorityQueue: public ThreadSafeQueueBase<TYPE> {
public:
	ThreadSafePriorityQueue(int maxsize = 0): ThreadSafeQueueBase<TYPE>(maxsize) {}
	virtual int size() const { return m_queue.size(); }
protected:
	virtual void pushItem(TYPE &item) {
		m_queue.push(item);
	}
	virtual TYPE popItem() {
		TYPE item = m_queue.top();
		m_queue.pop();
		return item;
	}
private:
	std::priority_queue<TYPE> m_queue;
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREADQUEUE_H__

