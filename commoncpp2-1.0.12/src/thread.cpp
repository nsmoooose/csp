// Copyright (C) 1999-2003 Open Source Telecom Corporation.
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
//
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/thread.h>
#include "private.h"

#ifdef CCXX_HAVE_NEW_INIT
#include <new>
#else
inline void* operator new(size_t s,void* p)
{ return p;}
#endif

#ifdef WIN32
#include <process.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifdef	_THR_UNIXWARE
#undef	_POSIX_THREAD_PRIORITY_SCHEDULING
#define	sigwait(x, y) _thr_sigwait(x, y)
#endif

#ifdef  __linux__
#define CCXX_SIG_THREAD_ALARM
// NOTE: Comment this line to test Resume/Signal using one signal method
#define CCXX_SIG_THREAD_STOPCONT
#endif

#ifndef WIN32
typedef	void	*(*exec_t)(void *);
typedef	RETSIGTYPE (*signalexec_t)(int);
extern "C"
{
#ifndef CCXX_SIG_THREAD_STOPCONT
#ifndef _THR_SUNOS5
#ifndef HAVE_PTHREAD_SUSPEND
static RETSIGTYPE ccxx_sigsuspend(int);
#endif
#endif
#endif

static void ccxx_exec_handler(Thread *th);
static void ccxx_thread_cleanup(void* arg);
static void ccxx_thread_destructor(void* arg);
static void ccxx_sig_handler(int signo);
}

#endif // ndef WIN32

#ifdef	CCXX_SIG_THREAD_CANCEL

extern "C" static RETSIGTYPE sigcancel(int)
{
	pthread_exit(NULL);
}

#endif

#ifdef WIN32
typedef	unsigned (__stdcall *exec_t)(void *);
#if defined(_MSC_VER) && !defined(_DLL)
# error This project cannot be compiled as a static library. Some implementation stuff require DLL
#endif
#endif // WIN32

/*
 * Start Suspend/Resume stuff
 */

// method to suspend are
// - system suspend/resume recursive
// - system suspend/resume not recursive
// - one signal only, not recursive
#ifndef WIN32
#define CCXX_SUSPEND_MODE_RECURSIVE 1
#define CCXX_SUSPEND_MODE_NOT_RECURSIVE 2
#define CCXX_SUSPEND_MODE_ONE_SIGNAL 3

#ifdef HAVE_PTHREAD_SUSPEND
#define CCXX_SUSPEND_MODE CCXX_SUSPEND_MODE_NOT_RECURSIVE
static inline void ccxx_resume(cctid_t tid) { pthread_continue(tid); }
static inline void ccxx_suspend(cctid_t tid) { pthread_suspend(tid); }
#else
# if defined(_THR_SUNOS5) || defined(CCXX_SIG_THREAD_STOPCONT)
#  define CCXX_SUSPEND_MODE CCXX_SUSPEND_MODE_NOT_RECURSIVE
#  ifdef _THR_SUNOS5
    static inline void ccxx_resume(cctid_t tid) { thr_continue((thread_t)tid); }
    static inline void ccxx_suspend(cctid_t tid) { thr_suspend((thread_t)tid); }
#  else
#   define CCXX_SIG_THREAD_SUSPEND SIGSTOP
#   define CCXX_SIG_THREAD_RESUME  SIGCONT
    static inline void ccxx_resume(cctid_t tid) { 
	    pthread_kill(tid, CCXX_SIG_THREAD_RESUME); 
    }
    static inline void ccxx_suspend(cctid_t tid) { 
	    pthread_kill(tid, CCXX_SIG_THREAD_SUSPEND); 
    }
#  endif
# else
#  define CCXX_SUSPEND_MODE CCXX_SUSPEND_MODE_ONE_SIGNAL
#  ifndef SIGUSR3
#  ifdef  SIGWINCH
#  define SIGUSR3 SIGWINCH
#  else
#  define SIGUSR3 SIGINT
#  endif
#  endif
#  define CCXX_SIG_THREAD_SUSPEND SIGUSR3
#  define CCXX_SIG_THREAD_RESUME SIGUSR3
   static inline void ccxx_resume(cctid_t tid) { 
	   pthread_kill(tid, CCXX_SIG_THREAD_RESUME); 
   }
   static inline void ccxx_suspend(cctid_t tid) { 
	   pthread_kill(tid, CCXX_SIG_THREAD_SUSPEND); 
   }
# endif
#endif
#endif // ndef WIN32

