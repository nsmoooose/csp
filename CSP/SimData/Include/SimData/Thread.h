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
	 *  requests.  @see cancel().
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

	/* Set the cancellation type to be deferred.  Cancellation requests will be
	 * deferred until a cancellation checkpoint is reached (e.g a blocking wait
	 * or an explicit call to testCancel()).
	 */
	static void useDeferredCancel() {
		const int result = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
		ThreadException::check(result);
	}

	/** Suspend execution of the current thread the specified amount of time.
	 *
	 *  @param seconds seconds to pause
	 *  @param nanosecondns nanoseconds to pause (in addition to seconds)
	 */
	static void sleep(int seconds, int nanoseconds=0) {
		static ThreadCondition m_condition;
		m_condition.lock();
		m_condition.wait(seconds + 1e-9 * nanoseconds);
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
	Task(): m_running(false), m_detached(false), m_complete(false), m_thread_id(0) {
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
		std::cout << getName() << " start\n";
		m_running = true;
		m_thread_id = pthread_self();
		run();
		std::cout << getName() << " exit\n";
		m_complete = true;
	}

	/** Mark this task as running in a detached thread.
	 */
	void setDetached() { m_detached = true; }

	/** Set the name of this task (arbitrary string identifier).
	 */
	void setName(std::string const &name) { m_name = name; }

	std::string m_name;
	bool m_running;
	bool m_detached;
	bool m_complete;
	ThreadId m_thread_id;
};


/** Object oriented wrapper for a Posix thread.
 *
 *  To create a thread, subclass Thread and implement the run() method.
 *  Then instantiate the class and call start().
 */
class BaseThread: public NonCopyable {

	BaseThread();

protected:

	BaseThread(Task *task, std::string const &name): m_task(task), m_thread_id(0) {
		assert(task);
		task->setName(name);
	}

public:
	/** Destroy a thread instance and free internal resources.
	 *
	 *  The thread will be cancelled if it is running and not detached.
	 */
	virtual ~BaseThread() {
		std::cout << "~BaseThread " << m_task->getName() << "\n";
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

	/*
	bool join(int sec, int ns) {
		pthread_joinoption_np_t joinoption;
		memset(&joinoption, 0, sizeof(pthread_joinoption_np_t));
		joinoption.deltatime.tv_sec = sec;
		joinoption.deltatime.tv_ns = ns;
		joinoption.leaveThreadAllocated = 1;
		int result = pthread_extendedjoin_np(m_thread_id, 0, &joinoption);
		if (result == 0) return true;
		if (result == ETIMEDOUT) return false;
		throw ThreadException(result);
	}
	*/

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
		assert(isStarted());
		const int result = pthread_cancel(m_thread_id);
		ThreadException::check(result);
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
};


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

};

NAMESPACE_SIMDATA_END

#endif // __SIMDATA_THREAD_H__
