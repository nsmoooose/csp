/* Combat Simulator Project
 * Copyright 2002-2005 Mark Rose <mkrose@users.sf.net>
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
 * @file LogStream.cpp
 * @brief Stream based logging mechanism
 */


#include <csp/csplib/util/LogStream.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Trace.h>
#include <csp/csplib/thread/Synchronization.h>
#include <csp/csplib/thread/Thread.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>


namespace csp {


CSPLIB_EXPORT LogStream& internal_log() {
	static LogStream *log_stream = 0;
	// this method of access is safe (no race condition), since the first log
	// messages are generated during static initialization.
	if (log_stream == 0) {
		bool is_new = false;
		// default to stderr, which may be overridden by the environment
		// settings.  note that this LogStream instance is never freed, so
		// it is safe to log messages from static destructors.
		log_stream = LogStream::getOrCreateNamedLog("CSP", &is_new);
		if (is_new) {
			log_stream->initFromEnvironment("CSPLOG_FILE", "CSPLOG_PRIORITY", "CSPLOG_FLAGS");
			log_stream->setNeverDeleted();
		}
	}
	return *log_stream;
}


// A trivial registry for LogStreams.  Note that the registry does not take ownership of
// the pointers it contains, the log streams will persist unless explicitly deleted.  Be
// careful when deleting log streams, since logs may be written even during static
// destruction.
namespace {
	typedef std::map<std::string, LogStream *> LogStreamRegistry;
	LogStreamRegistry *NamedLogStreamRegistry = 0;

	inline void writeDigits(char *&buffer, int value) {
		*buffer++ = '0' + static_cast<char>(value / 10);
		*buffer++ = '0' + static_cast<char>(value % 10);
	}