void Thread::suspend(void)
{
	if(!priv)
		return;

#ifdef WIN32
	if (!priv->_active || !priv->_suspendEnable)
	{
		if (getException() != throwNothing)
			throw this;
		return;
	}
	SuspendThread(priv->_hThread);

#else

	if (!priv->_suspendEnable) return;
#if CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_RECURSIVE
	ccxx_suspend(priv->_tid);
#endif
#if (CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_NOT_RECURSIVE) \
  || (CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_ONE_SIGNAL)
	if (++priv->_suspendcount != 1) return;
	ccxx_suspend(priv->_tid);
#endif

#endif // WIN32
}

void Thread::resume(void)
{
	if(!priv)
		return;

#ifdef WIN32
	if (!priv->_active || !priv->_suspendEnable)
	{
		if (getException() != throwNothing)
			throw this;
		return;
	}
	ResumeThread(priv->_hThread);

#else
	if (!priv->_suspendEnable) return;
#if CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_RECURSIVE
	ccxx_resume(priv->_tid);
#endif
#if (CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_NOT_RECURSIVE) \
  || (CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_ONE_SIGNAL)
	int c;
	if ( (c = --priv->_suspendcount) > 0) return;
	if ( c < 0 ) 
	{
		++priv->_suspendcount;
		return;
	}
	ccxx_resume(priv->_tid);
#endif

#endif // WIN32
}

void Thread::join(void)
{
	joinSem.wait();
}

#ifndef WIN32
#if CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_ONE_SIGNAL
// NOTE: Do not modify _suspendcount here, one program can call
// Suspend 2 or more time but this function can be called only once
inline RETSIGTYPE ThreadImpl::ThreadSigSuspend(int)
{
	sigset_t sigs;

	sigemptyset(&sigs);
	sigaddset(&sigs, SIGUSR3);
	while ( (getThread()->priv->_suspendcount) > 0)
	{
#ifdef	HAVE_SIGWAIT2
		int signo;
		sigwait(&sigs, &signo);
#else
		sigwait(&sigs);
#endif
	}
}

static RETSIGTYPE ccxx_sigsuspend(int signo)
{
	return ThreadImpl::ThreadSigSuspend(signo);
}
#endif

void	Thread::setSuspend(Suspend mode)
{
	if(!priv)
		return;

	priv->_suspendEnable = (mode == suspendEnable);
#ifndef	HAVE_PTHREAD_SUSPEND
#ifdef  CCXX_SIG_THREAD_SUSPEND
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, CCXX_SIG_THREAD_SUSPEND);

	switch(mode)
	{
	case suspendEnable:
		pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
		return;
	case suspendDisable:
		pthread_sigmask(SIG_BLOCK, &mask, NULL);
	}
#endif
#endif
}

/*
 * End Suspend/Resume stuff
 */

static	sigset_t *blocked_signals(sigset_t *sig)
{
	sigemptyset(sig);
	sigaddset(sig, SIGINT);
	sigaddset(sig, SIGKILL);
	sigaddset(sig, SIGHUP);
	sigaddset(sig, SIGABRT);
	sigaddset(sig, SIGALRM);
	sigaddset(sig, SIGPIPE);
#if CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_ONE_SIGNAL
	sigaddset(sig, SIGUSR3);
#endif
	return sig;
}
#endif // ndef WIN32

typedef enum ThreadType { 
	threadTypeNormal=0, 
	threadTypeMain, 
	threadTypePosix,
	threadTypeDummy
} ThreadType;

class MainThread : public Thread
{
protected:
	void run(void) {return;};
#ifndef WIN32
	void onSignal(int signo) { ::exit(signo);};
#endif

public:
	MainThread() : Thread(true) {};
};

// mantain info on thread creation
class DummyThread : public Thread
{
protected:
	void run() {};
public:
	DummyThread() : Thread(false) { priv->_type = threadTypeDummy; }
#ifdef WIN32
	static void CheckDelete();
#endif
};

#ifdef WIN32
static ThreadKey _self;
#else
// NOTE: _self instantiation MUST appear before _mainthread !!
ThreadKey ThreadImpl::_self(ccxx_thread_destructor);
#endif

#ifdef WIN32
void DummyThread::CheckDelete()
{
	Thread *th = (Thread*)_self.getKey();
	if (!th) return;

	// delete if dummy thread
	if (th->priv->_type == threadTypeDummy)
		delete th;
}
#endif

static	MainThread _mainthread;
Thread	*Thread::_main = NULL;

