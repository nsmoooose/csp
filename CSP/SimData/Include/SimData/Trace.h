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
 * TODO difficult to decode addresses for libraries with lazy loading.
 * TODO what's the point of TraceBase?
 * TODO need a windows implementation
 */


#ifndef __SIMDATA_TRACE_H__
#define __SIMDATA_TRACE_H__

#include <SimData/Namespace.h>
#include <SimData/Log.h>
#include <SimData/Singleton.h>

#if defined(__GNUC__) && !defined(__MINGW32__)
# include <csignal>
# include <execinfo.h>
# include <exception>
# include <cstdlib>
# include <dlfcn.h>
#endif // __GNUC__


NAMESPACE_SIMDATA


/** Base class for implementing signal handlers and stack trace dumps.
 */
class TraceBase {
private:
	typedef void (*Callback)();
	Callback _precallback, _postcallback;
	LogStream *_log;
	bool _traced;

protected:
	TraceBase(): _precallback(0), _postcallback(0), _log(0), _traced(false) { }

	void setCallbacks_impl(Callback precallback, Callback postcallback) {
		_precallback = precallback;
		_postcallback = postcallback;
	}

	void setLog_impl(LogStream &log_) {
		_log = &log_;
	}

	std::ostream &log() {
		if (_log) {
			return _log->entry(LOG_ERROR, LOG_ALL);
		} else {
			return simdata::log().entry(LOG_ERROR, LOG_ALL);
		}
	}

	void _preCallback() {
		if (_precallback != 0) _precallback();
	}

	void _postCallback() {
		if (_postcallback != 0) _postcallback();
	}

	void error(int skip, bool segv=false) {
		if (_traced) return;
		_traced = true;
		if (segv) {
			log() << "FATAL ERROR: segmentation fault." << std::endl;
		}
		_preCallback();
		_backtrace(log(), skip);
		_postCallback();
	}

	virtual void _backtrace(std::ostream&, int skip) {}
};


/** Singleton callstack trace error handler.
 */
class Trace: public TraceBase {

public:

#if defined(__GNUC__) && !defined(__MINGW32__)

	static void StackDump(std::ostream &out, int skip=0) {
		void *trace[64];
		char **messages = (char **)NULL;
		int i, trace_size = 0;

		trace_size = backtrace(trace, 64);
		out << "CALL STACK BACKTRACE:" << std::endl;
		messages = backtrace_symbols(trace, trace_size);
		if (messages) {
			for (i=skip; i<trace_size; ++i) {
				out << "  " << messages[i] << std::endl;
			}
			free(messages);
		} else {
			out << "  unavailable!" << std::endl;
		}
	}

#else

	static void StackDump(std::ostream &, int=0) { }

#endif // __GNUC__

#if defined(__GNUC__) && !defined(__MINGW32__)

private:
	static void __sigsegv(int /*sig_n*/) {
		getTrace().error(3, true);
		abort();
	}

	static void __sigabort(int /*sig_n*/) {
		getTrace().error(3);
	}

#endif // __GNUC__

	virtual void _backtrace(std::ostream& out, int skip) {
		StackDump(out, skip);
	}

	friend class Singleton<Trace>;
	static Trace &getTrace() {
		return Singleton<Trace>::getInstance();
	}

	Trace(): TraceBase() {}
	virtual ~Trace() {}

public:

	/** Install a new backtrace error handler.
	 *
	 *  @returns true if succesful.
	 */
	static bool install() {
#if defined(__GNUC__) && !defined(__MINGW32__)
		signal(SIGABRT, __sigabort);
		signal(SIGSEGV, __sigsegv);
		return true;
#else // __GNUC__
		return false;
#endif
	}

	/** Set custom callbacks before and after the stacktrace.
	 */
	static void setCallbacks(void (*precallback)(), void (postcallback)()) {
		getTrace().setCallbacks_impl(precallback, postcallback);
	}

	/** Set the active log (defaults to the simdata log).
	 */
	static void setLog(LogStream &log) {
		getTrace().setLog_impl(log);
	}

};


NAMESPACE_SIMDATA_END


#endif // __TRACE_H__

