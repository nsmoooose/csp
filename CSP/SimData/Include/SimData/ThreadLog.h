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


/** An output-only, category-based log stream.
 *
 *  logbuf is an output-only streambuf with the ability to disable sets of
 *  messages at runtime. Only messages with priority >= logbuf::logPriority
 *  and debugClass == logbuf::logClass are output.
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

	LogStream &m_log;
	typedef ScopedLock<LogStream> ScopedLogLock;

	StringBuf(): m_category(LOG_ALL), m_priority(LOG_INFO), m_file(0), m_line(0), m_log(::simdata::log()) {
		init();
	}

	StringBuf(LogStream &base): m_category(LOG_ALL), m_priority(LOG_INFO), m_log(base) {
		init();
	}

	virtual int overflow(int c) {
		m_buffer.push_back(static_cast<char>(c));
		return 1;
	}

	virtual int sync() {
		ScopedLogLock lock(m_log);
		std::ostream &entry = m_log.entry(m_priority, m_category, m_file, m_line);
		entry << m_buffer;
		entry.flush();
		m_buffer.clear();
		return 1;
	}

	inline void setPrefix(int priority, int category, const char *file, int line) {
		m_priority = priority;
		m_category = category;
		m_file = file;
		m_line = line;
	}

};

class SIMDATA_EXPORT ThreadLog: private StringBuf, protected std::ostream
{
public:
	ThreadLog(): std::ostream(this) {}

	ThreadLog(LogStream &base): StringBuf(base), std::ostream(this) {}

	void _close() {
		ScopedLogLock lock(m_log);
		m_log._close();
	}

	void setOutput(std::ostream& out_) {
		ScopedLogLock lock(m_log);
		m_log.setOutput(out_);
	}

	void setOutput(std::string const &filename) {
		ScopedLogLock lock(m_log);
		m_log.setOutput(filename);
	}

	void setLogPriority(int p) {
		ScopedLogLock lock(m_log);
		m_log.setLogPriority(p);
	}

	void setLogCategory(int c) {
		ScopedLogLock lock(m_log);
		m_log.setLogCategory(c);
	}

	void setPointLogging(bool enabled) {
		ScopedLogLock lock(m_log);
		m_log.setPointLogging(enabled);
	}

	bool getPointLogging() const {
		return m_log.getPointLogging();
	}

	void setTimeLogging(bool enabled) {
		ScopedLogLock lock(m_log);
		m_log.setTimeLogging(enabled);
	}

	bool getTimeLogging() const {
		return m_log.getTimeLogging();
	}

	std::ostream & entry(int priority, int category=LOG_ALL, const char *file=0, int line=0) {
		setPrefix(priority, category, file, line);
		return *this;
	}

};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_THREADLOG_H__

#endif // SIMDATA_NOTHREADS