// invalid pointer to thread used to test deleted thread
// point in the middle of mainthread...
#define DUMMY_INVALID_THREAD ((Thread*)(((char*)((Thread*)&_mainthread))+1))

#if !defined(WIN32)
#ifndef	CCXX_SIG_THREAD_ALARM
PosixThread *PosixThread::_timer = NULL;
Mutex	PosixThread::_arm;
#endif
#endif

void sigInstall(int);

Thread::Thread(bool isMain):
	_cancel(cancelDefault),
	_start(NULL),
	priv(new ThreadImpl(threadTypeDummy))
{
#ifdef WIN32
	priv->_tid = GetCurrentThreadId();

	// FIXME: error handling
	HANDLE process = GetCurrentProcess();
	DuplicateHandle(process,GetCurrentThread(),process,&priv->_hThread,0,FALSE,DUPLICATE_SAME_ACCESS);
	_parent = this;
	priv->_cancellation = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if(isMain)
	{
		priv->_type   = threadTypeMain;
		_main = this;
	}
	_self.setKey(this);

#else
	priv->_suspendEnable = false;
	priv->_tid = pthread_self();
	_parent = NULL;
	struct sigaction act;
	
	// NOTE: for race condition (signal handler can use getThread)
	// you should initialize _main and _self before registering signals
	ThreadImpl::_self.setKey(this);
	if(isMain == true)
	{
		_main = this;
		priv->_type = threadTypeMain;
#if !defined(__CYGWIN32__) && !defined(__MINGW32__)
		sigInstall(SIGHUP);
		sigInstall(SIGALRM);
		sigInstall(SIGPIPE);
		sigInstall(SIGABRT);

		act.sa_handler = (signalexec_t)&ccxx_sig_handler;
		sigemptyset(&act.sa_mask);
# ifdef	SA_RESTART
		act.sa_flags = SA_RESTART;
# else
		act.sa_flags = 0;
# endif
# ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
# endif
# ifdef	SIGPOLL
		sigaction(SIGPOLL, &act, NULL);
# else
		sigaction(SIGIO, &act, NULL);
# endif		

# if CCXX_SUSPEND_MODE == CCXX_SUSPEND_MODE_ONE_SIGNAL
		act.sa_handler = ccxx_sigsuspend;
		sigemptyset(&act.sa_mask);
#  ifdef	SA_RESTART
		act.sa_flags = SA_RESTART;
#  else
		act.sa_flags = 0;
#  endif
		sigaction(SIGUSR3, &act, NULL);
# endif

# ifdef	CCXX_SIG_THREAD_CANCEL
		act.sa_flags = 0;
		act.sa_handler = sigcancel;
		sigemptyset(&act.sa_mask);
		sigaddset(&act.sa_mask, SIGHUP);
		sigaddset(&act.sa_mask, SIGALRM);
		sigaddset(&act.sa_mask, SIGPIPE);

		sigaction(CCXX_SIG_THREAD_CANCEL, &act, NULL);
# endif
#endif
    	}

#endif // WIN32
}	

Thread::Thread(int pri, size_t stack):
	_cancel(cancelDefault),
	_start(NULL),
	priv(new ThreadImpl(threadTypeNormal))
{
#ifdef WIN32
	if(!_main)
	{
		_self.setKey(NULL);
		_main = this;
	}

	_parent = getThread();
	if(_parent)
		priv->_throw = _parent->priv->_throw;
	else
		_parent = this;

	priv->_cancellation = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!priv->_cancellation)
		throw(this);

	priv->_hThread = (HANDLE)_beginthreadex(NULL, stack, (exec_t)&Execute, (void *)this, CREATE_SUSPENDED, (unsigned *)&priv->_tid);
	if(!priv->_hThread)
	{
		CloseHandle(priv->_cancellation);
		priv->_cancellation = NULL;
		throw(this);
	}

	setCancel(cancelInitial);

	switch(pri)
	{
	case 1:
		pri = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case -1:
		pri = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case 2:
		pri = THREAD_PRIORITY_HIGHEST;
		break;
	case -2:
		pri = THREAD_PRIORITY_LOWEST;
		break;
	default:
		pri = THREAD_PRIORITY_NORMAL;
	}

	SetThreadPriority(priv->_hThread, pri);

#else
//	int	rc;

	pthread_attr_init(&priv->_attr);
	pthread_attr_setdetachstate(&priv->_attr, PTHREAD_CREATE_JOINABLE);

