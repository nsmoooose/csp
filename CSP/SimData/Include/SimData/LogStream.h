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


/** Class to manage the debug logging stream.
 */
class SIMDATA_EXPORT LogStream
{
	std::ostream m_null;
	std::ostream *m_stream;
	std::ofstream *m_fstream;
	bool m_log_point;
	bool m_log_time;
	int m_category;
	int m_priority;

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
	LogStream(std::ostream& out_):
	          m_null(NULL),
	          m_stream(NULL),
	          m_fstream(NULL),
	          m_log_point(true),
	          m_log_time(false),
	          m_category(~0),
	          m_priority(0) {
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
	 *  environment variables.  This method is not called by
	 *  the constructor; call it explicitly if needed.
	 *
	 *  @param log_file the environment variable specifying the log
	 *    output path (e.g. "SIMDATA_LOGFILE").
	 *  @param log_priority the environment variable specifying the log
	 *    priority threshold (e.g. "SIMDATA_LOGPRIORITY").
	 */
	void initFromEnvironment(const char *log_file, const char *log_priority);

	/** Close the underlying output stream.
	 *
	 *  Idempotent, and will not close cerr or cout.
	 */
	void _close() {
		if (m_fstream != NULL) {
			m_fstream->close();
			delete m_fstream;
			m_fstream = NULL;
		}
		m_stream = NULL;
	}

	/** Set the output stream
	 *
	 *  @param out_ output stream
	 */
	void setOutput(std::ostream& out_) {
		_close();
		m_stream = &out_;
	}

	/** Set the output stream
	 *
	 *  @param out_ output file stream (LogStream will close it)
	 */
	void setOutput(std::ofstream& out_) {
		_close();
		m_fstream = &out_;
		m_stream = &out_;
	}

	/** Set the output stream
	 *
	 *  @param filename output file path
	 */
	void setOutput(std::string const &filename) {
		_close();
		m_fstream = new std::ofstream(filename.c_str());
		assert(m_fstream != NULL);
		m_stream = m_fstream;
	}

	/** Set the global log priority level.
	 *  Log entries with priority lower than this value will be suppressed.
	 *
	 *  @param p priority
	 */
	void setLogPriority(int p) { m_priority = p; }

	/** Get the current logging priority.
	 *
	 *  @return the priority cutoff for logging messages
	 */
	int getLogPriority() { return m_priority; }

	/** Set the global log categories.
	 *  Log entries in categories not included in this set will be suppressed.
	 *
	 *  @param c categories (bitwise AND of one or more logging categories).
	 */
	void setLogCategory(int c) { m_category = c; }

	/** Get the logging categories currently enabled.
	 *
	 *  @return bitwise AND of logging category constants
	 */
	int getLogCategory() { return m_category; }

	/** Test if a given priority and category are logable.
	 */
	inline bool isNoteworthy(int priority, int category=~0) {
		return ((category & m_category) != 0 && priority >= m_priority);
	}

	/** Method for logging a message to the stream.
	 *
	 *  @param priority priority of this message.
	 *  @param category category of this message (default ALL).
	 *  @param file source file that generated this message; typically __FILE__.
	 *  @param line line number of the code that generated this message (typically __LINE__).
	 *  @return an output stream to receive the message contents.
	 */
	std::ostream & entry(int priority, int category=~0, const char *file=0, int line=0);

	/** Get or create a new log stream assocated with a string identifier.
	 *
	 *  This method can be used to easily share one log file across multiple
	 *  components.
	 *
	 *  @param name a unique identifier for the log stream.
	 *  @return the existing logstream associated with the identifier, or a
	 *    new logstream.
	 */
	static LogStream *getOrCreateNamedLog(const std::string &name);

};



NAMESPACE_SIMDATA_END


#endif // __SIMDATA_LOGSTREAM_H__


