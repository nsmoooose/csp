// Copyright (C) 1999-2002 Open Source Telecom Corporation.
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
#include <cc++/socket.h>
#include "private.h"
#ifndef WIN32
#include <cerrno>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifndef WIN32
TimerPort::TimerPort()
{
	active = false;
	gettimeofday(&timer, NULL);
}

void TimerPort::setTimer(timeout_t timeout)
{
	gettimeofday(&timer, NULL);
	active = false;
	if(timeout)
		incTimer(timeout);
}

void TimerPort::incTimer(timeout_t timeout)
{
	int secs = timeout / 1000;
	int usecs = (timeout % 1000) * 1000;

	timer.tv_usec += usecs;
	if(timer.tv_usec > 1000000l)
	{
		++timer.tv_sec;
		timer.tv_usec %= 1000000l;
	}
	timer.tv_sec += secs;
	active = true;
}

void TimerPort::endTimer(void)
{
	active = false;
}

timeout_t TimerPort::getTimer(void) const
{
	struct timeval now;
	long diff;

	if(!active)
		return TIMEOUT_INF;

	gettimeofday(&now, NULL);
	diff = (timer.tv_sec - now.tv_sec) * 1000l;
	diff += (timer.tv_usec - now.tv_usec) / 1000l;

	if(diff < 0)
		return 0l;

	return diff;
}

timeout_t TimerPort::getElapsed(void) const
{
	struct timeval now;
	long diff;

	if(!active)
		return TIMEOUT_INF;

	gettimeofday(&now, NULL);
	diff = (now.tv_sec -timer.tv_sec) * 1000l;
	diff += (now.tv_usec - timer.tv_usec) / 1000l;
	if(diff < 0)
		return 0;
	return diff;
}
#else // WIN32
TimerPort::TimerPort()
{
	active = false;
	timer = GetTickCount();
}

void TimerPort::setTimer(timeout_t timeout)
{
	timer = GetTickCount();
	active = false;
	if(timeout)
		incTimer(timeout);
}

void TimerPort::incTimer(timeout_t timeout)
{
	timer += timeout;
	active = true;
}

void TimerPort::endTimer(void)
{
	active = false;
}

timeout_t TimerPort::getTimer(void) const
{
	DWORD now;
	long diff;

	if(!active)
		return TIMEOUT_INF;

	now = GetTickCount();
	diff = timer - now;

	if(diff < 0)
		return 0l;

	return diff;
}

timeout_t TimerPort::getElapsed(void) const
{
	DWORD now;
	long diff;

	if(!active)
		return TIMEOUT_INF;

	now = GetTickCount();
	diff = now - timer;

	if(diff < 0)
		return 0l;

	return diff;
}
#endif

SocketPort::SocketPort(SocketService *svc, TCPSocket &tcp) :
	Socket(accept(tcp.so, NULL, NULL)),
	detect_pending(true),
	detect_output(false),
	detect_disconnect(true)
{
#ifdef WIN32
	// FIXME: error handling
	event = CreateEvent(NULL,TRUE,FALSE,NULL);
#endif
	next = prev = NULL;
	service = NULL;

	// FIXME: use macro here and in other files...
#ifndef WIN32
	if(so > -1)
#else
	if(so != INVALID_SOCKET)
#endif
	{
		setError(false);
		if(svc)
			svc->attach(this);
	}
}

SocketPort::SocketPort(SocketService *svc, const InetAddress &ia, 
		       tpport_t port) :
Socket(AF_INET, SOCK_DGRAM, 0)
{
#ifdef WIN32
	// FIXME: error handling
	event = CreateEvent(NULL,TRUE,FALSE,NULL);
#endif
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	next = prev = NULL;
	service = NULL;
	addr.sin_family = AF_INET;
	addr.sin_addr = getaddress(ia);
	addr.sin_port = htons(port);
	if(bind(so, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)))
	{
		endSocket();
		error(errBindingFailed);
		return;
	}
	state = BOUND;
	setError(false);
	if(svc)
		svc->attach(this);
}

SocketPort::SocketPort(SocketService *svc, const InetHostAddress &ih, 
		       tpport_t port) :