#ifdef	PTHREAD_STACK_MIN
	if(stack && pthread_attr_setstacksize(&priv->_attr, stack <= PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : stack))
	{
		switch(getException())
		{
		case throwObject:
			throw(this);
			return;
#ifdef	COMMON_STD_EXCEPTION
		case throwException:
			throw(ThrException("no stack space"));
			return;
#endif
		default:
			return;
		}
	}
#endif

#ifndef	__FreeBSD__
#ifdef	_POSIX_THREAD_PRIORITY_SCHEDULING
#ifdef HAVE_SCHED_GETSCHEDULER
#define	__HAS_PRIORITY_SCHEDULING__	
	if(pri)
	{
		struct sched_param sched;
		int policy;

		policy = sched_getscheduler(0);
		if(policy < 0)
		{
			switch(getException())
			{
			case throwObject:
				throw(this);
				return;
#ifdef	COMMON_STD_EXCEPTION
			case throwException:
				throw(ThrException("invalid scheduler"));
				return;
#endif
			default:
				return;
			}
		}

		sched_getparam(0, &sched);

		pri = sched.sched_priority - pri;
		if(pri 	> sched_get_priority_max(policy))
			pri = sched_get_priority_max(policy);

		if(pri < sched_get_priority_min(policy))
			pri = sched_get_priority_min(policy);

		sched.sched_priority = pri;
		pthread_attr_setschedpolicy(&priv->_attr, policy);
		pthread_attr_setschedparam(&priv->_attr, &sched);
	}	
#endif // ifdef HAVE_SCHED_GETSCHEDULER
#endif // ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
#endif // ifndef __FreeBSD__

#ifdef	__HAS_PRIORITY_SCHEDULING__
	if(!pri)
	        pthread_attr_setinheritsched(&priv->_attr, PTHREAD_INHERIT_SCHED);             
#else
        pthread_attr_setinheritsched(&priv->_attr, PTHREAD_INHERIT_SCHED);             
#endif

	_parent = getThread();
	priv->_throw = _parent->priv->_throw;

	_cancel = cancelInitial;

#endif // WIN32
};

#ifndef WIN32
Thread::Thread(const Thread &th)
{
	priv = new ThreadImpl(threadTypeNormal);
	_parent = th._parent;
	// FIXME: correct ??
	priv->_attr = th.priv->_attr;
	_cancel = cancelInitial;
	_start = NULL;
	priv->_throw = th.priv->_throw;
	priv->_suspendEnable = false;
	
//	sigset_t mask, newmask;
//	int rc;
//
//	pthread_sigmask(SIG_BLOCK, blocked_signals(&newmask), &mask);
//	rc = pthread_create(&_tid, &_attr, exec_t(&ccxx_exec_handler), this);
//	pthread_sigmask(SIG_SETMASK, &mask, NULL);
//	if(rc && getException() == throwObject)
//		throw(this);
//#ifdef	COMMON_STD_EXCEPTION
//	else if(rc && getException() == throwException)
//		throw(ThrException("cannot start copy"));
//#endif
}
#endif // ndef WIN32

Thread::~Thread()
{
//	was for 1.1.x where _msgbuf was dynamic
//	if(priv)
//		if( priv->_msgbuf != NULL)
//			free(priv->_msgbuf);
#ifdef WIN32
	terminate();
#else
	if(this != &_mainthread)
		terminate();
#endif // WIN32
}

bool Thread::isThread(void)
{
	if(!priv)
		return false;

#ifdef WIN32
	return ((priv->_tid == GetCurrentThreadId())) ? true : false;
#else
	return (priv->_tid == pthread_self()) ? true : false;
#endif
}

bool Thread::isDetached(void)
{
        if(!priv)
                return false;

#ifdef  WIN32
        return false;   // win32 doesn't support detached threads directly
#else
        int state;

        pthread_attr_getdetachstate(&priv->_attr, &state);
        if(state == PTHREAD_CREATE_DETACHED)
                return true;
        return false;
#endif
}

bool Thread::isRunning(void)
{
	if(!priv)
		return false;
#ifdef WIN32
	return (priv->_tid != 0 && priv->_active) ? true : false;
#else
	return (priv->_tid != 0) ? true : false;
#endif // WIN32
}

int Thread::start(Semaphore *start)
{
	if(!priv)
		return -1;

#ifdef WIN32
	_start = start;
	if (!priv->_active)
	{
		priv->_active = true;
		ResumeThread(priv->_hThread);
	}
	return 0;

#else
	if(priv->_tid)
	{
		if(_start)
		{
			_start->post();
			return 0;
		}
		else
			return -1;
	}

	_start = start;
	return pthread_create(&priv->_tid, &priv->_attr, exec_t(&ccxx_exec_handler), this);
#endif
}

