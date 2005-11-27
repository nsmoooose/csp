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
 * @file Thread.h
 * @brief PosixThreads wrapper and utility classes.
 */


#ifndef __CSPLIB_THREAD_THREAD_H__
#define __CSPLIB_THREAD_THREAD_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/thread/AtomicCounter.h>
#include <csp/csplib/thread/Synchronization.h>

#include <cc++/thread.h>


CSP_NAMESPACE


/** Base class for referenced counted objects that are shared between
 *  multiple threads.  Thread-safe, supporting atomic updates of the
 *  internal reference counter.
 */
typedef ReferencedBase<AtomicCounter> ThreadSafeReferenced;


/** Wrapper for ost::Thread that provides a restricted interface to
 *  the thread id (as an integer).  Caution: since this class keeps
 *  a pointer to the thread object, it is not safe to use after the
 *  thread is deleted.
 */
class ThreadId {
public:
	ThreadId(ost::Thread *thread): m_thread(thread) { }
	uint64 id() const { return m_thread ? static_cast<uint64>(m_thread->getId()) : 0; }
	bool operator==(const ThreadId &other) { return id() == other.id() && m_thread && other.m_thread; }
private:
	ost::Thread *m_thread;
};


/**
 * @namespace thread
 *
 * @brief Global functions for manipulating threads.
 */
namespace thread {

	/** Yield control from the current thread to another thread of equal
	 *  or higher priority.  If no such threads are available, the calling
	 *  thread continues immediately.
	 */
	static inline void yield() { ost::Thread::yield(); }

	/** Return a Thread instance for the currently executing thread.
	 */
	static inline ThreadId self() { return ThreadId(ost::getThread()); }

	/** Return the id of the current thread.
	 */
	static inline uint64 id() { return self().id(); }

	/** Suspend execution of the current thread the specified amount of time.
	 *
	 *  @param seconds seconds to pause (fractional values ok, but limited by
	 *    the granularity of the operating system's time slices).
	 */
	static inline void sleep(double seconds) {
		ost::Thread::sleep(static_cast<timeout_t>(seconds * 1000));
	}

	/** Suspend execution of the current thread the specified amount of time.
	 *
	 *  @param milliseconds milliseconds to pause (typically limited by the
	 *    granularity of the operating system's time slices).
	 */
	static inline void millisleep(int milliseconds) {
		ost::Thread::sleep(static_cast<timeout_t>(milliseconds));
	}

} // namespace thread


class BaseThread;


/** Abstract base class for tasks that can be executed in a thread.
 *
 *  Implement run() to define the action of the task.  Tasks are reference
 *  counted to manage task instance lifetime between the calling thread and
 *  the execution thread.
 */
class Task: public ThreadSafeReferenced {

friend class Thread;

public:
	typedef Ref<Task> Ref;

	/** Test if a request to abort the task has been made.
	 *
	 *  @return true if abort() has been called, false otherwise.
	 */
	bool isAborted() const { return m_abort; }

	/** Returns true if this task is executing in a detached thread.  Memory
	 *  resources associated with a detached thread are automatically reclaimed
	 *  when the thread exits, but other threads cannot join a detached thread.
	 */
	bool isDetached() const { return m_detached; }

	/** Returns true if this task is executing.  Returns false before the
	 *  run() method is called and after it returns.
	 */
	bool isRunning() const { return m_running; }

	/** Returns true if the run() method has complete.
	 */
	bool isComplete() const { return m_complete; }

protected:
	/** Entry point for a new thread.
	 *
	 *  Implement this method in subclasses to carry out the actions of the
	 *  thread.
	 */
	virtual void run()=0;

	/** Signal a task to abort.
	 *
	 *  This is advisory only.  Tasks can test for this signal by calling
	 *  isAborted(), then ignore or honor the aborted condition in any manner.
	 */
	virtual void abort() { m_abort = true; }

	/** Initialize the task state.
	 */
	Task(): m_running(false), m_detached(false), m_complete(false), m_abort(false) {
	}