Socket(AF_INET, SOCK_STREAM, 0)
{
#ifdef WIN32
	// FIXME: error handling
	event = CreateEvent(NULL,TRUE,FALSE,NULL);
#endif
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	next = prev = NULL;
	service = NULL;
	addr.sin_family = AF_INET;
	addr.sin_addr = getaddress(ih);
	addr.sin_port = htons(port);

#ifndef WIN32
	long opts = fcntl(so, F_GETFL);
	fcntl(so, F_SETFL, opts | O_NDELAY);
#else
	u_long opts = 1;
	ioctlsocket(so,FIONBIO,&opts);
#endif

	int rtn = ::connect(so, (struct sockaddr *)&addr, (socklen_t)sizeof(addr));

	if(!rtn)
	{
		state = CONNECTED;
	}
	else
	{
#ifndef WIN32
		if(errno == EINPROGRESS)
#else
		if(WSAGetLastError() == WSAEINPROGRESS || WSAGetLastError() == WSAEWOULDBLOCK)
#endif
		{
			state = CONNECTING;
		}
		else
		{
			endSocket();
			connectError();
			return;
		}
	}

#ifndef WIN32
	fcntl(so, F_SETFL, opts);
#else
	opts = 0;
	ioctlsocket(so,FIONBIO,&opts);
#endif

	setError(false);
	if(svc)
		svc->attach(this);
        if(state == CONNECTING)
                setDetectOutput(true);
}

SocketPort::~SocketPort()
{
#ifdef WIN32
	CloseHandle(event);
#endif
	if(service) {
		service->detach(this);
	}
	endSocket();
}

void SocketPort::attach(SocketService *svc)
{
	if(service)
		service->detach(this);
	service = svc;
	if(svc)
		svc->attach(this);
}

Socket::Error SocketPort::connect(const InetAddress &ia, tpport_t port)
{
	struct sockaddr_in addr;
	Error rtn = errSuccess;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = getaddress(ia);
	addr.sin_port = htons(port);

#ifndef WIN32
	long opts = fcntl(so, F_GETFL);
	fcntl(so, F_SETFL, opts | O_NDELAY);
#else
	u_long opts = 1;
	ioctlsocket(so,FIONBIO,&opts);
#endif
	if(::connect(so, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)))
		rtn = connectError();
#ifndef WIN32
	fcntl(so, F_SETFL, opts);
#else
	opts = 0;
	ioctlsocket(so,FIONBIO,&opts);
#endif
	return rtn;
}


void SocketPort::setTimer(timeout_t ptimer)
{
	TimerPort::setTimer(ptimer);
	if(service)
		service->update();
}

void SocketPort::incTimer(timeout_t ptimer)
{
	TimerPort::incTimer(ptimer);
	if(service)
		service->update();
}

void SocketPort::setDetectPending( bool val )
{
	if ( detect_pending != val ) {
		detect_pending = val;
#ifdef CCXX_USE_POLL
		if ( ufd ) {
			if ( val ) {
				ufd->events |= POLLIN;
			} else {
				ufd->events &= ~POLLIN;
			}
		}
#endif
		if(service)
			service->update();
	}
}

void SocketPort::setDetectOutput( bool val )
{
	if ( detect_output != val ) {
		detect_output = val;
#ifdef CCXX_USE_POLL
		if ( ufd ) {
			if ( val ) {
				ufd->events |= POLLOUT;
			} else {
				ufd->events &= ~POLLOUT;
			}
		}
#endif
		if(service)
			service->update();
	}
}

#ifdef WIN32
class SocketService::Sync
{
public:
	/* FIXME: error handling */
	Sync():
		sync(CreateEvent(NULL,TRUE,FALSE,NULL)),
		semWrite(CreateSemaphore(NULL,1,1,NULL)),
		flag(-1)
	{ }
	~Sync() 
	{
		CloseHandle(sync);
		CloseHandle(semWrite);
	}
	HANDLE GetSync() const 
	{ 
		return sync; 
	}
	void update(unsigned char flag)
	{
		// FIXME: cancellation
		WaitForSingleObject(semWrite,INFINITE);
		this->flag = flag;
		SetEvent(sync);
	}
	int getFlag()
	{
		int res = flag;
		flag = -1;
		if (res > 0)
		{
			ReleaseSemaphore(semWrite,1,NULL);
			ResetEvent(sync);
		}
		return res;
	}
private:
	HANDLE sync;
	HANDLE semWrite;
	int flag;
};
#endif