int Thread::detach(Semaphore *start)
{
	_parent = NULL;
#ifdef WIN32
	// win32 do not support detach	
	if(start)
		start->post();
	return 0;
#else
	int rtn;
	if(!priv)
		return -1;

	if(priv->_tid)
	{
		pthread_detach(priv->_tid);
		if(_start)
		{	
			_start->post();
		        pthread_attr_setdetachstate(&priv->_attr, PTHREAD_CREATE_DETACHED);
			return 0;
		}
		return -1;
	}

	pthread_attr_setdetachstate(&priv->_attr, PTHREAD_CREATE_DETACHED);

	_start = start;
	rtn = pthread_create(&priv->_tid, &priv->_attr, exec_t(&ccxx_exec_handler), this);
	if(!rtn && priv->_tid)
		return 0;
	return -1;
#endif
}

void Thread::terminate(void)
{
#ifdef WIN32
	if(!priv)
		return;

	if (!priv->_tid || isThread())
		return;

	bool terminated = false;
	if(!priv->_active)
	{
		// NOTE: add a test in testthread for multiple
		// suspended Terminate
		ResumeThread(priv->_hThread);
		TerminateThread(priv->_hThread, 0);
		terminated = true;
	}
	else
	{
		switch(_cancel)	{
		case cancelImmediate:
			TerminateThread(priv->_hThread, 0);
			terminated = true;
			break;
		default:
			SetEvent(priv->_cancellation);
		}
	}
	WaitForSingleObject(priv->_hThread, INFINITE);
	if(_parent)
		_parent->notify(this);
	// FIXME: test race condition, a thread can be restarted
	CloseHandle(priv->_hThread);
	priv->_hThread = NULL;
	CloseHandle(priv->_cancellation);
	priv->_cancellation = NULL;
	priv->_tid = 0;
	_self.setKey(DUMMY_INVALID_THREAD);
	delete priv;
	priv = NULL;

	if (terminated)
		final();
#else
	if(!priv)
		return;
	
	if(!priv->_tid)
		return;

	if(pthread_self() != priv->_tid)
	{
		// in suspend thread cannot be cancelled or signaled
		// ??? rigth
		// ccxx_resume(priv->_tid);
		
		// assure thread has ran before we try to cancel...
		if(_start)
			_start->post();

		pthread_cancel(priv->_tid);
		if(!isDetached())
			pthread_join(priv->_tid,NULL);
	}

        // called terminate this on current thread ??
        if (isThread())
        {
                priv->_tid = 0;
                ThreadImpl::_self.setKey(DUMMY_INVALID_THREAD);
        }
        pthread_attr_destroy(&priv->_attr);
	delete priv;
	priv = NULL;
#endif
}

void Thread::exit()
{
	if (isThread())
	{
		setCancel(cancelDisabled);
#ifdef WIN32
		close();
		ExitThread(0);
#else
		pthread_exit(NULL);
#endif // WIN32
	}
}
		
void Thread::close()
{
#if !defined(CCXX_SIG_THREAD_ALARM) && !defined(__CYGWIN32__) && !defined(__MINGW32__) && !defined(WIN32)
	if(this == PosixThread::_timer)
		PosixThread::_arm.leaveMutex();
#endif
	setCancel(cancelDisabled);
	if(_parent)
		_parent->notify(this);
      
	// final can call destructor (that call Terminate)
	final();

	// test if this class have been deleted
#ifdef WIN32
	if (_self.getKey() == this)
#else
	if (ThreadImpl::_self.getKey() == this)
#endif
	{
		if(priv)
			priv->_tid = 0;
		joinSem.post();
	}
}

#ifndef WIN32
inline void ThreadImpl::ThreadCleanup(Thread* th)
{
	// close thread
	// (freddy77) Originally I thougth to throw an exception for deferred
	// for capture it and cleanup using C++ destructor
	// this doesn't work out!! 
	// Throwing exception here (in cleanup) core dump app
	th->close();
}
static void ccxx_thread_cleanup(void* arg)
{
	ThreadImpl::ThreadCleanup( (Thread*)arg );
}

