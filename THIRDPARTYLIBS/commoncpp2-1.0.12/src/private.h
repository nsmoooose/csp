
#ifdef CCXX_NAMESPACES
namespace ost {
#endif

class ThreadImpl
{
friend class Thread;
friend class DummyThread;
friend class PosixThread;
friend class Slog;
friend CCXX_EXPORT(Thread::Throw) getException(void);
friend CCXX_EXPORT(void) setException(Thread::Throw mode);

	ThreadImpl(int type):
		_msgpos(0),
		_throw(Thread::throwObject),
		_suspendEnable(true),
		_type(type),
		_tid(0)
#ifndef WIN32
#else
		,
		_active(false),
		_hThread(NULL),
		_cancellation(NULL)
#endif
	{ ; };

	// derived class copy constructor creates new instance, so base
	// copy constructor of ThreadImpl should do nothing...

	ThreadImpl(const ThreadImpl& copy)
		{;};

	ThreadImpl &operator=(const ThreadImpl& copy)
		{return *this;};

#ifndef WIN32
	pthread_attr_t _attr;
	AtomicCounter _suspendcount;
	static ThreadKey _self;
#else
	HANDLE _cancellation;
#endif
	// log information
	int _msgpos;
	char _msgbuf[128];
	enum Thread::Throw _throw;

#ifndef WIN32
	friend Thread *getThread(void);
	volatile bool _suspendEnable:1;
	unsigned int _type:3;
#else
	bool _active:1;
	bool _suspendEnable:1;
	unsigned int _type:3;
	static unsigned __stdcall Execute(Thread *th);
	HANDLE	_hThread;
#endif
	cctid_t _tid;

public:
	// C binding functions
	static inline void ThreadExecHandler(Thread* th);
#ifndef WIN32
	static inline RETSIGTYPE ThreadSigSuspend(int);
	static inline void ThreadCleanup(Thread* arg);
	static inline void ThreadDestructor(Thread* arg);
	static inline void PosixThreadSigHandler(int signo);
#endif
};

#ifdef CCXX_NAMESPACES
}
#endif

