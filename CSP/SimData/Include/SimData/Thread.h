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
 * @file ThreadBase.h
 * @brief PosixThreads wrapper and utility classes.
 */


#ifndef __SIMDATA_THREAD_H__
#define __SIMDATA_THREAD_H__

#include <SimData/Namespace.h>
#include <SimData/Properties.h>
#include <SimData/ThreadBase.h>
#include <SimData/Ref.h>
#include <iostream>
#include <iomanip>
#include <pthread.h>


NAMESPACE_SIMDATA


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
	static int yield() {
		return sched_yield();
	}

	/** Return a Thread instance for the currently executing thread.
	 */
	//static Thread self() { return Thread(pthread_self()); }

	/** Terminate the currently executing thread.
	 */
	static void exit() {
		pthread_exit(0);
	}

	/** Set the thread cancellation state to enabled.  Requests to cancel
	 *  this thread will be honored.
	 */
	static void enableCancel() {
		const int result = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
		ThreadException::check(result);
	}

	/** Set the thread cancellation state to disabled.  Requests to cancel
	 *  this thread will be ignored.
	 */
	static void disableCancel() {
		const int result = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		ThreadException::check(result);
	}

	/** Terminate the current thread if there are any pending cancellation
	 *  requests.  @see Thread::cancel().
	 */
	static void testCancel() {
		pthread_testcancel();
	}

	/** Set the cancellation type to be asynchronous.  The current thread will
	 *  be terminated immediately if cancelled by another thread.
	 */
	static void useAsynchronousCancel() {
		const int result = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
		ThreadException::check(result);
	}

	/** Set the cancellation type to be deferred.  Cancellation requests will be
	 * deferred until a cancellation checkpoint is reached (e.g a blocking wait
	 * or an explicit call to testCancel()).
	 */
	static void useDeferredCancel() {
		const int result = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
		ThreadException::check(result);
	}

	/** Suspend execution of the current thread the specified amount of time.
	 *
	 *  @param seconds seconds to pause (fractional values ok, but limited by
	 *    the granularity of the operating system's time slices).
	 */
	static void sleep(double seconds) {
		static ThreadCondition m_condition;
		m_condition.lock();
		m_condition.wait(seconds);
		m_condition.unlock();
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

friend class BaseThread;

public:
	typedef pthread_t ThreadId;

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

	/** Get the name (string identifier) of this task.
	 */
	std::string getName() const { return m_name; }

	/** Initialize the task state.
	 */
	Task(): m_running(false), m_detached(false), m_complete(false), m_abort(false), m_thread_id(0) {
	}

	virtual ~Task() { }

private:
	/** Static callback to initialize the task in a new thread.
	 *
	 *  @param param A pointer to a Ref<Task> instance, which serves to pass the
	 *    task reference to the new thread and also ensures that the task will
	 *    remained reference even if the Thread instance that starts the task
	 *    goes out of scope before the new thread begins.  The caller passes
	 *    ownership of the pointer.
	 */
	static void *_start(void *param) {
		Ref<Task> *task_ptr = static_cast<Ref<Task>*>(param);
		assert(task_ptr);
		Ref<Task> task = *task_ptr;
		// delete param (we own it and it has served its purpose)
		delete task_ptr;
		assert(task);
		// execute the task
		task->_execute();
		// drop our reference to the task
		task = 0;
		// terminate this thread
		thread::exit();
	}

	/** Begin execution of the task.
	 */
	void _execute() {
		m_running = true;
		m_thread_id = pthread_self();
		run();
		m_complete = true;
		// signal any threads waiting to join with us
		m_exit.signalAll();
	}

	/** Mark this task as running in a detached thread.
	 */
	void setDetached() { m_detached = true; }

	/** Set the name of this task (arbitrary string identifier).
	 */
	void setName(std::string const &name) { m_name = name; }

	/** Join this task's thread by for an internal condition variable that
	 *  signals on exit.  Used via ThreadBase.
	 *
	 *  @param timeout the maximum time in seconds to wait for the task to finish.
	 *  @return true if the task ended, false if the timeout expired.
	 */
	bool join(double timeout) {
		ScopedLock<ThreadCondition> lock(m_exit);
		return m_exit.wait(timeout);
	}

	std::string m_name;
	bool m_running;
	bool m_detached;
	bool m_complete;
	bool m_abort;
	ThreadId m_thread_id;
	ThreadCondition m_exit;
};


/** Base class for wrapping a Posix thread.
 *
 *  This class is not used directly. To create a thread, subclass Task and
 *  implement the run() method. @see Thread for details.
 */
class BaseThread: public NonCopyable {

	// private and undefined, use BaseThread(Task*, std::string const &)
	BaseThread();

protected:

