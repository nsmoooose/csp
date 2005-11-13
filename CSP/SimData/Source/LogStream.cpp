/* SimData: Data Infrastructure for Simulations
 * Copyright 2002-2005 Mark Rose <mkrose@users.sf.net>
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
 * @file LogStream.cpp
 * @brief Stream based logging mechanism
 */


#include <SimData/LogStream.h>
#include <SimData/Log.h>
#include <SimData/FileUtility.h>
#include <SimData/Trace.h>
#include <SimData/Thread.h>
#include <SimData/ThreadUtil.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>


NAMESPACE_SIMDATA

// A trivial registry for LogStreams.  Note that the registry does not take ownership of
// the pointers it contains, the log streams will persist unless explicitly deleted.  Be
// careful when deleting log streams, since logs may be written even during static
// destruction.
namespace {
	typedef std::map<std::string, LogStream *> LogStreamRegistry;
	LogStreamRegistry NamedLogStreamRegistry;
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
			if (priority > FATAL) priority = FATAL;
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
	if (flags & LogStream::PRIORITY) {
		m_buffer << ((m_priority >= 0 && m_priority <= 4) ? "DIWEF"[m_priority] : '?') << " ";
	}
	if (flags & (LogStream::TIMESTAMP|LogStream::DATESTAMP)) {
		const time_t now = time(0);
		struct tm gmt;
		gmtime_r(&now, &gmt);
		char time_stamp[32];
		switch (flags & (LogStream::TIMESTAMP|LogStream::DATESTAMP)) {
			case LogStream::TIMESTAMP:
				strftime(time_stamp, 32, "%H%M%S", &gmt); break;
			case LogStream::DATESTAMP:
				strftime(time_stamp, 32, "%Y%m%d", &gmt); break;
			default:
				strftime(time_stamp, 32, "%Y%m%d %H%M%S", &gmt);
		}
		m_buffer << time_stamp << ' ';
	}
#ifndef SIMDATA_NOTHREADS
	if (flags & LogStream::THREAD) {
		pthread_t thread_id = thread::id();
		if (!pthread_equal(thread_id, m_stream.initialThread())) {
			// compress the low 32 or 36 bits of thread id into 6 characters.  note that this is *not*
			// the standard base64 encoding.  the numbers are in front to make it more likely that the
			// final output resembles a numeric value.
			const char *encode64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
			unsigned long id = pthread_t_to_ulong(thread_id);
			char id_buffer[8];
			id_buffer[6] = ' ';
			id_buffer[7] = 0;
			for (int i = 5; i >= 0; --i) { id_buffer[i] = encode64[id & 63]; id >>= 6; }
			m_buffer << id_buffer;
		}
	}
#endif
	if (filename && (flags & LogStream::LINESTAMP)) {
		const char *basename = filename;
		if ((flags & LogStream::FULLPATH) == 0) {
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
	AutoTrace::inhibitAbortHandler();
	::abort();
}

LogStream *LogStream::getOrCreateNamedLog(const std::string &name) {
	LogStreamRegistry::iterator iter = NamedLogStreamRegistry.find(name);
	if (iter == NamedLogStreamRegistry.end()) {
		LogStream *logstream = new LogStream(std::cerr);
		NamedLogStreamRegistry[name] = logstream;
		return logstream;
	}
	return iter->second;
}

void LogStream::init() {
#ifndef SIMDATA_NOTHREADS
	m_threadsafe = true;
	m_initial_thread = thread::id();
#endif
}

LogStream::LogStream():
		m_flags(PRIORITY|TIMESTAMP|LINESTAMP|THREAD),
		m_priority(INFO),
		m_categories(~0),
		m_stream(&std::cerr),
		m_fstream(0) {
	init();
}

LogStream::LogStream(std::ostream& stream):
		m_flags(PRIORITY|TIMESTAMP|LINESTAMP|THREAD),
		m_priority(INFO),
		m_categories(~0),
		m_stream(&stream),
		m_fstream(0) {
	init();
}

void LogStream::setStream(std::ostream &stream) {
	if (&stream != m_stream) {
		close();
		m_stream = &stream;
	}
}

void LogStream::close() {
	if (m_fstream) {
		m_fstream->close();
		delete m_fstream;
		m_fstream = 0;
	}
	m_stream = &std::cerr;
}

void LogStream::logToFile(std::string const &filename) {
	std::ofstream *target = new std::ofstream(filename.c_str());
	if (!target) SIMDATA_LOG(ERROR, ALL) << "Unable to open log stream to file " << filename;
	close();
	target->rdbuf()->pubsetbuf(0, 0);
	m_fstream = target;
	m_stream = m_fstream ? m_fstream : &std::cerr;
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

void fatal(std::string const &msg) {
	std::cerr << "SIMDATA fatal error:" << std::endl;
	std::cerr << msg << std::endl;
	// use abort() instead of exit() to trigger the SIGABRT handler (if installed)
	// and generate a stack trace.
	::abort();
}

// not really the right place for this, but convenient.
void SIMDATA_EXPORT _log_reference_count_error(int count, void *pointer) {
	SIMDATA_LOG(FATAL, ALL) << "simdata::ReferencedBase(" << pointer << ") deleted with non-zero reference count (" << count << "): memory corruption possible.";
}

// not really the right place for this, but convenient.
void SIMDATA_EXPORT _log_reference_conversion_error() {
	SIMDATA_LOG(ERROR, ALL) << "simdata::Ref() assignment: incompatible types (dynamic cast failed).";
}

NAMESPACE_SIMDATA_END

