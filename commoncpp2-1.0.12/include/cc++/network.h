// Copyright (C) 2002 Christian Prochnow <cproch@seculogix.de>
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
// The exception is that, if you link the Common C++ library with other files
// to produce an executable, this does not by itself cause the
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

#ifndef CCXX_NETWORK_H_
#define CCXX_NETWORK_H_

#ifndef CCXX_CONFIG_H
#include <cc++/config.h>
#endif

#ifndef	CCXX_SOCKET_H_
#include <cc++/socket.h>
#endif

#include <string>
#include <vector>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

//! Network device information class
/*!
	This class is used to hold various informations about a TCP/IP
	network device. Which can be obtained by a call to enumNetworkDevices()
	\author Christian Prochnow <cproch@seculogix.de>
*/
class CCXX_CLASS_EXPORT NetworkDeviceInfo 
{
private:
	std::string _name;
	InetHostAddress _addr;
	BroadcastAddress _broadcast;
	InetMaskAddress _netmask;
	int _mtu;

protected:
	NetworkDeviceInfo(const std::string& name, const InetHostAddress& addr,
			  const BroadcastAddress& broadcast, 
			  const InetMaskAddress& netmask, int mtu);

public:
	NetworkDeviceInfo(const NetworkDeviceInfo& ndi);
	~NetworkDeviceInfo();

	//! Returns the Name of the network device
	inline const std::string& name() const
	{ return _name; }

	//! Returns the Address of the network device
	inline const InetHostAddress& address() const
	{ return _addr; }

	//! Returns the Broadcast address of the network device
	inline const BroadcastAddress& broadcast() const
	{ return _broadcast; }

	//! Returns the Netmask of the network device
	inline const InetMaskAddress& netmask() const
	{ return _netmask; }

	//! Returns the MTU
	inline const int mtu() const
	{ return _mtu; }

	//! Enumerate all available network devices
        friend CCXX_EXPORT(bool) enumNetworkDevices(std::vector<NetworkDeviceInfo>& devs);
};

#ifdef	CCXX_NAMESPACES
}
#endif

#endif
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
