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
#include <cc++/unix.h>

#include <cc++/unix.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdlib>
#ifndef	WIN32
#include <netinet/tcp.h>
#include <sys/un.h>
#endif

#ifdef	WIN32
#include <io.h>
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

#ifndef WIN32

UnixSocket::UnixSocket(const char* pathname, int backlog) :
 Socket(AF_UNIX, SOCK_STREAM, 0)
 {
 	struct sockaddr_un addr;
	socklen_t len;
 	memset(&addr, 0, sizeof(addr));
 	addr.sun_family = AF_UNIX;
 	strncpy( addr.sun_path, pathname, sizeof(addr.sun_path) );

#ifdef	SO_REUSEADDR
 	int opt = 1;
 	setsockopt(so, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, (socklen_t)sizeof(opt));
#endif

#ifdef	__SUN_LEN
	len = sizeof(addr.sun_len) + strlen(addr.sun_path) + sizeof(addr.sun_family) + 1;
	addr.sun_len = len;
#else
	len = strlen(addr.sun_path) + sizeof(addr.sun_family) + 1;
#endif
 	if(bind(so, (struct sockaddr *)&addr, len))
 	{
 		endSocket();
 		error(errBindingFailed);
 		return;
 	}

 	if(listen(so, backlog))
 	{
 		endSocket();
 		error(errBindingFailed);
 		return;
 	}
 	state = BOUND;
 }

 UnixStream::UnixStream(UnixSocket &server, int size, bool throwflag, timeout_t to) :
 	Socket(accept(server.so, NULL, NULL))
 	,streambuf()
 #ifdef	HAVE_OLD_IOSTREAM
 	,iostream()
 #else
 	,iostream((streambuf *)this)
 #endif
 	,bufsize(0)
 	,gbuf(NULL)
 	,pbuf(NULL)
 {
 #ifdef	HAVE_OLD_IOSTREAM
 	init((streambuf *)this);
 #endif

 	timeout = to;
 	setError(throwflag);

 	allocate(size);
 	Socket::state = CONNECTED;
 }

 UnixStream::UnixStream(const char* pathname, int size, bool throwflag, timeout_t to) :
 	Socket(AF_UNIX, SOCK_STREAM, 0),
 	streambuf(),
 #ifdef	HAVE_OLD_IOSTREAM
 	iostream(),
 #else
         iostream((streambuf *)this),
 #endif
 	bufsize(0),gbuf(NULL),pbuf(NULL)
 {
 #ifdef	HAVE_OLD_IOSTREAM
 	init((streambuf *)this);
 #endif
 	timeout = to;
 	setError(throwflag);
 	connect(pathname, size);
 }

 void UnixStream::connect(const char* pathname, int size)
 {
 	struct sockaddr_un addr;
	socklen_t len;
 	memset(&addr, 0, sizeof(addr));
 	addr.sun_family = AF_UNIX;
 	strncpy( addr.sun_path, pathname, sizeof(addr.sun_path) );
#ifdef	__SUN_LEN
	len = sizeof(addr.sun_len) + strlen(addr.sun_path) + sizeof(addr.sun_family) + 1;
	addr.sun_len = len;
#else
	len = strlen(addr.sun_path) + sizeof(addr.sun_family);
#endif
 	if(::connect(so, (struct sockaddr *)&addr, len) != 0)
 	{
 		connectError();
 		endSocket();
 		return;
 	}

 	allocate(size);
 	Socket::state = CONNECTED;
 }

 UnixStream::UnixStream(bool throwflag) :
 	Socket(PF_UNIX, SOCK_STREAM, 0),
 	streambuf(),
 #ifdef	HAVE_OLD_IOSTREAM
 	iostream(),
 #else
         iostream((streambuf *)this),
 #endif
 	 timeout(0),
         bufsize(0),gbuf(NULL),pbuf(NULL)
 {
 #ifdef	HAVE_OLD_IOSTREAM
 	init((streambuf *)this);
 #endif
 	setError(throwflag);
 }

 UnixStream::UnixStream(const UnixStream &source) :
 Socket(dup(source.so)), streambuf(),
 #ifdef	HAVE_OLD_IOSTREAM
 iostream()
 #else
 iostream((streambuf *)this)
 #endif
 {
 #ifdef	HAVE_OLD_IOSTREAM
 	init((streambuf *)this);
 #endif
 	bufsize = source.bufsize;
 	allocate(bufsize);
 }

 void UnixStream::endStream(void)
 {
 	if(bufsize) sync();
 	delete [] gbuf;
 	delete [] pbuf;
 	gbuf = pbuf = NULL;
 	bufsize = 0;
 	endSocket();
 }

 void UnixStream::allocate(int size)
 {
 	if(size < 2)
 	{
 		bufsize = 1;
 		return;
 	}

 	gbuf = new char[size];
 	pbuf = new char[size];
 	if(!pbuf || !gbuf)
 	{
 		error(errResourceFailure);
 		return;
 	}
 	bufsize = size;
 	clear();

 #if (defined(__GNUC__) && (__GNUC__ < 3)) && !defined(WIN32) && !defined(STLPORT)
 	setb(gbuf, gbuf + size, 0);
 #endif
 	setg(gbuf, gbuf + size, gbuf + size);
 	setp(pbuf, pbuf + size);
 }

 int UnixStream::doallocate()
 {
 	if(bufsize)
 		return 0;

 	allocate(1);
 	return 1;
 }

 int UnixStream::uflow(void)
 {
 	int ret = underflow();

 	if (ret == EOF)
 		return EOF;

 	if (bufsize != 1)
 		gbump(1);

 	return ret;
 }

 int UnixStream::underflow(void)
 {
 	int rlen = 1;
 	unsigned char ch;

 	if(bufsize == 1)
 	{
 		if(Socket::state == STREAM)
 			rlen = ::read(so, (char *)&ch, 1);
 		else if(timeout && !Socket::isPending(pendingInput, timeout))
         {
             clear(ios::failbit | rdstate());
             error(errTimeout);
             return EOF;
 		}
 		else
 			rlen = ::recv(so, (char *)&ch, 1, 0);
 		if(rlen < 1)
 		{
 			if(rlen < 0)
 			{
 				clear(ios::failbit | rdstate());
 				error(errInput);
 			}
 			return EOF;
 		}
 		return ch;
 	}

 	if(!gptr())
 		return EOF;

 	if(gptr() < egptr())
 		return (unsigned char)*gptr();

 	rlen = (gbuf + bufsize) - eback();
 	if(Socket::state == STREAM)
 		rlen = ::read(so, (char *)eback(), rlen);
 	else if(timeout && !Socket::isPending(pendingInput, timeout))
 	{
 		clear(ios::failbit | rdstate());
 		error(errTimeout);
 		return EOF;
 	}
 	else
 		rlen = ::recv(so, (char *)eback(), rlen, 0);
 	if(rlen < 1)
 	{
 		if(rlen < 0)
 		{
 			clear(ios::failbit | rdstate());
 			error(errInput);
 		}
 		return EOF;
 	}

 	setg(eback(), eback(), eback() + rlen);
 	return (unsigned char) *gptr();
 }

 bool UnixStream::isPending(Pending pending, timeout_t timeout)
 {
 	if(pending == pendingInput && in_avail())
 		return true;
 	else if(pending == pendingOutput)
 		flush();

 	return Socket::isPending(pending, timeout);
 }

 int UnixStream::sync(void)
 {
 	overflow(EOF);
 	setg(gbuf, gbuf + bufsize, gbuf + bufsize);
 	return 0;
 }

 int UnixStream::overflow(int c)
 {
 	unsigned char ch;
 	int rlen, req;

 	if(bufsize == 1)
 	{
 		if(c == EOF)
 			return 0;

 		ch = (unsigned char)(c);
 		if(Socket::state == STREAM)
 			rlen = ::write(so, (const char *)&ch, 1);
 		else
 			rlen = ::send(so, (const char *)&ch, 1, 0);
 		if(rlen < 1)
 		{
 			if(rlen < 0)
 			{
 				clear(ios::failbit | rdstate());
 				error(errOutput);
 			}
 			return EOF;
 		}
 		else
 			return c;
 	}

 	if(!pbase())
 		return EOF;

 	req = pptr() - pbase();
 	if(req)
 	{
 		if(Socket::state == STREAM)
 			rlen = ::write(so, (const char *)pbase(), req);
 		else
 			rlen = ::send(so, (const char *)pbase(), req, 0);
 		if(rlen < 1)
 		{
 			if(rlen < 0)
 			{
 				clear(ios::failbit | rdstate());
 				error(errOutput);
 			}
 			return EOF;
 		}
 		req -= rlen;
 	}

 	// if write "partial", rebuffer remainder

 	if(req)
 		memcpy(pptr(), pptr() + rlen, req);
 	setp(pbuf + req, pbuf + bufsize);

 	if(c != EOF)
 	{
 		*pptr() = (unsigned char)c;
 		pbump(1);
 	}
 	return c;
 }

 unixstream::unixstream() :
 UnixStream()
 {
 	setError(false);	/* no exceptions */
 }

 unixstream::unixstream(const char *pathname, int buf) :
 UnixStream()
 {
 	setError(false);
 	open(pathname, buf);
 }

 unixstream::unixstream(UnixSocket &server, int buf) :
 UnixStream()
 {
 	setError(false);
 	open(server, buf);
 }

 bool unixstream::operator!() const
 {
 	return (Socket::state != CONNECTED) ? true : false;
 }

 void unixstream::open(UnixSocket &unixsock, int buf)
 {
 	endStream();
 	so = accept(unixsock.so, NULL, NULL);
 	if(so == INVALID_SOCKET)
 		return;

 	allocate(buf);
 	Socket::state = CONNECTED;
 }

 void unixstream::close(void)
 {
 	if(Socket::state == AVAILABLE)
 		return;

 	endStream();
 	so = socket(AF_UNIX, SOCK_STREAM, 0);
 	if(so != INVALID_SOCKET)
 		Socket::state = AVAILABLE;
 }

 UnixSession::UnixSession(const char* pathname, int size, int pri, int stack) :
 UnixStream(), Thread(pri, stack)
 {
 	socklen_t len;
 	setCompletion(false);
 	setError(false);
 	allocate(size);

 	struct sockaddr_un addr;
 	memset(&addr, 0, sizeof(addr));
 	addr.sun_family = AF_UNIX;
 	strncpy( addr.sun_path, pathname, sizeof(addr.sun_path) );

#ifdef	__SUN_LEN
	len = sizeof(addr.sun_len) + strlen(addr.sun_path) + sizeof(addr.sun_family) + 1;
	addr.sun_len = len;
#else
	len = strlen(addr.sun_path) + sizeof(addr.sun_family);
#endif
 	if(::connect(so, (struct sockaddr *)&addr, len ) != 0)
 	{
 #ifdef WIN32
 		if( WSAGetLastError() == WAEISCONN )
 #else
 		if( EINPROGRESS == errno )
 #endif
 		{
 			Socket::state = CONNECTING;
 		}
 		else
 		{
 			endSocket();
 			Socket::state = INITIAL;
 		}
 		return;
 	}

 	setCompletion(true);
 	Socket::state = CONNECTED;
 }


 UnixSession::UnixSession(UnixSocket &s,int size, int pri, int stack) :
 UnixStream(s, size), Thread(pri, stack)
 {
 	setCompletion(true);
 	setError(false);
 }

 int UnixSession::waitConnection(timeout_t timeout)
 {
 	long sockopt = 0;
 	socklen_t len = sizeof(sockopt);

 	switch(Socket::state)
 	{
 	case INITIAL:
 		return -1;
 	case CONNECTED:
 		break;
 	case CONNECTING:
 		if(!Socket::isPending(pendingOutput, timeout))
 		{
 			endSocket();
 			Socket::state = INITIAL;
 			return -1;
 		}

 		getsockopt(so, SOL_SOCKET, SO_ERROR, (char *)&sockopt, &len);
 		if(sockopt)
 		{
 			endSocket();
 			Socket::state = INITIAL;
 			return -1;
 		}
	case AVAILABLE:
	case BOUND:
	case STREAM:
		break;
 	}
 	Socket::state = CONNECTED;
 	return 0;
 }

 void UnixSession::initial(void)
 {
 	if(waitConnection(60000))
 		exit();
 }

#endif // ndef WIN32 
#ifdef	CCXX_NAMESPACES
};
#endif
