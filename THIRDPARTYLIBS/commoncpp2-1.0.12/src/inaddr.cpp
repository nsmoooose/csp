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
#include <cc++/socket.h>
#include "private.h"
#include <cstdlib>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

#ifndef WIN32
Mutex InetAddress::mutex;
#endif

const InetMcastAddrValidator InetMcastAddress::validator;

void
InetMcastAddrValidator::operator()(const in_addr address) const
{
	// "0.0.0.0" is always accepted, as it is an "empty" address.
	if ( (address.s_addr != INADDR_ANY) &&
	     (address.s_addr & MCAST_VALID_MASK) != MCAST_VALID_VALUE ) {
		throw "Multicast address not in the valid range: from 224.0.0.1 through 239.255.255.255";
	}
}

InetAddress::InetAddress(const InetAddrValidator *_validator) 
	: validator(_validator), ipaddr(NULL), addr_count(0), hostname(NULL)
{
	*this = (long unsigned int)INADDR_ANY;
}

InetAddress::InetAddress(const char *address, const InetAddrValidator *_validator) : 
	validator(_validator), ipaddr(NULL), addr_count(0), hostname(NULL)
{
	if ( this->validator )
		this->validator = validator;
	if(address == 0 || !strcmp(address, "*"))
		setAddress(NULL);
	else
		setAddress(address);
}

InetAddress::InetAddress(struct in_addr addr, const InetAddrValidator *_validator) : 
	validator(_validator), ipaddr(NULL), hostname(NULL)
{
	if ( this->validator ){
		this->validator = validator;
		(*validator)(addr);
	}
	addr_count = 1;
	ipaddr = new struct in_addr[1];
	ipaddr[0] = addr;
}

InetAddress::InetAddress(const InetAddress &rhs) : validator(rhs.validator),
						   addr_count(rhs.addr_count),
						   hostname(NULL)
{
	ipaddr = new struct in_addr[addr_count];
	memcpy(ipaddr, rhs.ipaddr, sizeof(struct in_addr) * addr_count);
}

InetAddress::~InetAddress()
{
	if(ipaddr)
		delete[] ipaddr;
	if(hostname)
		delete[] hostname;
}

struct in_addr InetAddress::getAddress(void) const
{
	return ipaddr[0];
}

struct in_addr InetAddress::getAddress(size_t i) const
{
	return (i < addr_count ? ipaddr[i] : ipaddr[0]);
}

bool InetAddress::isInetAddress(void) const
{
	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));
	if(memcmp(&addr, &ipaddr[0], sizeof(addr)))
		return true;
	return false;
}

InetAddress &InetAddress::operator=(const char *str)
{
	if(str == 0 || !strcmp(str, "*"))
		str = "0.0.0.0";

	setAddress(str);

	return *this;
}

InetAddress &InetAddress::operator=(struct in_addr addr)
{
	if(ipaddr)
		delete[] ipaddr;
	if ( validator )
		(*validator)(addr);
	addr_count = 1;
	ipaddr = new struct in_addr[1];
	ipaddr[0] = addr;
	if(hostname)
		delete[] hostname;
	hostname = NULL;
	return *this;
}

InetAddress &InetAddress::operator=(unsigned long addr)
{
	if ( validator )
		(*validator)(*reinterpret_cast<in_addr*>(&addr));
	if(ipaddr)
		delete[] ipaddr;
	addr_count = 1;
	ipaddr = new struct in_addr[1];
        memcpy(ipaddr, &addr, sizeof(struct in_addr));
	if(hostname)
		delete[] hostname;
	hostname = NULL;
	return *this;        
}

InetAddress &InetAddress::operator=(const InetAddress &rhs)
{
	if(this == &rhs) return *this;

	addr_count = rhs.addr_count;
	if(ipaddr)
		delete[] ipaddr;
	ipaddr = new struct in_addr[addr_count];
	memcpy(ipaddr, rhs.ipaddr, sizeof(struct in_addr) * addr_count);
	validator = rhs.validator;
	if(hostname)
		delete[] hostname;
	hostname = NULL;

	return *this;
}

bool InetAddress::operator==(const InetAddress &a) const
{
	const InetAddress *smaller, *larger;
	size_t s, l;
	
	if(addr_count > a.addr_count)
	{
		smaller = &a;
		larger  = this;
	}
	else
	{
		smaller = this;
		larger  = &a;
	}

	// Loop through all addr's in the smaller and make sure
	// that they are all in the larger
	for(s = 0; s < smaller->addr_count; s++)
	{
		// bool found = false;
		for(l = 0; l < larger->addr_count && 
			memcmp((char *)&ipaddr[s], (char *)&a.ipaddr[l], sizeof(struct in_addr)); l++);
		if(l == larger->addr_count) return false;
	}
	return true;
}

bool InetAddress::operator!=(const InetAddress &a) const
{
	// Impliment in terms of operator==
	return (*this == a ? false : true);
}

InetHostAddress &InetHostAddress::operator&=(const InetMaskAddress &ma)
{
	for(size_t i = 0; i < addr_count; i++)
	{		
		struct in_addr mask = ma.getAddress();
		unsigned char *a = (unsigned char *)&ipaddr[i];
		unsigned char *m = (unsigned char *)&mask;
		
		for(size_t j = 0; j < sizeof(struct in_addr); ++j)
			*(a++) &= *(m++);
	}
	if(hostname)
		delete[] hostname;
	hostname = NULL;

	return *this;
}
	
