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
#include <fcntl.h>
#include <cerrno>
#include <cstdlib>
#ifndef	WIN32
#include <netinet/tcp.h>
#endif

#ifdef	WIN32
#include <io.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#ifndef	WIN32
# ifndef  O_NONBLOCK
#  define O_NONBLOCK	O_NDELAY
# endif
# ifdef	IPPROTO_IP
#  ifndef  SOL_IP
#   define SOL_IP	IPPROTO_IP
#  endif // !SOL_IP
# endif	 // IPPROTO_IP
#endif	 // !WIN32

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

#if defined(WIN32) && !defined(__MINGW32__)
static SOCKET dupSocket(SOCKET so,enum Socket::State state)
{
	if (state == Socket::STREAM)
		return dup(so);

	HANDLE pidHandle = GetCurrentProcess();
	HANDLE dupHandle;
	if(DuplicateHandle(pidHandle, reinterpret_cast<HANDLE>(so), pidHandle, &dupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
		return reinterpret_cast<SOCKET>(dupHandle);
	return INVALID_SOCKET;
}
# define DUP_SOCK(s,state) dupSocket(s,state)
#else
# define DUP_SOCK(s,state) dup(s)
#endif 

Socket::Socket(int domain, int type, int protocol)
{
	setSocket();
	so = socket(domain, type, protocol);
	if(so == INVALID_SOCKET)
	{
		error(errCreateFailed);
		return;
	}
	state = AVAILABLE;
}

Socket::Socket(SOCKET fd)
{
	setSocket();
	if (fd == INVALID_SOCKET)
	{
		error(errCreateFailed);
		return;
	}
	so = fd;
	state = AVAILABLE;
}

Socket::Socket(const Socket &orig)
{
	setSocket();
	so = DUP_SOCK(orig.so,orig.state);
	if(so == INVALID_SOCKET)
		error(errCopyFailed);
	state = orig.state;
}

Socket::~Socket()
{
	endSocket();
}

void Socket::setSocket(void)
{
	flags.thrown    = false;
	flags.broadcast = false;
	flags.route     = true;
	flags.keepalive = false;
	flags.loopback  = true;
	flags.multicast = false;
	flags.linger	= false;
	flags.ttl	= 1;
	errid           = errSuccess;
	errstr          = NULL;
	state           = INITIAL;
	so              = INVALID_SOCKET;
}

ssize_t Socket::readLine(char *str, size_t request, timeout_t timeout)
{
	bool crlf = false;
	bool nl = false;
	size_t nleft = request - 1; // leave also space for terminator
	int nstat,c;

	if(request < 1)
		return 0;

	str[0] = 0;

	while(nleft && !nl)
	{
		if(timeout)
		{
			if(!isPending(pendingInput, timeout))
			{
				error(errTimeout);
				return -1;
			}
		}
		nstat = ::recv(so, str, nleft, MSG_PEEK);
		if(nstat <= 0)
		{
			error(errInput);
			return -1;
		}

		// FIXME: if unique char in buffer is '\r' return "\r"
		//        if buffer end in \r try to read another char?
		//        and if timeout ??
		//        remember last \r

		for(c=0; c < nstat; ++c)
		{
			if(str[c] == '\n')
			{
				if (c > 0 && str[c-1] == '\r')
					crlf = true;
				++c;
				nl = true;
				break;
			}
		}
		nstat = ::recv(so, str, c, 0);
		// TODO: correct ???
		if(nstat < 0)
			break;

		// adjust ending \r\n in \n
		if(crlf)
		{
			--nstat;
			str[nstat - 1] = '\n';
		}

		str += nstat;
		nleft -= nstat;
	}
	*str = 0;
	return (ssize_t)(request - nleft - 1);
}

ssize_t Socket::readData(void *Target, size_t Size, char Separator, 
		     timeout_t timeout)
{
  if ((Separator == 0x0D) || (Separator == 0x0A))
    return (readLine ((char *) Target, Size, timeout));

  if (Size < 1)
    return (0);

  ssize_t nstat;

  if (Separator == 0)           // Flat-out read for a number of bytes.
    {
      if (timeout)
        if (!isPending (pendingInput, timeout))
          {
            error(errTimeout);
            return (-1);
          }
      nstat =::recv (so, (char *)Target, Size, 0);

      if (nstat < 0)
        {
          error (errInput);
          return (-1);
        }
      return (nstat);
    }
  /////////////////////////////////////////////////////////////
  // Otherwise, we have a special char separator to use
  /////////////////////////////////////////////////////////////
  bool found = false;
  size_t nleft = Size;
  int c;
  char *str = (char *) Target;

  memset (str, 0, Size);

  while (nleft && !found)
    {
      if (timeout)
        if (!isPending (pendingInput, timeout))
          {
            error(errTimeout);
            return (-1);
          }

      nstat =::recv (so, str, nleft, MSG_PEEK);
      if (nstat <= 0)
        {
          error (errInput);
          return (-1);
        }

      for (c = 0; (c < nstat) && !found; ++c)
        if (str[c] == Separator)
          found = true;

      memset (str, 0, nleft); 
      nstat =::recv (so, str, c, 0);
      if (nstat < 0)
        break;

      str += nstat;
      nleft -= nstat;
    }
  return (Size - (ssize_t) nleft);
}

ssize_t Socket::writeData(const void *Source, size_t Size, timeout_t timeout)
{
  if (Size < 0)
    return (0);

  ssize_t nstat;
  const char *Slide = (const char *) Source;

  while (true)
    {
      if (timeout)
        if (!isPending (pendingOutput, timeout))
          {
            error(errOutput);
            return (-1);
          }

      nstat =::send (so, Slide, Size, 0);

      if (nstat <= 0)
        {
          error(errOutput);
          return (-1);
        }
      Size -= nstat;
      Slide += Size;

      if (Size <= 0)
        break;
    }
  return (nstat);
}

bool Socket::isConnected(void) const
{
	return (Socket::state == CONNECTED) ? true : false;
}

bool Socket::isActive(void) const
{
	return (state != INITIAL) ? true : false;
}

bool Socket::operator!() const
{
	return (Socket::state == INITIAL) ? true : false;
}

void Socket::endSocket(void)
{
	if(Socket::state == STREAM)
	{
		state = INITIAL;
#ifdef	WIN32
		if(so != (UINT)-1)
		{
			close(so);
			so = INVALID_SOCKET;
		}
#else
		if(so > -1)
		{
			close(so);
			so = INVALID_SOCKET;
		}
#endif
		return;
	}

	state = INITIAL;
	if(so == INVALID_SOCKET)
		return;

#ifdef	SO_LINGER
	struct linger linger;

	if(flags.linger)
	{
		linger.l_onoff = 1;
		linger.l_linger = 60;
	}
	else
		linger.l_onoff = linger.l_linger = 0;
	setsockopt(so, SOL_SOCKET, SO_LINGER, (char *)&linger,
		(socklen_t)sizeof(linger));
#endif
//	shutdown(so, 2);
#ifdef WIN32
	closesocket(so);
#else
	close(so);
#endif
	so = INVALID_SOCKET;
}

#ifdef WIN32
Socket::Error Socket::connectError(void)
{
	switch(WSAGetLastError())
	{
	case WSAENETDOWN:
		return error(errResourceFailure);
	case WSAEINPROGRESS:
		return error(errConnectBusy);
	case WSAEADDRNOTAVAIL:
		return error(errConnectInvalid);
	case WSAECONNREFUSED:
		return error(errConnectRefused);
	case WSAENETUNREACH:
		return error(errConnectNoRoute);
	default:
		return error(errConnectFailed);
	}
}
#else
Socket::Error Socket::connectError(void)
{
	switch(errno)
	{
#ifdef	EHOSTUNREACH
	case EHOSTUNREACH:
		return error(errConnectNoRoute);
#endif
#ifdef	ENETUNREACH
	case ENETUNREACH:
		return error(errConnectNoRoute);
#endif
	case EINPROGRESS:
		return error(errConnectBusy);
#ifdef	EADDRNOTAVAIL
	case EADDRNOTAVAIL:
		return error(errConnectInvalid);
#endif
	case ECONNREFUSED:
		return error(errConnectRefused);
	case ETIMEDOUT:
		return error(errConnectTimeout);
	default:
		return error(errConnectFailed);
	}
}
#endif

Socket::Error Socket::error(Error err, char *errs) const
{
	errid  = err;
	errstr = errs;
	if(!err)
		return err;

	if(flags.thrown)
		return err;

	// prevents recursive throws

	flags.thrown = true;
	switch(getException())
	{
	case Thread::throwObject:
		throw((Socket *)this);
#ifdef	COMMON_STD_EXCEPTION
	case Thread::throwException:
		{
			if(!errs)
				errs = "";
			SockException ex = string(errs);
			throw ex;
		}
#endif
	case Thread::throwNothing:
		break;
	}
	return err;
}

Socket::Error Socket::setBroadcast(bool enable)
{
	int opt = (enable ? 1 : 0);
	if(setsockopt(so, SOL_SOCKET, SO_BROADCAST,
		      (char *)&opt, (socklen_t)sizeof(opt)))
		return error(errBroadcastDenied);
	flags.broadcast = enable;
	return errSuccess;
}

Socket::Error Socket::setKeepAlive(bool enable)
{
	int opt = (enable ? ~0: 0);
#if (defined(SO_KEEPALIVE) || defined(WIN32))
	if(setsockopt(so, SOL_SOCKET, SO_KEEPALIVE,
		      (char *)&opt, (socklen_t)sizeof(opt)))
		return error(errKeepaliveDenied);
#endif
	flags.keepalive = enable;
	return errSuccess;
}

Socket::Error Socket::setLinger(bool linger)
{
#ifdef	SO_LINGER
	flags.linger = linger;
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

Socket::Error Socket::setRouting(bool enable)
{
	int opt = (enable ? 1 : 0);

#ifdef	SO_DONTROUTE
	if(setsockopt(so, SOL_SOCKET, SO_DONTROUTE,
		      (char *)&opt, (socklen_t)sizeof(opt)))
		return error(errRoutingDenied);
#endif
	flags.route = enable;
	return errSuccess;
}

Socket::Error Socket::setNoDelay(bool enable)
{
	int opt = (enable ? 1 : 0);

	if(setsockopt(so, IPPROTO_TCP, TCP_NODELAY,
		      (char *)&opt, (socklen_t)sizeof(opt)))
		return error(errNoDelay);

	return errSuccess;
}

Socket::Error Socket::setTypeOfService(Tos service)
{
#ifdef	SOL_IP
	unsigned char tos;
	switch(service)
	{
#ifdef 	IPTOS_LOWDELAY
	case tosLowDelay:
		tos = IPTOS_LOWDELAY;
		break;
#endif
#ifdef 	IPTOS_THROUGHPUT
	case tosThroughput:
		tos = IPTOS_THROUGHPUT;
		break;
#endif
#ifdef	IPTOS_RELIABILITY
	case tosReliability:
		tos = IPTOS_RELIABILITY;
		break;
#endif
#ifdef	IPTOS_MINCOST
	case tosMinCost:
		tos = IPTOS_MINCOST;
		break;
#endif
	default:
		return error(errServiceUnavailable);
	}
	if(setsockopt(so, SOL_IP, IP_TOS,(char *)&tos, (socklen_t)sizeof(tos)))
		return error(errServiceDenied);
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

Socket::Error Socket::setTimeToLive(unsigned char ttl)
{
#ifdef	IP_MULTICAST_TTL
	if(!flags.multicast)
		return error(errMulticastDisabled);

	flags.ttl = ttl;
	setsockopt(so, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

Socket::Error Socket::setLoopback(bool enable)
{
#ifdef	IP_MULTICAST_LOOP
	unsigned char loop;

	if(!flags.multicast)
		return error(errMulticastDisabled);

	if(enable)
		loop = 1;
	else
		loop = 0;
	flags.loopback = enable;
	setsockopt(so, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loop, sizeof(loop));
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

bool Socket::isPending(Pending pending, timeout_t timeout)
{
	int status;
#ifdef CCXX_USE_POLL
	struct pollfd pfd;

	pfd.fd = so;
	pfd.revents = 0;
	switch(pending)
	{
	case pendingInput:
		pfd.events = POLLIN;
		break;
	case pendingOutput:
		pfd.events = POLLOUT;
		break;
	case pendingError:
		pfd.events = POLLERR | POLLHUP;
		break;
	}

	status = 0;
	while(status < 1)
	{

		if(timeout == TIMEOUT_INF)
			status = poll(&pfd, 1, -1);
		else
			status = poll(&pfd, 1, timeout);

		if(status < 1)
		{
			// don't stop polling because of a simple
			// signal :)
			if(status == -1 && errno == EINTR)
				continue;

			return false;
		}
	}

	if(pfd.revents & pfd.events)
		return true;
	return false;
#else
	struct timeval tv;
	struct timeval *tvp = &tv;
	fd_set grp;

	if(timeout == TIMEOUT_INF)
		tvp = NULL;
	else
	{
		tv.tv_usec = (timeout % 1000) * 1000;
		tv.tv_sec = timeout / 1000;
	}

	FD_ZERO(&grp);
	FD_SET(so, &grp);
	switch(pending)
	{
	case pendingInput:
		status = select(so + 1, &grp, NULL, NULL, tvp);
		break;
	case pendingOutput:
		status = select(so + 1, NULL, &grp, NULL, tvp);
		break;
	case pendingError:
		status = select(so + 1, NULL, NULL, &grp, tvp);
		break;
	}
	if(status < 1)
		return false;
	if(FD_ISSET(so, &grp))
		return true;
	return false;
#endif
}

Socket &Socket::operator=(const Socket &from)
{
	if(so == from.so)
		return *this;

	if(state != INITIAL)
		endSocket();

	so = DUP_SOCK(from.so,from.state);
	if(so == INVALID_SOCKET)
	{
		error(errCopyFailed);
		state = INITIAL;
	}
	else
		state = from.state;

	return *this;
}

void Socket::setCompletion(bool immediate)
{
	flags.completion = immediate;
#ifdef WIN32
	// note that this will not work on some versions of Windows for Workgroups. Tough. -- jfc
	switch( immediate )
	{
	case false:
		// this will not work if you are using WSAAsyncSelect or WSAEventSelect.
		// -- perhaps I should throw an exception
		ioctlsocket( so, FIONBIO, (unsigned long *) 1);
		break;
	case true:
		ioctlsocket( so, FIONBIO, (unsigned long *) 0);
		break;
	}
#else
	int fflags = fcntl(so, F_GETFL);

	switch( immediate )
	{
	case false:
		fflags |= O_NONBLOCK;
		fcntl(so, F_SETFL, fflags);
		break;
	case true:
		fflags &=~ O_NONBLOCK;
		fcntl(so, F_SETFL, fflags);
		break;
	}
#endif
}

InetHostAddress Socket::getSender(tpport_t *port) const
{
	struct sockaddr_in from;
	char buf;
	socklen_t len = sizeof(from);
	int rc = ::recvfrom(so, &buf, 1, MSG_PEEK,
			    (struct sockaddr *)&from, &len);

#ifdef WIN32
	if(rc < 1 && WSAGetLastError() != WSAEMSGSIZE)
	{
		if(port)
			*port = 0;

		memset((void*) &from, 0, sizeof(from));
		error(errInput);
	}
#else
	if(rc < 1)
	{
		if(port)
			*port = 0;

		memset((void*) &from, 0, sizeof(from));
		error(errInput);
	}
#endif
	else
	{
		if(port)
			*port = ntohs(from.sin_port);
	}

	return InetHostAddress(from.sin_addr);
}

InetHostAddress Socket::getLocal(tpport_t *port) const
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	if(getsockname(so, (struct sockaddr *)&addr, &len))
	{
		error(errResourceFailure);
	      	if(port)
			*port = 0;
		memset(&addr.sin_addr, 0, sizeof(addr.sin_addr));
	}
	else
	{
		if(port)
		    	*port = ntohs(addr.sin_port);
	}
	return InetHostAddress(addr.sin_addr);
}

InetHostAddress Socket::getPeer(tpport_t *port) const
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	if(getpeername(so, (struct sockaddr *)&addr, &len))
	{
#ifndef WIN32
		if(errno == ENOTCONN)
			error(errNotConnected);
		else
#endif
			error(errResourceFailure);
		if(port)
			*port = 0;
		memset(&addr.sin_addr, 0, sizeof(addr.sin_addr));
	}
	else
	{
		if(port)
	    		*port = ntohs(addr.sin_port);
	}
	return InetHostAddress(addr.sin_addr);
}

Socket::Error Socket::setMulticast(bool enable)
{
#ifdef	IP_MULTICAST_IF
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	if(enable == flags.multicast)
		return errSuccess;

	flags.multicast = enable;
	if(enable)
		getsockname(so, (struct sockaddr *)&addr, &len);
	else
		memset(&addr.sin_addr, 0, sizeof(addr.sin_addr));

	setsockopt(so, IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr.sin_addr, sizeof(addr.sin_addr));
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

Socket::Error Socket::join(const InetMcastAddress &ia)
{
#ifdef	IP_ADD_MEMBERSHIP
	struct ip_mreq group;
	struct sockaddr_in myaddr;
	socklen_t len = sizeof(myaddr);

	if(!flags.multicast)
		return error(errMulticastDisabled);

	getsockname(so, (struct sockaddr *)&myaddr, &len);
	memcpy(&group.imr_interface, &myaddr.sin_addr, sizeof(&myaddr.sin_addr));
	group.imr_multiaddr = getaddress(ia);
	setsockopt(so, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group));
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

Socket::Error Socket::drop(const InetMcastAddress &ia)
{
#ifdef	IP_DROP_MEMBERSHIP
	struct ip_mreq group;
	struct sockaddr_in myaddr;
	socklen_t len = sizeof(myaddr);

	if(!flags.multicast)
		return error(errMulticastDisabled);

	getsockname(so, (struct sockaddr *)&myaddr, &len);
	memcpy(&group.imr_interface, &myaddr.sin_addr, sizeof(&myaddr.sin_addr));
	group.imr_multiaddr = getaddress(ia);
	setsockopt(so, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&group, sizeof(group));
	return errSuccess;
#else
	return error(errServiceUnavailable);
#endif
}

UDPSocket::UDPSocket(void) :
Socket(AF_INET, SOCK_DGRAM, 0)
{
	memset(&peer, 0, sizeof(peer));
	peer.sin_family = AF_INET;
}

UDPSocket::UDPSocket(const InetAddress &ia, tpport_t port) :
Socket(AF_INET, SOCK_DGRAM, 0)
{
	memset(&peer, 0, sizeof(peer));
	peer.sin_family = AF_INET;
	peer.sin_addr = getaddress(ia);
	peer.sin_port = htons(port);
#ifdef	SO_REUSEADDR
	int opt = 1;
	setsockopt(so, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, (socklen_t)sizeof(opt));
#endif
	if(bind(so, (struct sockaddr *)&peer, sizeof(peer)))
	{
		endSocket();
		error(errBindingFailed);
		return;
	}
	state = BOUND;
}

UDPSocket::~UDPSocket()
{
	endSocket();
}

Socket::Error UDPSocket::join(const InetMcastAddress &ia,int InterfaceIndex)
{
#ifndef WIN32
#if defined(IP_ADD_MEMBERSHIP) && defined(SIOCGIFINDEX) && !defined(__FreeBSD__)

        struct ip_mreqn      group;
        struct sockaddr_in   myaddr;
        socklen_t            len = sizeof(myaddr);

        if(!flags.multicast)
          return error(errMulticastDisabled);

        getsockname(so, (struct sockaddr *)&myaddr, &len);
        memcpy(&group.imr_address, &myaddr.sin_addr, sizeof(&myaddr.sin_addr));
        group.imr_multiaddr = getaddress(ia);
        group.imr_ifindex   = InterfaceIndex;
        setsockopt(so, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group));
        return errSuccess;
#else
        return error(errServiceUnavailable);
#endif
#else
		return error(errServiceUnavailable);
#endif // WIN32
}


Socket::Error UDPSocket::getInterfaceIndex(const char *DeviceName,int& InterfaceIndex)
{
#ifndef WIN32
#if defined(IP_ADD_MEMBERSHIP) && defined(SIOCGIFINDEX) && !defined(__FreeBSD__)

  struct ip_mreqn  mreqn;
  struct ifreq     m_ifreq;
  int              i;
  sockaddr*        sa = (sockaddr*) &peer;

  InterfaceIndex = -1;

  mreqn.imr_multiaddr = *(struct in_addr *) &(sa->sa_data[2]);

  for (i = 0; i < IFNAMSIZ && DeviceName[i]; ++i)
    m_ifreq.ifr_name[i] = DeviceName[i];
  for (; i < IFNAMSIZ; ++i)
    m_ifreq.ifr_name[i] = 0;

  if (ioctl (so, SIOCGIFINDEX, &m_ifreq))
    return error(errServiceUnavailable);

#if defined(__FreeBSD__)
  InterfaceIndex = m_ifreq.ifr_ifru.ifru_index;
#else
  InterfaceIndex = m_ifreq.ifr_ifindex;
#endif
  return errSuccess;
#else
  return error(errServiceUnavailable);
#endif
#else
  return error(errServiceUnavailable);
#endif // WIN32
}


void UDPSocket::setPeer(const InetHostAddress &ia, tpport_t port)
{
	memset(&peer, 0, sizeof(peer));
	peer.sin_family = AF_INET;
	peer.sin_addr = getaddress(ia);
	peer.sin_port = htons(port);
}

InetHostAddress UDPSocket::getPeer(tpport_t *port) const
{
	// FIXME: insufficient buffer
	//        how to retrieve peer ??
	char buf;
	socklen_t len = sizeof(peer);
	int rtn = ::recvfrom(so, &buf, 1, MSG_PEEK, (struct sockaddr *)&peer, &len);

#ifdef WIN32
	if(rtn < 1 && WSAGetLastError() != WSAEMSGSIZE)
	{
		if(port)
			*port = 0;

		memset((void*) &peer, 0, sizeof(peer));
	}
#else
	if(rtn < 1)
	{
		if(port)
			*port = 0;

		memset((void*) &peer, 0, sizeof(peer));
	}
#endif
	else
	{
		if(port)
			*port = ntohs(peer.sin_port);
	}
	return InetHostAddress(peer.sin_addr);
}

UDPBroadcast::UDPBroadcast(const InetAddress &ia, tpport_t port) :
UDPSocket(ia, port)
{
	if(so != INVALID_SOCKET)
		setBroadcast(true);
}

void UDPBroadcast::setPeer(const BroadcastAddress &ia, tpport_t port)
{
	memset(&peer, 0, sizeof(peer));
	peer.sin_family = AF_INET;
	peer.sin_addr = getaddress(ia);
	peer.sin_port = htons(port);
}

TCPSocket::TCPSocket(const InetAddress &ia, tpport_t port, int backlog) :
Socket(AF_INET, SOCK_STREAM, 0)
{
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = getaddress(ia);
	addr.sin_port = htons(port);

#ifdef	SO_REUSEADDR
	int opt = 1;
	setsockopt(so, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, (socklen_t)sizeof(opt));
#endif
	if(bind(so, (struct sockaddr *)&addr, sizeof(addr)))
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

void TCPSocket::reject(void)
{
	int rej = accept(so, NULL, NULL);
	shutdown(rej, 2);
#ifdef WIN32
	closesocket(rej);
#else
	close(rej);
#endif
}

TCPStream::TCPStream(TCPSocket &server, int size, bool throwflag, timeout_t to) :
	streambuf(), Socket(accept(server.so, NULL, NULL))
#ifdef	HAVE_OLD_IOSTREAM
	,iostream()
#else
	,iostream((streambuf *)this)
#endif
	,bufsize(0)
	,gbuf(NULL)
	,pbuf(NULL)
{
	tpport_t port;

#ifdef	HAVE_OLD_IOSTREAM
	init((streambuf *)this);
#endif

	timeout = to;
	setError(throwflag);
	InetHostAddress host = getPeer(&port);
	if(!server.onAccept(host, port))
	{
		endSocket();
		error(errConnectRejected);
		clear(ios::failbit | rdstate());
		return;
	}

	allocate(size);
	Socket::state = CONNECTED;
}

TCPStream::TCPStream(const InetHostAddress &host, tpport_t port, int size, bool throwflag, timeout_t to) :
	streambuf(),
	Socket(AF_INET, SOCK_STREAM, 0),
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
	connect(host, port, size);
}

void TCPStream::connect(const InetHostAddress &host, tpport_t port, int size)
{
	size_t i;
	fd_set fds;
	struct timeval to;
	bool connected = false;
	int rtn;
	long sockopt;
	socklen_t len = sizeof(sockopt);

	for(i = 0 ; i < host.getAddressCount(); i++)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr = host.getAddress(i);
		addr.sin_port = htons(port);
	
		if(timeout)
			setCompletion(false);
	
		rtn = ::connect(so, (struct sockaddr *)&addr, (socklen_t)sizeof(addr));
		if(!rtn)
		{
			connected = true;
			break;
		}

#ifndef WIN32
		if(errno == EINPROGRESS)
#else
		if(WSAGetLastError() == WSAEINPROGRESS)
#endif
		{
			FD_ZERO(&fds);
			FD_SET(so, &fds);
			to.tv_sec = timeout / 1000;
			to.tv_usec = timeout % 1000 * 1000;
			
			// timeout check for connect completion

			if(::select(so + 1, NULL, &fds, NULL, &to) < 1)
				continue;

			getsockopt(so, SOL_SOCKET, SO_ERROR, (char *)&sockopt, &len);
			if(!sockopt)
			{
				connected = true;
				break;
			}
			endSocket();
			so = socket(AF_INET, SOCK_STREAM, 0);
			if(so == INVALID_SOCKET)
				break;
		}
	}

	setCompletion(true);
	if(!connected)
	{
		connectError();
		endSocket();
		return;
	}

	allocate(size);
	Socket::state = CONNECTED;
}

TCPStream::TCPStream(bool throwflag) :
	streambuf(),
	Socket(PF_INET, SOCK_STREAM, 0),
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

TCPStream::TCPStream(const TCPStream &source) :
streambuf(), Socket(DUP_SOCK(source.so,source.state)),
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

void TCPStream::endStream(void)
{
	if(bufsize) sync();
	delete [] gbuf;
	delete [] pbuf;
	gbuf = pbuf = NULL;
	bufsize = 0;
	clear();
	endSocket();
}

void TCPStream::allocate(int size)
{
	if(size < 2)
	{
		bufsize = 1;
		gbuf = pbuf = 0;
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

int TCPStream::doallocate()
{
	if(bufsize)
		return 0;

	allocate(1);
	return 1;
}

int TCPStream::uflow()
{
	int ret = underflow();

	if (ret == EOF)
		return EOF; 

	if (bufsize != 1)
		gbump(1); 

	return ret; 
}

int TCPStream::underflow()
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

bool TCPStream::isPending(Pending pending, timeout_t timeout)
{
	if(pending == pendingInput && in_avail())
		return true;
	else if(pending == pendingOutput)
		flush();

	return Socket::isPending(pending, timeout);
}

int TCPStream::sync(void)
{
	overflow(EOF);
	setg(gbuf, gbuf + bufsize, gbuf + bufsize);
	return 0;
}

int TCPStream::overflow(int c)
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
		memmove(pbuf, pptr() + rlen, req);
	setp(pbuf, pbuf + bufsize);
	pbump(req);

	if(c != EOF)
	{
		*pptr() = (unsigned char)c;
		pbump(1);
	}
	return c;
}

tcpstream::tcpstream() : 
TCPStream()
{
	setError(false);	/* no exceptions */
}

tcpstream::tcpstream(const char *addr, int buf) : 
TCPStream()
{
	setError(false);
	open(addr, buf);
}

tcpstream::tcpstream(TCPSocket &server, int buf) :
TCPStream()
{
	setError(false);
	open(server, buf);
}

bool tcpstream::operator!() const
{
	return (Socket::state != CONNECTED) ? true : false;
}

void tcpstream::open(TCPSocket &tcp, int buf)
{
	tpport_t port;

	endStream();
	so = accept(tcp.so, NULL, NULL);
	if(so == INVALID_SOCKET)
		return;

	InetHostAddress host = getPeer(&port);
	if(!tcp.onAccept(host, port))
	{
		endSocket();
		clear(ios::failbit | rdstate());
		return;
	}

	allocate(buf);
	Socket::state = CONNECTED;
}
	
void tcpstream::open(const char *path, int buf)
{
	char abuf[256];
	const char *cp;
	char *ep;

	close();
	if(Socket::state != AVAILABLE)
		return;
	
	cp = strrchr(path, ':');
	if(!cp)
		return;

	++cp;
	strncpy(abuf, path, sizeof(abuf) - 1);
	abuf[sizeof(abuf) - 1] = 0;
	ep = strrchr(abuf, ':');
	if(ep)
		*ep = 0;

	connect(InetHostAddress(abuf), (tpport_t)atoi(cp), buf);
}
	
void tcpstream::close(void)
{
	if(Socket::state == AVAILABLE)
		return;

	endStream();
	so = socket(AF_INET, SOCK_STREAM, 0);
	if(so != INVALID_SOCKET)
		Socket::state = AVAILABLE;
}

TCPSession::TCPSession(const InetHostAddress &ia, 
		       tpport_t port, int size, int pri, int stack) :
TCPStream(), Thread(pri, stack)
{
	setCompletion(false);
	setError(false);
	allocate(size);
	
	size_t i;
	for(i = 0 ; i < ia.getAddressCount(); i++)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr = ia.getAddress(i);
		addr.sin_port = htons(port);
		
		if(::connect(so, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)) == 0)
			break;
		
#ifdef WIN32
//		if(WSAGetLastError() == WSAEWOULDBLOCK)
		if(WSAGetLastError() == WSAEISCONN)
#else
		if(errno == EINPROGRESS)
#endif
		{
			Socket::state = CONNECTING;
			return;
		}
	}

	if(i == ia.getAddressCount())
	{
		endSocket();
		Socket::state = INITIAL;
		return;
	}

	setCompletion(true);
	Socket::state = CONNECTED;
}


TCPSession::TCPSession(TCPSocket &s,
		       int size, int pri, int stack) :
TCPStream(s, size), Thread(pri, stack)
{
	setCompletion(true);
	setError(false);
}

int TCPSession::waitConnection(timeout_t timeout)
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

void TCPSession::initial(void)
{
	if(waitConnection(60000))
		exit();
}

CCXX_EXPORT(ostream&) operator<<(ostream &os, const InetAddress &ia)
{
	os << inet_ntoa(getaddress(ia));
	return os;
}

#ifdef WIN32
init_WSA::init_WSA()
{
	//-initialize OS socket resources!
	WSADATA	wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		abort();
	}
};

init_WSA::~init_WSA() 
{ 
	WSACleanup(); 
} 

init_WSA init_wsa;
#endif

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
