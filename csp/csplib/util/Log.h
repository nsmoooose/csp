#pragma once
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

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/LogStream.h>
#include <csp/csplib/util/LogConstants.h>

namespace csp {

// Internal accessor for the main csp LogStream.  Use log() instead, which
// is more efficient, or better yet the CSPLOG macros.
extern CSPLIB_EXPORT LogStream &internal_log();

/**
 * Accessor for the main csp LogStream instance.  Under most circumstances
 * the CSPLOG macro should be used instead of this function.
 * @return csp LogStream
 */
inline LogStream &log() {
	static LogStream *log = 0;
	if (log == 0) {
		log = &(internal_log());
	}
	return *log;
}

// The static_cast helps gcc determine that log() is a function that returns
// a LogStream& when used in the LogEntry contstructor.
#define CSPLOG_ static_cast<csp::LogStream&>(csp::log())

#ifdef CSP_NDEBUG
# define CSPLOG_NOTEWORTHY(P, C) false
#else
# define CSPLOG_NOTEWORTHY(P, C) csp::log().isNoteworthy(P, C)
#endif

#define CSPLOG(P, C) \
	if (!CSPLOG_NOTEWORTHY(P, C)); \
	else csp::LogStream::LogEntry(CSPLOG_, P, C, __FILE__, __LINE__)

} // namespace csp
