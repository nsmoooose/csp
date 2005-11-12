/* SimData: Data Infrastructure for Simulations
 * Copyright 2002-2005 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This file is part of SimData.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/**
 * @file Log.h
 * @brief Constants and functions for writing to the SimData log.
 */


#ifndef __SIMDATA_LOG_H__
#define __SIMDATA_LOG_H__


#include <string>

#include <SimData/LogStream.h>
#include <SimData/Namespace.h>
#include <SimData/Export.h>
#include <SimData/LogConstants.h>


NAMESPACE_SIMDATA


/** Display a fatal error message to stderr and abort.
 */
extern SIMDATA_EXPORT void fatal(std::string const &msg);


/** Return the one and only LogStream instance.
 *
 *  We use a function instead of a global object so we are assured that the
 *  log will be available during static initialization.
 *
 *  @return current LogStream
 */
inline SIMDATA_EXPORT LogStream& log() {
	static LogStream *log_stream = 0;
	// this method of access is safe (no race condition), since the first log
	// messages are generated during static initialization.
	if (log_stream == 0) {
		// default to stderr, which may be overridden by the environment
		// settings.  note that this LogStream instance is never freed, so
		// it is safe to log messages from static destructors.
		log_stream = new LogStream();
		log_stream->initFromEnvironment("SIMDATA_LOGFILE", "SIMDATA_LOGPRIORITY", "SIMDATA_LOGFLAGS");
	}
	return *log_stream;
}


#define SIMDATA_LOG_ static_cast<SIMDATA(LogStream)&>(SIMDATA(log()))

#ifdef SIMDATA_NDEBUG
# define SIMDATA_NOTEWORTHY(P, C) false
#else
# define SIMDATA_NOTEWORTHY(P, C) SIMDATA_LOG_.isNoteworthy(P, C)
#endif

/** Convenience macros for logging messages.
 *
 *  @param C category
 *  @param P priority
 *  @param M message
 */
# define SIMDATA_LOG(P, C) \
	if (!SIMDATA_NOTEWORTHY(SIMDATA(LOG_##P), SIMDATA(LOG_##C))); \
	else SIMDATA(LogStream::LogEntry)(SIMDATA_LOG_, SIMDATA(LOG_##P), __FILE__, __LINE__)


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_LOG_H__

