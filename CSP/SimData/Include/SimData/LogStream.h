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
 * @file LogStream.h
 * @brief Stream based logging mechanism.
 *
 * Stream based logging mechanism.
 *
 * Based on the LogStream library written by Bernie Bright, 1998
 * Copyright (C) 1998  Bernie Bright - bbright@c031.aone.net.au
 *
 * Source code from Bernie Bright's LogStream library is used
 * here under the turms of the GNU General Public License
 * version 2 or later, as allowed by the GNU Library General
 * Public License Version 2 (clause 3).
 */


#ifndef __SIMDATA_LOGSTREAM_H__
#define __SIMDATA_LOGSTREAM_H__

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <ctime>

#include <SimData/Namespace.h>
#include <SimData/Export.h>
#include <SimData/ThreadBase.h>

#if defined(_MSC_VER)
# pragma warning(disable: 4275)
#endif


NAMESPACE_SIMDATA


/** Standard logging interface:
	void _close()=0;
	void setOutput(std::ostream &)=0;
	void setOutput(std::string const &filename)=0;
	void setLogPriority(int priority)=0;
	void setLogCategory(int category)=0;
	std::ostream & entry(int priority, int category=LOG_ALL, const char *file=0, int line=0);
*/


/** An output-only, category-based log stream.
 *
 *  LogBuffer is an output-only streambuf with the ability to disable sets of
 *  messages at runtime.  Messages can be suppressed based on priority and
 *  category.
 */
class SIMDATA_EXPORT LogBuffer: public NonCopyable, public std::streambuf
{
public:

	/** Constructor */
	LogBuffer();

	/** Destructor */
	~LogBuffer();

	/** Test if logging is enabled.
	 */
	bool enabled() { return m_enabled; }

	/** Set the logging level of subsequent messages.
	 *
	 *  @param p message priority
	 *  @param c message category
	 */
	void setLogState(int p, int c) {
		m_enabled = ((c & m_category) != 0 && p >= m_priority);
	}

	/** Set the allowed logging categories.
	 *
	 *  @param c Logging categories to enable (bitwise AND of one or more
	 *    category constants); zero suppresses all categories.
	 */
	void setLogCategory(int c);

	/** Get the logging categories currently enabled.
	 *
	 *  @return bitwise AND of logging category constants
	 */
	int getLogCategory();

	/** Set the logging priority.  Messages with a lower priority will
	 *  be suppressed.
	 *
	 *  @param p the priority cutoff for logging messages
	 */
	void setLogPriority(int p);

	/** Get the current logging priority.
	 *
	 *  @return the priority cutoff for logging messages
	 */
	int getLogPriority();

	/** Set the underlying stream buffer
	 *
	 *  @param sb stream buffer used for output
	 */
	void set_sb(std::streambuf* sb);

protected:

	/** sync/flush the underlying streambuf */
	virtual int sync() {
		if (!m_buffer) return -1;
		return m_buffer->pubsync();
	}

	/** overflow (write characters to the underlying streambuf) */
	virtual int overflow(int c) {
		return m_enabled ? m_buffer->sputc(static_cast<char>(c)) : (EOF == 0 ? 1: 0);
	}

private:

	// The streambuf used for actual output. Defaults to cerr.rdbuf().
	std::streambuf* m_buffer;

	bool m_enabled;
	int m_category;
	int m_priority;
};


/** A helper class for LogStream construction.
 *
 *  A helper class that ensures a streambuf and ostream are constructed and
 *  destroyed in the correct order.  The streambuf must be created before the
 *  ostream but bases are constructed before members.  Thus, making this class
 *  a private base of LogStream, declared to the left of ostream, we ensure the
 *  correct order of construction and destruction.
 */
struct SIMDATA_EXPORT LogStreamBase
{
	LogStreamBase() {}
	LogBuffer m_log_buffer;
};


/** Class to manage the debug logging stream.
 */
