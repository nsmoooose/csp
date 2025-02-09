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
 * @file Thread.h
 * @brief PosixThreads wrapper and utility classes.
 */

#include <optional>
#include <thread>

#include <csp/csplib/util/Properties.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/thread/AtomicCounter.h>
#include <csp/csplib/thread/Synchronization.h>
#include <csp/csplib/util/undef.h>

namespace csp {

/** Base class for referenced counted objects that are shared between
 *  multiple threads.  Thread-safe, supporting atomic updates of the
 *  internal reference counter.
 */
typedef ReferencedBase<AtomicCounter> ThreadSafeReferenced;

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
	typedef Ref<Task> TaskRef;

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
	 *  signals on exit.  Used via Thread.
	 *
	 *  @param timeout the maximum time in seconds to wait for the task to finish.
	 *  @return true if the task ended, false if the timeout expired.
	 */
	bool join(double timeout) {
		return m_exit.wait(timeout);
	}

	struct StartAndFinish {
		StartAndFinish(Task &task): m_task(task) { m_task.m_running = true; }
		~StartAndFinish() { m_task.m_complete = true; m_task.m_exit.signal(); }
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
	Event m_exit;
};


/** Base class for wrapping a Posix thread.
 *
 *  This class is not used directly. To create a thread, subclass Task and
 *  implement the run() method. @see Thread for details.
 */
class Thread {
private:
	std::optional<std::thread> m_thread;
public:
	/** Construct a new thread with a bound task.
	 *
	 *  @param task a Task pointer to bind to this thread, or NULL.
	 */
	Thread(Task *task = 0):
		m_task(task),
		m_started(false),
		m_cancelled(false) {
	}

	/** Destroy a thread instance and free internal resources.
	 *
	 *  The thread will be joined if it is running and not detached.
	 */
	virtual ~Thread() {
		CSPLOG(INFO, THREAD) << "~thread " << this;
		if (!isDetached()) {
			join();
		}
	}

	bool isDetached() {
		assert(isStarted());
		return !m_thread->joinable();
	}

	/** Get the task bound to this thread, or NULL.
	 */
	Ref<Task> const &getTask() const { return m_task; }

	void join() {
		assert(!isDetached());
		assert(isStarted());
		m_thread->join();
	}

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
		m_thread.emplace(&Thread::run, this);
		m_started = true;
	}

	void detach(Task *task = 0) {
		assert(!isStarted());
		if (m_task.valid()) assert(task == 0);
		if (task) m_task = task;
		m_thread->detach();
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
		CSPLOG(INFO, THREAD) << "thread exit " << m_thread->get_id();
	}

	virtual void run() {
		CSPLOG(INFO, THREAD) << "thread start " << m_thread->get_id();
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


} // namespace csp
