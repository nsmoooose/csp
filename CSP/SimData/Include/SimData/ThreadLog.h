/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <tm2@stm.lbl.gov>
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
 * @file ThreadLog.h
 * @brief Thread safe interface to LogStream.
 */


#ifndef SIMDATA_NOTHREADS

#ifndef __SIMDATA_THREADLOG_H__
#define __SIMDATA_THREADLOG_H__

#include <SimData/Namespace.h>
#include <SimData/Export.h>
#include <SimData/ThreadBase.h>
#include <SimData/LogStream.h>
#include <SimData/Log.h>


NAMESPACE_SIMDATA


/** A steam buffer class for thread-safe logging.
 *
 *  StringBufs are used internally by ThreadLog to provide serialized
 *  access to a LogStream.
 */
class SIMDATA_EXPORT StringBuf: public NonCopyable, public std::streambuf
{
	std::string m_buffer;
	int m_category;
	int m_priority;
	const char *m_file;
	int m_line;

	void init() {
		m_buffer.reserve(1024);
	}

protected:

	// accessed directly by ThreadLog
	LogStream &m_log;
	typedef ScopedLock<LogStream> ScopedLogLock;

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

	/** Cache the log entry parameters.
	 */
	inline void setPrefix(int priority, int category, const char *file, int line) {
		m_priority = priority;
		m_category = category;
		m_file = file;
		m_line = line;
	}

};


/** A thread-safe logging class that provides serialized access to an
 *  underlying LogStream instance.  The public interface is the same as
 *  LogStream, and the logging macro as defined to use thread-specific
 *  ThreadLog instances instead of the global LogStream whenever threading
 *  is enabled.
 */
class SIMDATA_EXPORT ThreadLog: private StringBuf, protected std::ostream
{
public:
	/** Default constructor, connects to the global simdata log.
	 */
	ThreadLog(): std::ostream(this) {}

	/** Constructor, connects to the specified LogStream
	 */
	ThreadLog(LogStream &base): StringBuf(base), std::ostream(this) {}

	/** Close the underlying (shared) LogStream.
	 */
	void _close() {
		ScopedLogLock lock(m_log);
		m_log._close();
	}

	/** Set the output stream used by the underlying (shared) LogStream.
	 */
	void setOutput(std::ostream& out_) {
		ScopedLogLock lock(m_log);
		m_log.setOutput(out_);
	}

	/** Set the output file used by the underlying (shared) LogStream.
	 */
	void setOutput(std::string const &filename) {
		ScopedLogLock lock(m_log);
		m_log.setOutput(filename);
	}

	/** Set the logging priority threshold of the underlying (shared) LogStream.
	 */
	void setLogPriority(int p) {
		ScopedLogLock lock(m_log);
		m_log.setLogPriority(p);
	}

	/** Set the logging category mask of the underlying (shared) LogStream.
	 */
	void setLogCategory(int c) {
		ScopedLogLock lock(m_log);
		m_log.setLogCategory(c);
	}

	/** Enable or disable point logging (source file and line number) by the
	 *  underlying (shared) LogStream.
	 */
	void setPointLogging(bool enabled) {
		ScopedLogLock lock(m_log);
		m_log.setPointLogging(enabled);
	}

	/** Get the point logging state of the underlying (shared) LogStream.
	 */
	bool getPointLogging() const {
		return m_log.getPointLogging();
	}

	/** Enable or disable time stamping of log entries written to the
	 *  underlying (shared) LogStream.
	 */
	void setTimeLogging(bool enabled) {
		ScopedLogLock lock(m_log);
		m_log.setTimeLogging(enabled);
	}

	/** Get the time logging state of the underlying (shared) LogStream.
	 */
	bool getTimeLogging() const {
		return m_log.getTimeLogging();
	}

	/** Method for logging a message to the underlying (shared) LogStream.
	 *
	 *  @param priority priority of this message.
	 *  @param category category of this message (default ALL).
	 *  @param file source file that generated this message; typically __FILE__.
	 *  @param line line number of the code that generated this message (typically __LINE__).
	 *  @return an output stream to receive the message contents.
	 */
	std::ostream & entry(int priority, int category=LOG_ALL, const char *file=0, int line=0) {
		setPrefix(priority, category, file, line);
		return *this;
	}

};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_THREADLOG_H__

#endif // SIMDATA_NOTHREADS