	// Replaced gmtime_r and strftime with logTime, mainly because gmtime_r isn't
	// available on win32.  This implementation should also be faster.
	void logTime(const time_t t, char *buffer, bool time, bool date) {
		if (date) {
			const uint32_t days = static_cast<uint32>(t / 86400);

			// Taken from glib; originally from the Calendar FAQ.  Offset adjusted for
			// the unix epoch (rather than the Julian Period starting 1 Jan 4713 BC).
			const uint32_t A = days + 2440588 + 32045;
			const uint32_t B = (4 *(A + 36524)) / 146097 - 1;
			const uint32_t C = A - (146097*B) / 4;
			const uint32_t D = (4 * (C + 365)) / 1461 - 1;
			const uint32_t E = C - ((1461 * D) / 4);
			const uint32_t M = (5 * (E - 1) + 2) / 153;

			int year = static_cast<int>(100 * B + D - 4800 + (M/10));
			int month = static_cast<int>(M + 3 - 12 * (M/10));
			int day = static_cast<int>(E - (153 * M + 2) / 5);

			writeDigits(buffer, year / 100);
			writeDigits(buffer, year % 100);
			writeDigits(buffer, month);
			writeDigits(buffer, day);
			if (time) *buffer++ = ' ';
		}
		if (time) {
			int secs = static_cast<int>(t % 86400);
			int hour = secs / 3600;
			int min = (secs % 3600) / 60;
			int sec = secs % 60;
			writeDigits(buffer, hour);
			writeDigits(buffer, min);
			writeDigits(buffer, sec);
		}
		*buffer = 0;
	}

/** A private singleton used as a hook at static destruction to force
 *  logstreams that will never be deleted to enter autoflush mode.
 *  This ensures that the log will not be truncated when the program
 *  terminates.  LogStreams that are allocated on the heap and never
 *  deleted should be registered with this helper by calling
 *  setNeverDeleted().
 */
class AutoFlushAtExitHelper {
	// lazy construction to ensure proper initialization during static
	// construction.
	static std::vector<LogStream*> *m_streams;
	static Mutex *m_mutex;
public:
	// warning: this is not thread-safe!
	static void registerStream(LogStream* stream) {
		if (!m_mutex) m_mutex = new Mutex;
		if (!m_streams) m_streams = new std::vector<LogStream*>;
		m_mutex->lock();
		m_streams->push_back(stream);
		m_mutex->unlock();
	}
	AutoFlushAtExitHelper() {
		// force initialization in case no streams are registered during
		// static construction.
		if (!m_mutex) m_mutex = new Mutex;
		if (!m_streams) m_streams = new std::vector<LogStream*>;
	}
	~AutoFlushAtExitHelper() {
		for (std::vector<LogStream*>::iterator iter = m_streams->begin(); iter != m_streams->end(); ++iter) {
			(*iter)->setAlwaysFlush(true);
			(*iter)->flush();
		}
		delete m_streams;
		delete m_mutex;
	}
} AutoFlushAtExit;  // static instance

std::vector<LogStream*> *AutoFlushAtExitHelper::m_streams;
Mutex *AutoFlushAtExitHelper::m_mutex;

} // namespace


void LogStream::setNeverDeleted() {
	if (!m_never_deleted) {
		AutoFlushAtExitHelper::registerStream(this);
		m_never_deleted = true;
	}
}

void LogStream::initFromEnvironment(const char *log_file, const char *log_priority, const char *log_flags) {
	if (log_file) {
		char *env_logfile = getenv(log_file);
		if (env_logfile && *env_logfile) {
			logToFile(env_logfile);
		}
	}
	if (log_priority) {
		char *env_priority = getenv(log_priority);
		if (env_priority && *env_priority) {
			int priority = atoi(env_priority);
			if (priority < 0) priority = 0;
			if (priority > cFatal) priority = cFatal;
			setPriority(priority);
		}
	}
	if (log_flags) {
		char *env_flags = getenv(log_flags);
		if (env_flags && *env_flags) {
			int flags = atoi(env_flags);
			if (flags >= 0) setFlags(flags);
		}
	}
}

void LogStream::LogEntry::prefix(const char *filename, int linenum) {
	const int flags = m_stream.getFlags();
	if (flags & LogStream::cPriority) {
		m_buffer << ((m_priority >= 0 && m_priority <= 4) ? "DIWEF"[m_priority] : '?') << " ";
	}
	if (flags & (LogStream::cTimestamp|LogStream::cDatestamp)) {
		const time_t now = time(0);
		char time_stamp[32];
		logTime(now, time_stamp, (flags & LogStream::cTimestamp) != 0, (flags & LogStream::cDatestamp) != 0);
		m_buffer << time_stamp << ' ';
	}
#ifndef CSP_NOTHREADS
	if (flags & LogStream::cThread) {
		uint64_t thread_id = thread::id();
		if (thread_id != m_stream.initialThread()) {
			// compress the low 32 or 36 bits of thread id into 6 characters.  note that this is *not*
			// the standard base64 encoding.  the numbers are in front to make it more likely that the
			// final output resembles a numeric value.
			const char *encode64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
			char id_buffer[8];
			id_buffer[6] = ' ';
			id_buffer[7] = 0;
			for (int i = 5; i >= 0; --i) { id_buffer[i] = encode64[thread_id & 63]; thread_id >>= 6; }
			m_buffer << id_buffer;
		}
	}
#endif
	if (filename && (flags & LogStream::cLinestamp)) {
		const char *basename = filename;
		if ((flags & LogStream::cFullPath) == 0) {
			for (const char *scanner = filename; *scanner; ++scanner) {
				if (*scanner == ospath::DIR_SEPARATOR) basename = scanner + 1;
			}
		}
		m_buffer << '(' << basename << ':' << linenum << ") ";
	}
}

void LogStream::LogEntry::die() {
	m_stream.flush();
	m_stream.trace();
	if (m_stream.getThrowOnFatal()) { throw FatalException(m_buffer.get()); }
	AutoTrace::inhibitAbortHandler();
	::abort();
}

LogStream::LogEntry::~LogEntry() {
	m_stream.lock();
	m_stream.getStream() << m_buffer.get() << "\n";
	if (m_priority >= LogStream::cWarning || m_stream.autoflush()) m_stream.flush();
	m_stream.unlock();
	if (m_priority == LogStream::cFatal) die();
}

LogStream *LogStream::getOrCreateNamedLog(const std::string &name, bool *created) {
	if (created) *created = false;
	if (!NamedLogStreamRegistry) NamedLogStreamRegistry = new LogStreamRegistry;
	LogStreamRegistry::iterator iter = NamedLogStreamRegistry->find(name);
	if (iter == NamedLogStreamRegistry->end()) {
		if (created) *created = true;
		LogStream *logstream = new LogStream(std::cerr);
		NamedLogStreamRegistry->insert(std::make_pair(name, logstream));
		return logstream;
	}
	return iter->second;
}

void LogStream::init() {
	m_mutex = new Mutex;
	m_threadsafe = true;
	m_initial_thread = thread::id();
}

LogStream::LogStream():
		m_flags(cPriority|cTimestamp|cLinestamp|cThread),
		m_priority(cInfo),
		m_categories(~0),
		m_stream(&std::cerr),
		m_fstream(0),
		m_mutex(0),
		m_throw_on_fatal(false),
		m_autoflush(false),
		m_never_deleted(false) {
	init();
}

LogStream::LogStream(std::ostream& stream):
		m_flags(cPriority|cTimestamp|cLinestamp|cThread),
		m_priority(cInfo),
		m_categories(~0),
		m_stream(&stream),
		m_fstream(0),
		m_mutex(0),
		m_autoflush(false),
		m_never_deleted(false) {
	init();
}

LogStream::~LogStream() {
	close();
	delete m_mutex;
	assert(!m_never_deleted);
}

void LogStream::setStream(std::ostream &stream) {
	if (&stream != m_stream) {
		close();
		m_stream = &stream;
	}
}

void LogStream::close() {
	flush();
	if (m_fstream) {
		std::cout << "Closing logfile" << std::endl;
		m_fstream->close();
		delete m_fstream;
		m_fstream = 0;
	}
	m_stream = &std::cerr;
}

void LogStream::logToFile(std::string const &filename) {
	std::ofstream *target = new std::ofstream(filename.c_str());
	if (!target) {
	  CSPLOG(ERROR, ALL) << "Unable to open log stream to file " << filename;
	}
	close();
	if (target) {
		target->rdbuf()->pubsetbuf(0, 0);
		m_stream = target;
		m_filename = filename;
	} else {
		m_stream = &std::cerr;
		m_filename = "";
	}
	m_fstream = target;
}

void LogStream::endl() {
	if (m_stream) *m_stream << std::endl;
}

void LogStream::flush() {
	if (m_stream) m_stream->flush();
}

void LogStream::lock() {
	if (m_threadsafe && m_mutex) m_mutex->lock();
}

void LogStream::unlock() {
	if (m_threadsafe && m_mutex) m_mutex->unlock();
}

void LogStream::trace(StackTrace const *stacktrace) {
	if (!m_stream) return;
	if (!stacktrace) {
		StackTrace trace;
		trace.acquire(1);
		lock();
		*m_stream << trace;
	} else {
		lock();
		*m_stream << *stacktrace;
	}
	m_stream->flush();
	unlock();
}

void CSPLIB_EXPORT fatal(std::string const &msg) {
	std::cerr << "CSP fatal error:" << std::endl;
	std::cerr << msg << std::endl;
	// use abort() instead of exit() to trigger the SIGABRT handler (if installed)
	// and generate a stack trace.
	::abort();
}

// not really the right place for this, but convenient.
void CSPLIB_EXPORT _log_reference_count_error(int count, void *pointer) {
	CSPLOG(FATAL, ALL) << "ReferencedBase(" << pointer << ") deleted with non-zero reference count (" << count << "): memory corruption possible.";
}

// not really the right place for this, but convenient.
void CSPLIB_EXPORT _log_reference_conversion_error() {
	CSPLOG(ERROR, ALL) << "Ref() assignment: incompatible types (dynamic cast failed).";
}

} // namespace csp