inline void ThreadImpl::ThreadExecHandler(Thread *th)
{
	ThreadImpl::_self.setKey(th);
	sigset_t mask;

	pthread_sigmask(SIG_BLOCK, blocked_signals(&mask), NULL);
	th->priv->_tid = pthread_self();
	th->setCancel(Thread::cancelInitial);
	// using SIGUSR3 do not enable suspend by default
	th->setSuspend(Thread::suspendEnable);
	th->yield();
	if(th->_start)
	{
		th->_start->wait();
		th->_start = NULL;
	}

	pthread_cleanup_push(ccxx_thread_cleanup,th);
	th->initial();
	if(th->getCancel() == Thread::cancelInitial)
		th->setCancel(Thread::cancelDefault);
	th->run();
	th->setCancel(Thread::cancelDisabled);

	pthread_cleanup_pop(0);
	th->close();
	pthread_exit(NULL);
}

// delete Thread class created for no CommonC++ thread
inline void ThreadImpl::ThreadDestructor(Thread* th)
{
	if (!th || th == DUMMY_INVALID_THREAD)
		return;
	if (th->priv->_type == threadTypeDummy)
		delete th;
}

static void ccxx_thread_destructor(void* arg)
{
	ThreadImpl::ThreadDestructor( (Thread*)arg );
}

static void ccxx_exec_handler(Thread *th)
{
	ThreadImpl::ThreadExecHandler(th);
}
#endif // ndef WIN32

#ifdef	CCXX_SIG_THREAD_CANCEL

void	Thread::setCancel(thread_cancel_t mode)
{
	sigset_t	mask;

	sigemptyset(&mask);
	sigaddset(&mask, CCXX_SIG_THREAD_CANCEL);
	
	switch(mode)
	{
	case cancelImmediate:
		pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
		break;
	case cancelInitial:
	case cancelDisabled:
	case cancelDeferred:
		pthread_sigmask(SIG_BLOCK, &mask, NULL);
		break;
	}
	_cancel = mode;
}
#else

void	Thread::setCancel(Cancel mode)
{
#ifdef WIN32
	switch(mode)	
	{
	case cancelDeferred:
	case cancelImmediate:
		_cancel = mode;
		yield();
		break;
	case cancelDisabled:
	case cancelInitial:
		_cancel = mode;
	}

#else
	int old;

	switch(mode)
	{
	case cancelImmediate:
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old);
		break;
	case cancelDeferred:
		pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old);
		break;
	case cancelInitial:
	case cancelDisabled:
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);
		break;
	default:
		return;
	}
	_cancel = mode;
#endif // WIN32
}

#endif

void	Thread::yield(void)
{
#ifdef WIN32
	Thread::sleep(1);   // note: on Win32, Sleep(0) is "optimized" to NOP.
#else

#ifdef	CCXX_SIG_THREAD_CANCEL
	Thread* th = getThread();
	sigset_t cancel, old;

	sigemptyset(&cancel);
	sigaddset(&cancel, CCXX_SIG_THREAD_CANCEL);

	if(th && th->_cancel != cancelDisabled && 
	   th->_cancel != cancelInitial)
		pthread_sigmask(SIG_UNBLOCK, &cancel, &old);
#else
	pthread_testcancel();
#endif
#ifdef	HAVE_PTHREAD_YIELD
	pthread_yield();
#endif

#ifdef	CCXX_SIG_THREAD_CANCEL
	if(th && th->_cancel != cancelDisabled)
		pthread_sigmask(SIG_SETMASK, &old, NULL);
#endif

#endif // WIN32
}

bool Thread::testCancel(void)
{
#ifdef WIN32
	switch(_cancel)
	{
	case cancelInitial:
	case cancelDisabled:
		break;
	default:
		if(WaitForSingleObject(priv->_cancellation, 0) == WAIT_OBJECT_0)
		{
			if (_cancel == cancelManual)
				throw InterruptException();
			else
				exit();
		}
	}
	return false;

#else // WIN32

#ifdef  CCXX_SIG_THREAD_CANCEL
        sigset_t cancel, old;
 
        sigemptyset(&cancel);
        sigaddset(&cancel, CCXX_SIG_THREAD_CANCEL);
 
        if(_cancel != cancelDisabled &&
           _cancel != cancelInitial)
                pthread_sigmask(SIG_UNBLOCK, &cancel, &old);
#else
        pthread_testcancel();
#endif
 
#ifdef  CCXX_SIG_THREAD_CANCEL
        if(_cancel != cancelDisabled)
                pthread_sigmask(SIG_SETMASK, &old, NULL);
#endif
	return false;

#endif // WIN32
} 