	/** Construct a new thread with a bound task.  See Thread<> for details.
	 *
	 *  @param task a non-null Task pointer to bind to this thread.
	 *  @param name a unique string identifier for this thread.
	 */
	BaseThread(Task *task, std::string const &name): m_task(task), m_thread_id(0), m_cancelled(false) {
		assert(task);
		task->setName(name);
	}

	/** Accessor used by Thread<> to provide type-specialized access to the
	 *  associated task instance.
	 */
	Ref<Task> const &getTask() const { return m_task; }

public:

	/** Destroy a thread instance and free internal resources.
	 *
	 *  The thread will be cancelled if it is running and not detached.
	 */
	virtual ~BaseThread() {
		if (isActive() && !isDetached()) {
			cancel();
		}
	}

	/** Wait (indefinitely) for this thread to finish.
	 */
	void join() {
		assert(isStarted());
		assert(!isDetached());
		const int result = pthread_join(m_thread_id, 0);
		ThreadException::check(result);
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

	/** Detach the thread.  Once detached, the resources of the thread will
	 *  be reclaimed automatically when the thread exits.  Note that you
	 *  cannot join a detached thread.
	 */
	void detach() {
		assert(isStarted());
		assert(!isDetached());
		const int result = pthread_detach(m_thread_id);
		ThreadException::check(result);
		m_task->setDetached();
	}

	/** Request termination of the thread.
	 *
	 *  The response of the thread depends on the thread's cancellation
	 *  type and state.  If cancellation is enabled and the cancellation
	 *  type is asynchronous, the thread will exit immediately.  Otherwise
	 *  the cancellation will be deferred until the thread hits a
	 *  cancellation checkpoint (e.g. by calling testCancel).
	 */
	void cancel() {
		if (m_cancelled) return;
		assert(isStarted());
		const int result = pthread_cancel(m_thread_id);
		ThreadException::check(result);
		m_cancelled = true;
		//m_thread_id = 0;
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
	void start() {
		assert(!isStarted());
		// create a new Ref<Task> to ensure that m_task remains referenced until _start
		// is called.  _start takes care of deleting the reference.
		int result = pthread_create(&m_thread_id, 0, &Task::_start, new Ref<Task>(m_task));
		ThreadException::check(result);
	}

	/** Returns true if this thread has been started successfully via start().
	 */
	bool isStarted() const {
		return m_thread_id != 0;
	}

	/** Test if this thread has been started but has not yet finished.
	 */
	bool isActive() const {
		return m_task.valid() && m_thread_id && !m_task->isComplete();
	}

	/** Test if this thread has been detached.
	 */
	bool isDetached() const { return m_task.valid() && m_task->isDetached(); }

	/** Return the name of the task associated with this thread.
	 */
	std::string getName() const {
		assert(m_task.valid());
		return m_task->getName();
	}

private:
	Task::ThreadId m_thread_id;
	Ref<Task> m_task;
	bool m_cancelled;
};


/** Class for starting and manipulating a posix thread.
 *
 *  To create a new thread, subclass Task and implement the run() method.
 *  Then instantiate Thread<YourTaskSubclass>, and call the start() method.
 *
 *  For example:
 *  @code
 *  // implement a task to run in a separate thread; this is just a
 *  // silly example.
 *  class MyTask: public simdata::Task {
 *  protected:
 *    // the task action; the thread running this task will terminate when
 *    // when run() returns.
 *    virtual void run() {
 *      for (int i = 0; i < 1000; i++) std::cout << "hello " << i << "\n";
 *    }
 *  };
 *
 *  void start_a_couple_tasks() {
 *    // construct a couple threads to execute MyTask
 *    simdata::Thread<MyTask> task1("hello writer 1");
 *    simdata::Thread<MyTask> task2("hello writer 2");
 *
 *    // start both tasks
 *    task1.start();
 *    task2.start();
 *
 *    // wait for both tasks to finish
 *    task1.join()
 *    task2.join()
 *  }
 *  @endcode
 */
template <class TASK>
class Thread: public BaseThread {

	// must specify a task name
	Thread();

public:

	/** Construct a new Thread instance to execute a task.
	 *
	 *  The thread does not start running upon creation; you must call
	 *  the start() method to begin execution.
	 *
	 *  @param name An arbitrary string identifier for the new task.
	 */
	Thread(std::string const &name): BaseThread(new TASK(), name) { }

	/** Get the associated task instance.
	 *
	 *  Requires an internal downcast from Task to the template type, so
	 *  store a local copy if this overhead is a concern.
	 */
	Ref<TASK> getTask() const { return BaseThread::getTask(); }

};

NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREAD_H__
