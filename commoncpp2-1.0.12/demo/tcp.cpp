// Copyright (C) 1999-2001 Open Source Telecom Corporation.
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

#include <cc++/socket.h>
#include <iostream>
#include <cstdlib>

#ifdef	CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

class myTCPSocket : public TCPSocket
{
protected:
	bool onAccept(const InetHostAddress &ia, tpport_t port);

public:
	myTCPSocket(InetAddress &ia);
};

myTCPSocket::myTCPSocket(InetAddress &ia) : TCPSocket(ia, 4096) {};

bool myTCPSocket::onAccept(const InetHostAddress &ia, tpport_t port)
{
	cout << "accepting from: " << ia << ":" << port << endl;;
	return true;
}

int main(int argc, char *argv[])
{
	int i;
	tcpstream tcp;
	InetAddress addr;
	addr = "255.255.255.255";
	cout << "testing addr: " << addr << ":" << 4096 << endl;

	addr = "127.0.0.1";
	cout << "binding for: " << addr << ":" << 4096 << endl;

	try
	{
		myTCPSocket server(addr);
		while(server.isPendingConnection(30000))
		{
			tcp.open(server);
//			tcp.unsetf(ios::binary);
			tcp << "welcome to " << addr << endl;
			if(tcp.isPending(Socket::pendingInput, 2000))
			{
				tcp >> i;
				tcp << "user entered " << i << endl;
			}
			tcp << "exiting now" << endl;
			tcp.close();
		}
	}
	catch(Socket *socket)
	{
		tpport_t port;
		InetAddress saddr = (InetAddress)socket->getPeer(&port);
		cerr << "socket error " << saddr << ":" << port << endl;
		cout << "error number " << socket->getErrorNumber() << endl;
		if(socket->getErrorNumber() == Socket::errResourceFailure)
		{
			cerr << "bind failed; no resources" << endl;
			exit(-1);
		}
		if(socket->getErrorNumber() == Socket::errBindingFailed)
		{
			cerr << "bind failed; port busy" << endl;
			exit(-1);
		}
	}
	cout << "timeout after 30 seconds inactivity, exiting" << endl;
	return 0;
}