#ifdef WIN32

bool Thread::isCancelled()
{
	return waitThread(priv->_cancellation, 0) == WAIT_OBJECT_0;
}

CCXX_EXPORT(DWORD) waitThread(HANDLE hRef, timeout_t timeout)
{
	Thread *th = getThread();

	if(th)
		return th->waitHandle(hRef, timeout);
	else
		return WaitForSingleObject(hRef, timeout);
}

void Thread::sleep(timeout_t timeout) 
{
	Thread *th = getThread();
	if(!th)
	{
		SleepEx(timeout, FALSE);
		return;
	}

	switch(th->_cancel)
	{
	case cancelInitial:
	case cancelDisabled:
		SleepEx(timeout, FALSE);
		break;
	default:
		if(WaitForSingleObject(th->priv->_cancellation, timeout) == WAIT_OBJECT_0)
		{
			if (th->_cancel == cancelManual)
				throw InterruptException();
			else
				th->exit();
		}
	}
}

DWORD Thread::waitHandle(HANDLE obj, timeout_t timeout) 
{
	HANDLE objects[2];
	DWORD stat;

	objects[0] = priv->_cancellation;
	objects[1] = obj;

	// FIXME: what should happen if someone enable cancellation on wait??
	switch(_cancel)
	{
	case cancelInitial:
	case cancelDisabled:
		return WaitForSingleObject(obj, timeout);
	default:
		switch(stat = WaitForMultipleObjects(2, objects, false, timeout)) 	{
		case WAIT_OBJECT_0:
			if (_cancel == cancelManual)
				throw InterruptException();
			else
				exit();
		case WAIT_OBJECT_0 + 1:
			return WAIT_OBJECT_0;
		default:
			return stat;
		}
	}
}

// Entry point linked for default disable thread call, not suitable
// for threading library...
BOOL WINAPI DllMain(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        )
{
	switch(dwReason)
	{
	case DLL_THREAD_DETACH:
		DummyThread::CheckDelete();
		break;
	}
        return TRUE ;
}

#endif // WIN32

#ifndef WIN32
Thread *getThread(void)
{
	Thread *thread;
	
	// fix strange no-init on Solaris
        if(!Thread::_main)
	{
		new (&_mainthread) MainThread();
		return &_mainthread;
	}

	thread = (Thread *)ThreadImpl::_self.getKey();

	// class have been deleted, return NULL
	if (thread == DUMMY_INVALID_THREAD)
		return NULL;

	if(!thread)
	{
		// this Thread will be deleted by ccxx_thread_destruct
		thread = new DummyThread;
		ThreadImpl::_self.setKey(thread);
	}
	return thread;
}

#else // WIN32

CCXX_EXPORT(Thread*) getThread(void)
{
	Thread *th = (Thread *)_self.getKey();
	if (th == DUMMY_INVALID_THREAD) return NULL;
	// for no common c++ thread construct a dummy thread
	if (!th)
		th = new DummyThread();
	return th;
}

unsigned __stdcall Thread::Execute(Thread *th)
{
	_self.setKey(th);
	th->yield();
	if(!th->priv->_active)
	{
		th->setCancel(cancelImmediate);
		th->yield();
		return 0;	
	}
	if(th->_start)
	{
		th->_start->wait();
		th->_start = NULL;
	}

	try
	{
		th->initial();
		if(th->getCancel() == cancelInitial) 
			th->setCancel(cancelDefault);
		th->run();
	}
	// ignore cancellation exception
	catch(const InterruptException&)
	{ ; }

	th->close();
	return 0;
}
#endif //WIN32

#if !defined(WIN32)
/*
 * PosixThread implementation
 */
inline void ThreadImpl::PosixThreadSigHandler(int signo)
{
	Thread	*t = getThread();
	PosixThread *th = NULL;
	if (t) th = dynamic_cast<PosixThread*>(t);
	if (!th) return;

	switch(signo)
	{
	case SIGHUP:
		if(th)
			th->onHangup();
		break;
	case SIGABRT:
		if(th)
			th->onException();
		break;
	case SIGPIPE:
		if(th)
			th->onDisconnect();
		break;
	case SIGALRM:
#ifndef	CCXX_SIG_THREAD_ALARM
		if(PosixThread::_timer)
		{
			PosixThread::_timer->_alarm = 0;
			PosixThread::_timer->onTimer();
		}
		else
#endif 
		    if(th)
			th->onTimer();
		break;
#ifdef	SIGPOLL
	case SIGPOLL:
#else
	case SIGIO:
#endif
		if(th)
			th->onPolling();
		break;
	default:
		if(th)
			th->onSignal(signo);
	}
}
static void ccxx_sig_handler(int signo)
{
	ThreadImpl::PosixThreadSigHandler(signo);
}