class SIMDATA_EXPORT LogStream : private LogStreamBase, protected std::ostream
{
	std::ofstream *m_out;
	bool m_log_point;
	bool m_log_time;

#ifndef SIMDATA_NOTHREADS
	ThreadMutex m_mutex;
public:
	void lock() { m_mutex.lock(); }
	void unlock() { m_mutex.unlock(); }
#endif // SIMDATA_NOTHREADS

public:
	/** The default is to send messages to cerr.
	 *
	 *  @param out_ output stream
	 */
	LogStream(std::ostream& out_)
	        : LogStreamBase(),
	          std::ostream(&m_log_buffer),
	          m_out(NULL),
	          m_log_point(true),
	          m_log_time(false) {
		m_log_buffer.set_sb(out_.rdbuf());
	}

	/** Destructor; closes the output stream */
	virtual ~LogStream() { _close(); }

	/** Enable or disable point logging.
	 *
	 *  When point logging is enabled, log messages generated with macros that
	 *  pass __FILE__ and __LINE__ to LogStream::log will include the source
	 *  file and line number of the code that generated the log entry.  When
	 *  disabled, this information will be suppressed.
	 */
	void setPointLogging(bool enabled) { m_log_point = enabled; }

	/** Get the current point logging state.
	 *
	 *  @returns true if point logging is enabled; false otherwise.
	 */
	bool getPointLogging() const { return m_log_point; }

	/** Enable or disable time logging.
	 *
	 *  When time logging is enabled, each log entry will be time stamped.
	 */
	void setTimeLogging(bool enabled) { m_log_time = enabled; }

	/** Get the current time logging state.
	 *
	 *  @returns true if time logging is enabled; false otherwise.
	 */
	bool getTimeLogging() const { return m_log_time; }

	/** Configure logging parameters and destination base on
	 *  environment variables.  The default implementation uses
	 *  SIMDATA_LOGFILE and SIMDATA_PRIORITY to set the output
	 *  file and priority threshold.  This method is not called
	 *  by the constructor; call it explicitly if needed.
	 */
	virtual void initFromEnvironment();

	/** Close the underlying output stream.
	 *
	 *  Idempotent, and will not close cerr or cout.
	 */
	void _close() {
		m_log_buffer.set_sb(NULL);
		if (m_out != NULL && m_out != std::cerr && m_out != std::cout) {
			m_out->close();
			delete m_out;
			m_out = NULL;
		}
	}

	/** Set the output stream
	 *
	 *  @param out_ output stream
	 */
	void setOutput(std::ostream& out_) {
		_close();
		m_log_buffer.set_sb(out_.rdbuf());
	}

	/** Set the output stream
	 *
	 *  @param filename output file path
	 */
	void setOutput(std::string const &filename) {
		_close();
		m_out = new std::ofstream(filename.c_str());
		assert(m_out != NULL);
		m_log_buffer.set_sb(m_out->rdbuf());
	}

	/** Set the global log priority level.
	 *  Log entries with priority lower than this value will be suppressed.
	 *
	 *  @param p priority
	 */
	void setLogPriority(int p);

	/** Set the global log categories.
	 *  Log entries in categories not included in this set will be suppressed.
	 *
	 *  @param c categories (bitwise AND of one or more logging categories).
	 */
	void setLogCategory(int c);

	/** Method for logging a message to the stream.
	 *
	 *  @param priority priority of this message.
	 *  @param category category of this message (default ALL).
	 *  @param file source file that generated this message; typically __FILE__.
	 *  @param line line number of the code that generated this message (typically __LINE__).
	 *  @return an output stream to receive the message contents.
	 */
	std::ostream & entry(int priority, int category=~0, const char *file=0, int line=0) {
		m_log_buffer.setLogState(priority, category);
		*this << priority << ' ';
		if (m_log_time) {
			char time_stamp[32];
			time_t now;
			time(&now);
			strftime(time_stamp, 32, "%Y%m%d %H%M%S", gmtime(&now));
			*this << time_stamp << ' ';
		}
		if (file && m_log_point) {
			*this << '(' << file << ':' << line << ") ";
		}
		return *this;
	}

};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_LOGSTREAM_H__