SocketService::SocketService(int pri) :
Thread(pri), Mutex()
{
	first = last = NULL;
	count = 0;
#ifndef WIN32
	FD_ZERO(&connect);
	long opt;
	::pipe(iosync);
	hiwater = iosync[0] + 1;
	FD_SET(iosync[0], &connect);

	opt = fcntl(iosync[0], F_GETFL);
	fcntl(iosync[0], F_SETFL, opt | O_NDELAY);	
#else
	sync = new Sync();
#endif
}

SocketService::~SocketService()
{
	update(0);

#ifdef WIN32
	// FIXME: thread is finished ???
	delete sync;
#endif

	terminate();

	while(first)
		delete first;
}

void SocketService::attach(SocketPort *port)
{
	enterMutex();
#ifdef	CCXX_USE_POLL
	port->ufd = 0;
#endif
	if(last)
		last->next = port;

	port->prev = last;
	last = port;
#ifndef WIN32
	FD_SET(port->so, &connect);
	if(port->so >= hiwater)
		hiwater = port->so + 1;
#endif
	port->service = this;

	++count;
	if(!first) first = port;
	
	// start thread if necessary
	if (count == 1)
	{
		if (!isRunning())
		{
			leaveMutex();
			start();
			return;
		}
	}
	leaveMutex();
	update();
}

void SocketService::detach(SocketPort *port)
{
	enterMutex();
#if !defined(CCXX_USE_POLL) && !defined(WIN32)
	FD_CLR(port->so, &connect);
#endif
	if(port->prev) {
		port->prev->next = port->next;
	} else {
		first = port->next;
	}
	
	if(port->next) {
		port->next->prev = port->prev;
	} else {
		last = port->prev;
	}
	port->service = NULL;
	
	--count;
	leaveMutex();
	update();
}



void SocketService::update(unsigned char flag)
{
#ifndef WIN32
	::write(iosync[1], (char *)&flag, 1);
#else
	sync->update(flag);
#endif
}

#define MUTEX_START { MutexLock _lock_(*this);
#define MUTEX_END }

