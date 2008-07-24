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
 * @file Trace.cpp
 * @brief Automatic callstack traces reporting on error.
 */

#include <csp/csplib/util/Trace.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Verify.h>

#include <iostream>
#include <iomanip>
#include <csignal>
#include <cstdlib>


namespace csp {


// static StackTrace to avoid allocations in the signal handlers.
StackTrace AutoTrace::_trace;

// log for signal handler traces.  if null the main log is used.
LogStream *AutoTrace::_log = 0;

// preallocation that the signal handlers free to ensure heap space.
char *AutoTrace::_reserve = 0;

// flag used by the segv handler to prevent the sigabort handler from firing.
bool AutoTrace::_abort = true;



// Platform-specific implementation of stack traces.  Each platform and/or
// compiler must implement the StackTrace::TraceData class, which provides
// three public methods: acquire, valid, and writeToStream.

#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__APPLE__)

// TODO need to add a config test for these headers.
#include <dlfcn.h>
#include <execinfo.h>

/** A class that can acquire, store, and display a stack trace.
 */
class StackTrace::TraceData {
public:
	TraceData(): _skip(0), _stack_depth(0) { }

	bool acquire(int skip) {
		_skip = skip;
		_stack_depth = backtrace(_trace, MAX_STACK_DEPTH);
		return _stack_depth > 0;
	}

	/** Write the stack trace to a stream.  The output consists of one line per
	 *  stack frame, in the format:
	 *
	 *    xxxxxxxx yyyyyyyy zzzzzzzz filepath symbol
	 *
	 *  'xxxxxxxx' is the hex address of the call.
	 *  'yyyyyyyy' is the hex offset of the call point relative to the start of
	 *             the object file.
	 *  'zzzzzzzz' is the hex offset of the call point relative to the symbol.
	 *
	 *  For dynamically linked libraries, the offset relative to the object file
	 *  can be passed to addr2line to determine the source file and line number
	 *  of the call, while the absolute address should be used for static libraries
	 *  and executables.
	 */
	void writeToStream(std::ostream &os) const {
		if (_stack_depth == 0) os << "(stack trace unavailable)";
		for (int i = _skip; i < _stack_depth; ++i) {
			Dl_info info;
			if (dladdr(_trace[i], &info) != 0) {
				const uintptr_t addr = alias_cast<uintptr_t>(_trace[i]);
				const uintptr_t sofs = addr - alias_cast<uintptr_t>(info.dli_saddr);
				const uintptr_t bofs = addr - alias_cast<uintptr_t>(info.dli_fbase);
				if (!info.dli_fname) info.dli_fname = "??";
				if (!info.dli_sname) info.dli_sname = "";
				os << std::hex
					<< std::setw(8) << std::setfill('0') << addr << " "
					<< std::setw(8) << std::setfill('0') << bofs << " "
					<< std::setw(8) << std::setfill('0') << sofs << " "
					<< info.dli_fname << " "
					<< info.dli_sname << std::endl;
			}
		}
	}

	bool valid() const { return _stack_depth > 0; }

private:
	enum { MAX_STACK_DEPTH=64 };
	void *_trace[MAX_STACK_DEPTH];
	int _skip;
	int _stack_depth;
};

#else  // non-gcc or apple

// Windows version not yet implemented.  Please feel free to add it if
// you know how.

class StackTrace::TraceData {
public:
	bool acquire(int) { return false; }
	void writeToStream(std::ostream &) const { }
	void writeToDescriptor(int, char*, int) const { }
	bool valid() const { return false; }
};

#endif


// Platform-independent implementation.


StackTrace::StackTrace(): _data(new TraceData) { }

StackTrace::~StackTrace() {
	if (_data) delete _data;
}

bool StackTrace::valid() const {
	CSP_VERIFY(_data);
	return _data->valid();
}

bool StackTrace::acquire(int skip) {
	CSP_VERIFY(_data);
	return _data->acquire(skip);
}

void StackTrace::writeToStream(std::ostream &os) const {
	CSP_VERIFY(_data);
	_data->writeToStream(os);
}

StackTrace::StackTrace(StackTrace const &other): _data(new TraceData) {
	*this = other;
}

StackTrace const &StackTrace::operator=(StackTrace const &other) {
	CSP_VERIFY(_data && other._data);
	*_data = *other._data;
	return *this;
}


bool AutoTrace::install() {
	CSP_VERIFY(_reserve == 0);
	_reserve = (char*) malloc(4096); // we may need this space later!
	signal(SIGABRT, __sigabort);
	signal(SIGSEGV, __sigsegv);
	signal(SIGTERM, __sigterm);
	return true;
}

void AutoTrace::__sigsegv(int /*sig_n*/) {
	if (_reserve) free(_reserve); _reserve = 0;  // free some memory!
	_trace.acquire(/*skip=*/3);
	LogStream *logstream = log();
	// don't log at FATAL priority since that allocates another stack trace.
	LogStream::LogEntry(*logstream, LogStream::cError) << "FATAL ERROR: segmentation fault";
	logstream->trace(&_trace);
	_abort = false;  // prevent the abort handler from also logging a trace
	abort();
}

void AutoTrace::__sigabort(int /*sig_n*/) {
	if (_abort) {
		_abort = false;
		if (_reserve) free(_reserve); _reserve = 0;  // free some memory!
		_trace.acquire(/*skip=*/3);
		LogStream *logstream = log();
		LogStream::LogEntry(*logstream, LogStream::cFatal) << "ABORT";
	}
}

void AutoTrace::__sigterm(int /*sig_n*/) {
	_trace.acquire(/*skip=*/3);
	LogStream *logstream = log();
	_abort = false;
	LogStream::LogEntry(*logstream, LogStream::cFatal) << "TERMINATED";
}

LogStream *AutoTrace::log() {
	return _log ? _log : &CSP(log());
}

void AutoTrace::setLog(LogStream &log) {
	_log = &log;
}


} // namespace csp

