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
 * @file NetLog.h
 *
 **/

// TODO retire netlog?

#ifndef __CSPLIB_NET_NETLOG_H__
#define __CSPLIB_NET_NETLOG_H__

#include <csp/lib/util/Log.h>

CSP_NAMESPACE

/**
 * Define the possible classes/categories of logging messages
 */
enum {
	LOGCAT_NONE        = 0x00000000,
	LOGCAT_TIMING      = 0x00000001,
	LOGCAT_HANDSHAKE   = 0x00000002,
	LOGCAT_BALANCING   = 0x00000004,
	LOGCAT_PACKET      = 0x00000008,
	LOGCAT_PEER        = 0x00000010,
	LOGCAT_MESSAGE     = 0x00000020,
	LOGCAT_ALL         = 0xFFFFFFFF
};


/**
 * @relates LogStream
 * Return the one and only net LogStream instance.
 * We use a function together with lazy construction so we are assured
 * that cerr has been initialised.
 * @return net LogStream
 */
inline LogStream& netlog() {
	static LogStream *log_stream = 0;
	if (log_stream == 0) log_stream = new LogStream();
	return *log_stream;
}


#if defined(NDEBUG) || defined(SIMNET_NDEBUG)
# define SIMNET_NOTEWORTHY(P, C) false
#else
# define SIMNET_NOTEWORTHY(P, C) CSP(netlog()).isNoteworthy(CSPLOG_PRIORITY(P), CSP(LOGCAT_##C))
#endif

# define SIMNET_LOG(P, C) \
	if (!SIMNET_NOTEWORTHY(P, C)); \
	else CSP(LogStream::LogEntry)(static_cast<CSP(LogStream)&>(netlog()), CSPLOG_PRIORITY(P), __FILE__, __LINE__)


CSP_NAMESPACE_END


#endif // __CSPLIB_NET_NETLOG_H__


