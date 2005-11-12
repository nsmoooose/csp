/* SimData: Data Infrastructure for Simulations
 * Copyright 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Trace.h
 * @brief Automatic callstack traces reporting on error.
 *
 * TODO does not show the offending address (need siginfo_t).
 * TODO need a windows implementation
 */


#ifndef __SIMDATA_TRACE_H__
#define __SIMDATA_TRACE_H__

#include <SimData/Export.h>
#include <SimData/Namespace.h>
#include <iosfwd>


NAMESPACE_SIMDATA


class LogStream;


/** A class that can acquire, store, and display a stack trace.
 */
SIMDATA_EXPORT class StackTrace {
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


SIMDATA_EXPORT class AutoTrace {
public:
	// install sigv and abort handlers.
	static bool install();
	static void setLog(LogStream &log);
	static void inhibitAbortHandler() { _abort = false; }

private:
	// signal handlers.
	static void __sigterm(int /*sig_n*/);
	static void __sigsegv(int /*sig_n*/);
	static void __sigabort(int /*sig_n*/);

	static LogStream *log();

	static StackTrace _trace;
	static LogStream *_log;
	static char *_reserve;
	static bool _abort;
};


NAMESPACE_SIMDATA_END


#endif // __TRACE_H__

