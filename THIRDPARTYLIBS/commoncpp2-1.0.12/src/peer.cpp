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

#ifdef WIN32
#include <fcntl.h>
#endif
#include <cerrno>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

UDPTransmit::UDPTransmit(const InetAddress &ia, tpport_t port) : 
UDPSocket(ia, port)
{
	disconnect();	// assure not started live
	shutdown(so, 0);
}

UDPTransmit::UDPTransmit() : UDPSocket()
{
	disconnect();
	shutdown(so, 0);
}

Socket::Error UDPTransmit::cConnect(const InetAddress &ia, tpport_t port)
{
	int len = sizeof(peer);

	peer.sin_family = AF_INET;
	peer.sin_addr = getaddress(ia);
	peer.sin_port = htons(port);
	if(::connect(so, (sockaddr *)&peer, len))
		return connectError();
	return errSuccess;
}

Socket::Error UDPTransmit::connect(const InetHostAddress &ia, tpport_t port)
{
	if(isBroadcast())
		setBroadcast(false);

	return cConnect((InetAddress)ia,port);
}

Socket::Error UDPTransmit::connect(const BroadcastAddress &subnet, tpport_t  port)
{
	if(!isBroadcast())
		setBroadcast(true);

	return cConnect((InetAddress)subnet,port);
}

Socket::Error UDPTransmit::connect(const InetMcastAddress &group, tpport_t  port)
{
	Error error;
	if(!( error = setMulticast(true) ))
		return error;

	return cConnect((InetAddress)group,port);
}

#ifdef	AF_UNSPEC
Socket::Error UDPTransmit::disconnect(void)
{
	struct sockaddr_in addr;
	int len = sizeof(addr);

	memset(&addr, 0, len);
#ifndef WIN32
	addr.sin_family = AF_UNSPEC;
#else
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_NONE;
#endif
	if(::connect(so, (sockaddr *)&addr, len))
		return connectError();
	return errSuccess;
}
#else
Socket::Error UDPTransmit::disconnect(void)
{
	return connect(getLocal());
}
#endif

UDPReceive::UDPReceive(const InetAddress &ia, tpport_t port) : 
UDPSocket(ia, port)
{
	shutdown(so, 1);
}

Socket::Error UDPReceive::connect(const InetHostAddress &ia, tpport_t port)
{
	int len = sizeof(peer);

	peer.sin_family = AF_INET;
	peer.sin_addr = getaddress(ia);
	peer.sin_port = htons(port);
	if(::connect(so, (sockaddr *)&peer, len))
		return connectError();
	return errSuccess;
}

// FIXME same implementation as UDPTrasmit::Disconnect
#ifdef	AF_UNSPEC
Socket::Error UDPReceive::disconnect(void)
{
	struct sockaddr_in addr;
	int len = sizeof(addr);

	memset(&addr, 0, len);
#ifndef WIN32
	addr.sin_family = AF_UNSPEC;
#else
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_NONE;
#endif
	if(::connect(so, (sockaddr *)&addr, len))
		return connectError();
	return errSuccess;
}
#else
Socket::Error UDPReceive::disconnect(void)
{
	return connect(getLocal());
}
#endif

UDPDuplex::UDPDuplex(const InetAddress &bind, tpport_t port) :
UDPTransmit(bind, port + 1), UDPReceive(bind, port)
{};

Socket::Error UDPDuplex::connect(const InetHostAddress &host, tpport_t port)
{
	Error rtn = UDPTransmit::connect(host, port);
	if(rtn)
	{
		UDPTransmit::disconnect();
		UDPReceive::disconnect();
		return rtn;
	}
	return UDPReceive::connect(host, port + 1);
}

Socket::Error UDPDuplex::disconnect(void)
{
	Error rtn = UDPTransmit::disconnect();
	Error rtn2 = UDPReceive::disconnect();
	if (rtn) return rtn;
	return rtn2;
}

#ifdef	CCXX_NAMESPACES
};
#endif
