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

#ifdef __GNUC__
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
	logstream *_log;
	bool _traced;
	void *_reference;

protected:
	TraceBase(): _precallback(0), _postcallback(0), _log(0), _traced(false), _reference(0) { }

	void setCallbacks_impl(Callback precallback, Callback postcallback) {
		_precallback = precallback;
		_postcallback = postcallback;
	}

	void setReference(void *ref) {
		_reference = ref;
	}

	void setLog_impl(logstream &log) {
		_log = &log;
	}

	std::ostream &log() {
		if (_log) {
			return (*_log) << loglevel(LOG_ALL, LOG_ERROR);
		} else {
			return simdata::log() << loglevel(LOG_ALL, LOG_ERROR);
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
			if (_reference) {
				log() << "reference = " << _reference << std::endl;
			}
		}
		_preCallback();
		_backtrace(skip, segv);
		_postCallback();
	}

	virtual void _backtrace(int skip, bool segv) {}
};


/** Singleton callstack trace error handler.
 */
class Trace: public TraceBase {

#ifdef __GNUC__
	virtual void _backtrace(int skip, bool segv=false) {
		void *trace[64];
		char **messages = (char **)NULL;
		int i, trace_size = 0;

		trace_size = backtrace(trace, 64);
		log() << "backtrace:" << std::endl;
		if (0 && segv) { // XXX needed?
			backtrace_symbols_fd(trace, trace_size, fileno(stderr));
			return;
		}
		messages = backtrace_symbols(trace, trace_size);
		if (messages) {
			for (i=skip; i<trace_size; ++i) {
				log() << "  " << messages[i] << std::endl;
			}
			free(messages);
		} else {
			log() << "  unavailable!" << std::endl;
		}
	}

	static void __sigsegv(int sign) {
		getTrace().error(3, true);
		abort();
	}

	static void __sigabort(int sign) {
		getTrace().error(3);
	}
#endif // __GNUC__

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
	static bool install(void *reference) {
#ifdef __GNUC__
		getTrace().setReference(reference);
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
	static void setLog(logstream &log) {
		getTrace().setLog_impl(log);
	}

};


NAMESPACE_SIMDATA_END


#endif // __TRACE_H__

