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


#ifndef __SIMNET_NETLOG_H__
#define __SIMNET_NETLOG_H__

#include <SimData/Log.h>

namespace simnet {

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
inline simdata::LogStream& netlog() {
	static simdata::LogStream *log_stream = 0;
	if (log_stream == 0) log_stream = new simdata::LogStream();
	return *log_stream;
}


#if defined(NDEBUG) || defined(SIMNET_NDEBUG)
# define SIMNET_NOTEWORTHY(P, C) false
#else
# define SIMNET_NOTEWORTHY(P, C) ::simnet::netlog().isNoteworthy(simdata::LOG_##P, simnet::LOGCAT_##C)
#endif

# define SIMNET_LOG(P, C) \
	if (!SIMNET_NOTEWORTHY(P, C)); \
	else simdata::LogStream::LogEntry(static_cast<simdata::LogStream&>(simnet::netlog()), simdata::LOG_##P, __FILE__, __LINE__)


} // namespace simnet


#endif // __SIMNET_NETLOG_H__