PosixThread::PosixThread(int pri, size_t stack):
	Thread(pri,stack)
{
	time(&_alarm);
}

void PosixThread::setTimer(timeout_t timer, bool periodic)
{
	sigset_t sigs;

#ifdef	HAVE_SETITIMER
	struct itimerval itimer;
	
	memset(&itimer, 0, sizeof(itimer));
	itimer.it_value.tv_usec = (timer * 1000) % 1000000;
	itimer.it_value.tv_sec = timer / 1000;
	if (periodic) {
		itimer.it_interval.tv_usec = itimer.it_value.tv_usec;
		itimer.it_interval.tv_sec = itimer.it_value.tv_sec;
	}
#else
	timer /= 1000;
#endif

#ifndef	CCXX_SIG_THREAD_ALARM	
	_arm.enterMutex();
	_timer = this;
#endif
	time(&_alarm);
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGALRM);
	pthread_sigmask(SIG_UNBLOCK, &sigs, NULL);
#ifdef	HAVE_SETITIMER
	setitimer(ITIMER_REAL, &itimer, NULL);
#else
	alarm(timer);
#endif
}	

timeout_t PosixThread::getTimer(void) const
{
#ifdef	HAVE_SETITIMER
	struct itimerval itimer;
#endif

	if(!_alarm)
		return 0;

#ifdef	HAVE_SETITIMER
	getitimer(ITIMER_REAL, &itimer);
	return (timeout_t)(itimer.it_value.tv_sec * 1000 +
		itimer.it_value.tv_usec / 1000);
#else
	time_t now;
	time(&now);
	return (timeout_t)(((now - _alarm) * 1000) + 500);
#endif
}

void PosixThread::endTimer(void)
{
#ifdef	HAVE_SETITIMER
	static const struct itimerval itimer = {{0, 0},{0,0}};
#endif

	sigset_t sigs;
#ifndef	CCXX_SIG_THREAD_ALARM
	if(_timer != this)
		return;
#endif

#ifdef	HAVE_SETITIMER
	setitimer(ITIMER_REAL, (struct itimerval *)&itimer, NULL);
#else
	alarm(0);
#endif
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &sigs, NULL);
#ifndef	CCXX_SIG_THREAD_ALARM
	_arm.leaveMutex();
	_timer = NULL;
#endif
}

#ifdef HAVE_SIGWAIT
void	PosixThread::waitSignal(signo_t signo)
{
	sigset_t	mask;

	sigemptyset(&mask);
	sigaddset(&mask, signo);
#ifndef HAVE_SIGWAIT2
	signo = sigwait(&mask);
#else
	sigwait(&mask, &signo);
#endif
}
#endif // ifdef HAVE_SIGWAIT

void	PosixThread::setSignal(int signo, bool mode)
{
	sigset_t sigs;

	sigemptyset(&sigs);
	sigaddset(&sigs, signo);

	if(mode)
		pthread_sigmask(SIG_UNBLOCK, &sigs, NULL);
	else
		pthread_sigmask(SIG_BLOCK, &sigs, NULL);
}	

void	PosixThread::signalThread(Thread* th,signo_t signo)
{
	pthread_kill(th->priv->_tid, signo);
}

void sigInstall(int signo)
{
	struct sigaction act;
	
	act.sa_handler = (signalexec_t)&ccxx_sig_handler;
	sigemptyset(&act.sa_mask);

#ifdef	SA_INTERRUPT
	act.sa_flags = SA_INTERRUPT;
#else
	act.sa_flags = 0;
#endif
	sigaction(signo, &act, NULL);
}
#endif

#ifdef	CCXX_USE_POLL

Poller::Poller()
{
	nufds = 0;
	ufds = NULL;
};

Poller::~Poller()
{
	if(ufds)
		delete[] ufds;
}

pollfd *Poller::getList(int cnt)
{
	if(nufds < cnt)
	{
		if(ufds)
			delete[] ufds;
		ufds = new pollfd[cnt];
		nufds = cnt;
	}
	return ufds;
}

#endif

// C stuff
// this function must declared as extern "C" for some compiler

#ifdef CCXX_NAMESPACES
};
#endif
 
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */

