/* Combat Simulator Project
 * Copyright 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Trace.h
 * @brief Automatic callstack traces reporting on error.
 *
 * TODO does not show the offending address (need siginfo_t).
 * TODO need a windows implementation
 */


#ifndef __CSPLIB_UTIL_TRACE_H__
#define __CSPLIB_UTIL_TRACE_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Properties.h>
#include <iosfwd>


CSP_NAMESPACE


class LogStream;


/** A class that can acquire, store, and display a stack trace.
 */
CSPLIB_EXPORT class StackTrace {
public:
	StackTrace();
	~StackTrace();

	/** Acquire a stack track, omitting the first skip stack frames.
	 */
	bool acquire(int skip=0);

	/** Write the stack to a stream.  The output is platform dependent.
	 */
	void writeToStream(std::ostream &) const;

	/** Return true if acquire has been called successfully.
	 */
	bool valid() const;

	// warning: copying a stack trace is not guaranteed to be cheap.
	// a copy interface is provided to facilitate use with exceptions.
	StackTrace(StackTrace const&);
	StackTrace const &operator=(StackTrace const &);

private:
	struct TraceData;
	TraceData *_data;
};


inline std::ostream &operator<<(std::ostream &os, StackTrace const &trace) {
	trace.writeToStream(os);
	return os;
}


/** Static methods for installing signal handlers that log stack traces.
 */
CSPLIB_EXPORT class AutoTrace: public NonConstructable {
public:
	/** Install segv, abort, and sigterm handlers.
	 */
	static CSPLIB_EXPORT bool install();

	/** Set the logstream used by the signal handlers to log stack traces.
	 */
	static CSPLIB_EXPORT void setLog(LogStream &log);

	/** Disable the abort signal handler.  There is no way to reenable the
	 *  handler.  This is intended to be used before intentionally calling
	 *  abort after all relevant information has already been logged.
	 */
	static CSPLIB_EXPORT void inhibitAbortHandler() { _abort = false; }

private:
	// signal handlers.
	static CSPLIB_EXPORT void __sigterm(int /*sig_n*/);
	static CSPLIB_EXPORT void __sigsegv(int /*sig_n*/);
	static CSPLIB_EXPORT void __sigabort(int /*sig_n*/);

	static CSPLIB_EXPORT LogStream *log();

	static CSPLIB_EXPORT StackTrace _trace;
	static CSPLIB_EXPORT LogStream *_log;
	static CSPLIB_EXPORT char *_reserve;
	static CSPLIB_EXPORT bool _abort;
};

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_TRACE_H__