InetHostAddress::InetHostAddress(struct in_addr addr) :
InetAddress(addr) {}

InetHostAddress::InetHostAddress(const char *host) : 
InetAddress(host)
{
	char namebuf[256];

	if(!host)
	{
		gethostname(namebuf, 256);
		setAddress(namebuf);
	}
}

bool InetAddress::setIPAddress(const char *host)
{
	if(!host)
		return false;

#if defined(WIN32)
	struct sockaddr_in addr;
	
	addr.sin_addr.s_addr = inet_addr(host);
	if ( validator )
		(*validator)(addr.sin_addr);
	if(addr.sin_addr.s_addr == INADDR_NONE)
		return false;
	*this = addr.sin_addr.s_addr;
#else
	struct in_addr l_addr;
	
	int ok = inet_aton(host, &l_addr);
	if ( validator )
		(*validator)(l_addr);
	if ( !ok )
		return false;
	*this = l_addr;
#endif
	return true;
}

void InetAddress::setAddress(const char *host)
{	
	if(hostname)
		delete[] hostname;
	hostname = NULL;

	if(!host)  // The way this is currently used, this can never happen
	{
		*this = (long unsigned int)htonl(INADDR_ANY);
		return;
	}

	if(!setIPAddress(host))
	{
		struct hostent *hp;
		struct in_addr **bptr;
#if defined(__GLIBC__)
		char   hbuf[8192];
		struct hostent hb;
		int    rtn;

		if(gethostbyname_r(host, &hb, hbuf, sizeof(hbuf), &hp, &rtn))
			hp = NULL;
#elif defined(sun)
		char   hbuf[8192];
		struct hostent hb;
		int    rtn;

		hp = gethostbyname_r(host, &hb, hbuf, sizeof(hbuf), &rtn);
#elif (defined(__osf__) || defined(WIN32))
		hp = gethostbyname(host);
#else
		mutex.enterMutex();
		hp = gethostbyname(host);
		mutex.leaveMutex();
#endif
		if(!hp)
		{
			if(ipaddr)
				delete[] ipaddr;
			ipaddr = new struct in_addr[1];
			memset((void *)&ipaddr[0], 0, sizeof(ipaddr));
			return;
		}
		
		// Count the number of IP addresses returned
		addr_count = 0;
		for(bptr = (struct in_addr **)hp->h_addr_list; *bptr != NULL; bptr++)
		{
			addr_count++;
		}

		// Allocate enough memory
		if(ipaddr)
			delete[] ipaddr;	// Cause this was allocated in base
		ipaddr = new struct in_addr[addr_count];

		// Now go through the list again assigning to 
		// the member ipaddr;
		bptr = (struct in_addr **)hp->h_addr_list;
		for(unsigned int i = 0; i < addr_count; i++)
		{
			if ( validator )
				(*validator)(*bptr[i]);
			ipaddr[i] = *bptr[i];
		}
	}
}

BroadcastAddress::BroadcastAddress(const char *net) : 
InetAddress(net)
{
}

InetMaskAddress::InetMaskAddress(const char *mask)
{
	unsigned long x = 0xffffffff;
	int l = 32 - atoi(mask);

	if(setIPAddress(mask))
		return;

	if(l < 1 || l > 32)
	{
		if(getException() == Thread::throwObject)
			throw((InetAddress *)this);
		return;
	}

	*this = htonl(x << l);
}

const char *InetAddress::getHostname(void) const
{
	struct hostent *hp = NULL;
	struct in_addr addr0;

	memset(&addr0, 0, sizeof(addr0));
	if(!memcmp(&addr0, &ipaddr[0], sizeof(addr0)))
		return NULL;

#if defined(__GLIBC__)
	char   hbuf[8192];
	struct hostent hb;
	int    rtn;
	if(gethostbyaddr_r((char *)&ipaddr[0], sizeof(addr0), AF_INET, &hb, hbuf, sizeof(hbuf), &hp, &rtn))
		hp = NULL;
#elif defined(sun)
	char   hbuf[8192];
	struct hostent hb;
	int    rtn;
	hp = gethostbyaddr_r((char *)&ipaddr[0], (int)sizeof(addr0), (int)AF_INET, &hb, hbuf, (int)sizeof(hbuf), &rtn);
#elif defined(__osf__) || defined(WIN32)
	hp = gethostbyaddr((char *)&ipaddr[0], sizeof(addr0), AF_INET);
#else
	mutex.enterMutex();
	hp = gethostbyaddr((char *)&ipaddr[0], sizeof(addr0), AF_INET);
	mutex.leaveMutex();
#endif
	if(hp) {
		if(hostname)
			delete[] hostname;
		hostname = new char[strlen(hp->h_name) + 1];
		strcpy(hostname,hp->h_name);
		return hostname;
	} else {
		return inet_ntoa(ipaddr[0]);
	}
}
	
InetHostAddress operator&(const InetHostAddress &addr, 
			  const InetMaskAddress &mask)
{
	InetHostAddress temp = addr;
	temp &= mask;
	return temp;
}

InetMcastAddress::InetMcastAddress() : 
	InetAddress(&validator)
{ }

InetMcastAddress::InetMcastAddress(const struct in_addr address) : 
	InetAddress(address,&validator)
{ }

InetMcastAddress::InetMcastAddress(const char *address) :
	InetAddress(address,&validator)
{ }

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
