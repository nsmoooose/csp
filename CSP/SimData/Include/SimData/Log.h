/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
#include <iostream>

#include <SimData/LogStream.h>
#include <SimData/Namespace.h>
#include <SimData/Export.h>


NAMESPACE_SIMDATA


/** Log message priorities
 *
 *  Arranged in order of increasing importance.  Each log entry has
 *  a priority.  Low priority messages can be suppressed by setting
 *  the threshold priority of the log instance (see LogStream for
 *  details).
 */
enum { LOG_BULK, LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ALERT, LOG_ERROR };

/** Log message categories used internally by SimData
 *
 *  An application using the SimData logging facilities should define
 *  its own specialized set of categories.
 */
enum {
	LOG_NONE      = 0x00000000,
	LOG_TYPE      = 0x00000001,
	LOG_ARCHIVE   = 0x00000002,
	LOG_REGISTRY  = 0x00000004,
	LOG_THREAD    = 0x00000008,
	LOG_ALL       = 0xffffffff
};


/** Display a fatal error message to stderr and exit.
 */
extern void fatal(std::string const &msg);


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
		log_stream->initFromEnvironment("SIMDATA_LOGFILE", "SIMDATA_LOGPRIORITY");
	}
	return *log_stream;
}


// when threads are disabled, use the main simdata LogStream instance
// directly.  when threads are enabled, access the main LogStream via
// thread-specific ThreadLog instances.  ThreadLog provides the same
// public interface as LogStream, but serializes access to the log().

#ifdef SIMDATA_NOTHREADS
#  define SIMDATA_LOG_ SIMDATA(log())

#else  // threading enabled
#  define SIMDATA_LOG_ SIMDATA(threadlog())

NAMESPACE_SIMDATA_END
#  include <SimData/ThreadLog.h>
NAMESPACE_SIMDATA

// swig doesn't like this for some reason
#  ifndef SWIG
inline SIMDATA_EXPORT ThreadLog& threadlog() {
	static ThreadSpecific<ThreadLog> specific_thread_log;
	return *specific_thread_log;
}
#  endif // SWIG

#endif // threading enabled


#ifdef SIMDATA_NDEBUG
# define SIMDATA_NOTEWORTHY(C, P) false
#else
# define SIMDATA_NOTEWORTHY(C, P) SIMDATA_LOG_.isNoteworthy(P, C)
#endif

/** Convenience macros for logging messages.
 *
 *  @param C category
 *  @param P priority
 *  @param M message
 */
# define SIMDATA_LOG(C,P,M) if (SIMDATA_NOTEWORTHY(C, P)) \
                              SIMDATA_LOG_.entry(P, C, __FILE__, __LINE__) << M << std::endl

# define SIMDATA_BULK(C,M) SIMDATA_LOG(C,SIMDATA(LOG_BULK),M)
# define SIMDATA_TRACE(C,M) SIMDATA_LOG(C,SIMDATA(LOG_TRACE),M)
# define SIMDATA_DEBUG(C,M) SIMDATA_LOG(C,SIMDATA(LOG_DEBUG),M)
# define SIMDATA_INFO(C,M) SIMDATA_LOG(C,SIMDATA(LOG_INFO),M)
# define SIMDATA_WARNING(C,M) SIMDATA_LOG(C,SIMDATA(LOG_WARNING),M)
# define SIMDATA_ALERT(C,M) SIMDATA_LOG(C,SIMDATA(LOG_ALERT),M)
# define SIMDATA_ERROR(C,M) SIMDATA_LOG(C,SIMDATA(LOG_ERROR),M)


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_LOG_H__