	virtual ~Task() { }

private:
	/** Mark this task as running in a detached thread.
	 */
	void setDetached() { m_detached = true; }

	/** Join this task's thread by for an internal condition variable that
	 *  signals on exit.  Used via ThreadBase.
	 *
	 *  @param timeout the maximum time in seconds to wait for the task to finish.
	 *  @return true if the task ended, false if the timeout expired.
	 */
	bool join(double timeout) {
		return m_exit.wait(timeout, false);
	}

	struct StartAndFinish {
		StartAndFinish(Task &task): m_task(task) { m_task.m_running = true; }
		~StartAndFinish() { m_task.m_complete = true; m_task.m_exit.signalAll(); }
		Task &m_task;
	};

	void _execute() {
		StartAndFinish start_and_finish(*this);
		run();
	}

	bool m_running;
	bool m_detached;
	bool m_complete;
	bool m_abort;
	Conditional m_exit;
};


/** Base class for wrapping a Posix thread.
 *
 *  This class is not used directly. To create a thread, subclass Task and
 *  implement the run() method. @see Thread for details.
 */
class Thread: public ost::Thread {
public:
	/** Construct a new thread with a bound task.
	 *
	 *  @param task a Task pointer to bind to this thread, or NULL.
	 */
	Thread(Task *task = 0): m_task(task), m_started(false), m_cancelled(false) { }

	/** Destroy a thread instance and free internal resources.
	 *
	 *  The thread will be joined if it is running and not detached.
	 */
	virtual ~Thread() {
		CSPLOG(INFO, THREAD) << "~thread " << this;
		if (!isDetached()) join();
		terminate();
	}

	/** Get the task bound to this thread, or NULL.
	 */
	Ref<Task> const &getTask() const { return m_task; }

	/** Wait (indefinitely) for this thread to finish.
	 */
	using ost::Thread::join;

	/** Wait for at most a limited amount of time for this thread to finish.
	 *
	 *  @param timeout the maximum time in seconds to wait for the task to finish.
	 *  @return true if the task ended, false if the timeout expired.
	 */
	bool join(double timeout) {
		assert(!isDetached());
		if (isActive()) {
			return m_task->join(timeout);
		}
		return false;
	}

	/** Signal the associated task to abort.  This is an advisory request;
	 *  the task is free to ignore or honor it in any manner.  Provides a
	 *  gentler alternative to cancel().
	 */
	void abort() {
		if (isStarted()) m_task->abort();
	}

	/** Start the thread running. Calls run() in the new thread and returns
	 *  immediately.
	 */
	void start(Task *task = 0) {
		assert(!isStarted());
		if (m_task.valid()) assert(task == 0);
		if (task) m_task = task;
		ost::Thread::start();
		m_started = true;
	}

	void detach(Task *task = 0) {
		assert(!isStarted());
		if (m_task.valid()) assert(task == 0);
		if (task) m_task = task;
		ost::Thread::detach();
		m_started = true;
	}

	/** Returns true if this thread has been started successfully via start().
	 */
	bool isStarted() const {
		return m_started;
	}

	/** Test if this thread has been started but has not yet finished.
	 */
	bool isActive() const {
		return m_task.valid() && isStarted() && !m_task->isComplete();
	}

protected:
	virtual void final() {
		CSPLOG(INFO, THREAD) << "thread exit " << ThreadId(this).id();
	}

	virtual void run() {
		CSPLOG(INFO, THREAD) << "thread start " << ThreadId(this).id();
		// should we try to catch and translate ost exceptions here?  note that the
		// ost::Thread constructor can throw, but there's no way to catch the exception
		// from this subclass.
		if (m_task.valid()) m_task->_execute();
	}

private:
	Ref<Task> m_task;
	bool m_started;
	bool m_cancelled;
};


CSP_NAMESPACE_END

#endif // __CSPLIB_THREAD_THREAD_H__