void SocketService::run(void)
{
	timeout_t timer, expires;
	SocketPort *port;
	unsigned char buf;

#ifndef WIN32
#ifdef 	CCXX_USE_POLL
	
	Poller			  mfd;
	pollfd			* p_ufd;
	int				  lastcount = 0;

	// initialize ufd in all attached ports :
	// probably don't need this but it can't hurt.
	enterMutex();
	port = first;
	while(port)
	{
		port->ufd = 0;
		port = port->next;
	}
	leaveMutex();
	
#else
	struct timeval timeout, *tvp;
	fd_set inp, out, err;
	FD_ZERO(&inp);
	FD_ZERO(&out);
	FD_ZERO(&err);
	int so;
#endif
#else // WIN32
	int numHandle = 0;
	HANDLE hv[MAXIMUM_WAIT_OBJECTS];
#endif


#ifdef WIN32
	// FIXME: needed ?
	ResetEvent(sync->GetSync());
#endif

	setCancel(cancelDeferred);
	for(;;)
	{
		timer = TIMEOUT_INF;
#ifndef WIN32
		while(1 == ::read(iosync[0], (char *)&buf, 1))
		{
#else
		for(;;)
		{
			int f = sync->getFlag();
			if (f < 0)
				break;

			buf = f;
#endif
			if(buf)
			{
				onUpdate(buf);
				continue;
			}

			setCancel(cancelImmediate);
			sleep(TIMEOUT_INF);
			exit();
		}

#ifndef WIN32
#ifdef	CCXX_USE_POLL

		bool	reallocate = false;
		
		MUTEX_START
		onEvent();
		port = first;
		while(port)
		{
			onCallback(port);
			if ( ( p_ufd = port->ufd ) ) {

				if ( ( POLLHUP | POLLNVAL ) & p_ufd->revents ) {
					// Avoid infinite loop from disconnected sockets
					port->detect_disconnect = false;
					p_ufd->events &= ~POLLHUP;

					SocketPort* p = port;
					port = port->next;
					detach(p);
					reallocate = true;
					p->disconnect();
					continue;
				}
	
				if ( ( POLLIN | POLLPRI ) & p_ufd->revents )
					port->pending();
	
				if ( POLLOUT & p_ufd->revents )
					port->output();

			} else {
				reallocate = true;
			}

retry:
			expires = port->getTimer();

			if(expires > 0)
				if(expires < timer)
					timer = expires;

			if(!expires)
			{
				port->endTimer();
				port->expired();
				goto retry;
			}

			port = port->next;
		}

		//
		// reallocate things if we saw a ServerPort without
		// ufd set !
		if ( reallocate || ( ( count + 1 ) != lastcount ) ) {
			lastcount = count + 1;
			p_ufd = mfd.getList( count + 1 );
	
			// Set up iosync polling
			p_ufd->fd = iosync[0];
			p_ufd->events = POLLIN | POLLHUP;
			p_ufd ++;
			
			port = first;
			while(port)
			{
				p_ufd->fd = port->so;
				p_ufd->events =
					( port->detect_disconnect ? POLLHUP : 0 )
					| ( port->detect_output ? POLLOUT : 0 )
					| ( port->detect_pending ? POLLIN : 0 )
				;
				port->ufd = p_ufd;
				p_ufd ++;
				port = port->next;
			}
		}
		MUTEX_END
		poll( mfd.getList(), lastcount, timer );

#else
		MUTEX_START
		onEvent();
		port = first;
		while(port)
		{
			onCallback(port);
			so = port->so;
			if(FD_ISSET(so, &err)) {
				port->detect_disconnect = false;
				
				SocketPort* p = port;
				port = port->next;
				p->disconnect();
				continue;
			}

			if(FD_ISSET(so, &inp))
				port->pending();

			if(FD_ISSET(so, &out))
				port->output();

retry:
			expires = port->getTimer();
			if(expires > 0)
				if(expires < timer)
					timer = expires;

			// if we expire, get new scheduling now

			if(!expires)
			{
				port->endTimer();
				port->expired();
				goto retry;
			}

			port = port->next;
		}

		FD_ZERO(&inp);
		FD_ZERO(&out);
		FD_ZERO(&err);
		FD_SET(iosync[0],&inp);
		port = first;
		while(port)
		{
			so = port->so;
			if(port->detect_pending)
				FD_SET(so, &inp);

			if(port->detect_output)
				FD_SET(so, &out);

			if(port->detect_disconnect)
				FD_SET(so, &err);

			port = port->next;
		}
		
		MUTEX_END
		if(timer == TIMEOUT_INF)
			tvp = NULL;
		else
		{
			tvp = &timeout;
			timeout.tv_sec = timer / 1000;
			timeout.tv_usec = (timer % 1000) * 1000;
		}
		select(hiwater, &inp, &out, &err, tvp);		
#endif
#else // WIN32
		MUTEX_START
		onEvent();

		hv[0] = sync->GetSync();
		numHandle = 1;
		port = first;
		while(port)
		{
			onCallback(port);

			long events = 0;

			if(port->detect_pending)
				events |= FD_READ;

			if(port->detect_output)
				events |= FD_WRITE;

			if(port->detect_disconnect)
				events |= FD_CLOSE;

			// !!! ignore some socket on overflow !!!
			if (events && numHandle < MAXIMUM_WAIT_OBJECTS)
			{
				WSAEventSelect(port->so,port->event,events);
				hv[numHandle++] = port->event;
			}

retry:
			expires = port->getTimer();
			if(expires > 0)
				if(expires < timer)
					timer = expires;

			// if we expire, get new scheduling now

			if(!expires)
			{
				port->endTimer();
				port->expired();
				goto retry;
			}

			port = port->next;
		}
		
		MUTEX_END

		// FIXME: handle thread cancellation correctly
		DWORD res = WaitForMultipleObjects(numHandle,hv,FALSE,timer);
		switch (res)
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			// FIXME: handle failures (detach SocketPort)
			if (res >= WAIT_OBJECT_0+1 && res <= WAIT_OBJECT_0+MAXIMUM_WAIT_OBJECTS)
			{
				int curr = res - (WAIT_OBJECT_0);
				WSANETWORKEVENTS events;

				// search port
				MUTEX_START
				port = first;
				while(port)
				{
					if (port->event == hv[curr])
						break;
					port = port->next;
				}
				MUTEX_END

				// if port not found ignore
				if (!port || port->event != hv[curr])
					break;

				WSAEnumNetworkEvents(port->so,port->event,&events);

				if(events.lNetworkEvents & FD_CLOSE) 
				{
					port->detect_disconnect = false;
					port->disconnect();
					continue;
				}

				if(events.lNetworkEvents & FD_READ)
					port->pending();

				if(events.lNetworkEvents & FD_WRITE)
					port->output();
			}
		}
#endif
	}
}						

#ifdef	CCXX_NAMESPACES
};
#endif		
	
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
