// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file SimNet/Sockets.h
 *
 */

#ifndef __SIMNET_SOCKETS_H__
#define __SIMNET_SOCKETS_H__

// use this to fix compile problems with mulitple includes of windows and winsock headers.
#define _WINSOCKAPI_
#include <cc++/network.h>

namespace simnet {


/** Minimal wrapper for GNU CommonC++ UDPReceive.
 *
 *  Exposes several protected methods.
 */
class DatagramReceiveSocket: public ost::UDPReceive {
public:
	DatagramReceiveSocket(const ost::InetHostAddress &host, ost::tpport_t port):
	    ost::UDPReceive(host, port) { }

	Error connect(const ost::InetHostAddress &host, ost::tpport_t port) {
		return ost::UDPReceive::connect(host, port);
	}

	Error disconnect(void) { return ost::UDPReceive::disconnect(); }

	inline bool isPendingReceive(timeout_t timeout) {
		return ost::UDPReceive::isPendingReceive(timeout);
	}

	ost::InetHostAddress getSender(ost::tpport_t *port = NULL) const {
		return ost::UDPReceive::getSender(port);
	}

	inline int peek(void *buf, size_t len) {
		return ost::UDPReceive::peek(buf, len);
	}
};


/** Minimal wrapper for GNU CommonC++ UDPTransmit.
 *
 *  Exposes several protected methods.
 */
class DatagramTransmitSocket: public ost::UDPTransmit {
public:
	DatagramTransmitSocket(): ost::UDPTransmit() { }

	inline Error connect(const ost::InetHostAddress &host, ost::tpport_t port) {
		return ost::UDPTransmit::connect(host, port);
	}

	inline Error disconnect(void) { return ost::UDPTransmit::disconnect(); }
};


} // namespace simnet

#endif // __SIMNET_SOCKETS_H__

