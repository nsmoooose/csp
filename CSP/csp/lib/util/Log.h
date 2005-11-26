// Combat Simulator Project
// Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Log.h
 *
 **/


#ifndef __CSPLIB_UTIL_LOG_H__
#define __CSPLIB_UTIL_LOG_H__

#include <csp/lib/util/Namespace.h>
#include <csp/lib/util/Export.h>
#include <csp/lib/util/LogStream.h>
#include <csp/lib/util/LogConstants.h>

CSP_NAMESPACE

/**
 * Accessor for the main csp LogStream instance.  Under most circumstances
 * the CSPLOG macro should be used instead of this function.
 * @return csp LogStream
 */
inline LogStream& log() {
	static LogStream *log_stream = 0;
	// this method of access is safe (no race condition), since the first log
	// messages are generated during static initialization.
	if (log_stream == 0) {
		// default to stderr, which may be overridden by the environment
		// settings.  note that this LogStream instance is never freed, so
		// it is safe to log messages from static destructors.
		log_stream = LogStream::getOrCreateNamedLog("CSP");
		log_stream->initFromEnvironment("CSPLOG_FILE", "CSPLOG_PRIORITY", "CSPLOG_FLAGS");
	}
	return *log_stream;
}


#define CSPLOG_PRIORITY(P) CSP(cLogPriority_##P)
#define CSPLOG_CATEGORY(C) CSP(cLogCategory_##C)

// The static_cast helps gcc determine that log() is a function that returns
// a LogStream& when used in the LogEntry contstructor.
#define CSPLOG_ static_cast<CSP(LogStream&)>(CSP(log()))

#ifdef CSP_NDEBUG
# define CSPLOG_NOTEWORTHY(P, C) false
#else
# define CSPLOG_NOTEWORTHY(P, C) CSP(log()).isNoteworthy(CSPLOG_PRIORITY(P), CSPLOG_CATEGORY(C))
#endif

#define CSPLOG(P, C) \
	if (!CSPLOG_NOTEWORTHY(P, C)); \
	else CSP(LogStream::LogEntry)(CSPLOG_, CSPLOG_PRIORITY(P), __FILE__, __LINE__)


CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_LOG_H__

