/* SimData: Data Infrastructure for Simulations
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file ThreadLog.cpp
 * @brief Threadsafe logging classes.
 **/

#ifndef SIMDATA_NOTHREADS

// Log.h and ThreadLog.h are co-dependent; always use Log.h.
#include <SimData/Log.h>

#include <iostream>


NAMESPACE_SIMDATA


/** A steam buffer class for thread-safe logging.
 *
 *  StringBufs are used internally by ThreadLog to provide serialized
 *  access to a LogStream.
 */
class ThreadLog::StringBuf: public NonCopyable, public std::streambuf
{
	std::string m_buffer;
	int m_category;
	int m_priority;
	const char *m_file;
	int m_line;
	LogStream &m_log;

	void init() {
		m_buffer.reserve(1024);
	}

	typedef ScopedLock<LogStream> ScopedLogLock;

public:

	/** Default constructor, connects to the global simdata log.
	 */
	StringBuf(): m_category(LOG_ALL), m_priority(LOG_INFO), m_file(0), m_line(0), m_log(::simdata::log()) {
		init();
	}

	/** Constructor, connects to the specified LogStream
	 */
	StringBuf(LogStream &base): m_category(LOG_ALL), m_priority(LOG_INFO), m_log(base) {
		init();
	}

	/** Cache the log entry parameters.
	 */
	inline void setPrefix(int priority, int category, const char *file, int line) {
		m_priority = priority;
		m_category = category;
		m_file = file;
		m_line = line;
	}

	/** Retrieve the associated LogStream.
	 */
	inline LogStream &getLogStream() const { return m_log; }

protected:

	/** Overflow (cache characters internally)
	 */
	virtual int overflow(int c) {
		m_buffer.push_back(static_cast<char>(c));
		return 1;
	}

	/** Sync/flush the cached log entry to the underlying LogStream
	 */
	virtual int sync() {
		ScopedLogLock lock(m_log);
		std::ostream &entry = m_log.entry(m_priority, m_category, m_file, m_line);
		entry << m_buffer;
		entry.flush();
		m_buffer.clear();
		return 1;
	}
};


std::ostream & ThreadLog::entry(int priority, int category, const char *file, int line) {
	m_stringbuf->setPrefix(priority, category, file, line);
	return *m_logstream;
}

ThreadLog::ThreadLog():
	m_stringbuf(new StringBuf()),
	m_logstream(new std::ostream(m_stringbuf.get())),
	m_log(m_stringbuf->getLogStream()) {}

ThreadLog::ThreadLog(LogStream &base):
	m_stringbuf(new StringBuf(base)),
	m_logstream(new std::ostream(m_stringbuf.get())),
	m_log(m_stringbuf->getLogStream()) {}

ThreadLog::~ThreadLog() {}

NAMESPACE_SIMDATA_END


#endif // SIMDATA_NOTHREADS

