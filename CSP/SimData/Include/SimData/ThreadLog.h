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
#include <SimData/ScopedPointer.h>


NAMESPACE_SIMDATA


/** A thread-safe logging class that provides serialized access to an
 *  underlying LogStream instance.  The public interface is the same as
 *  LogStream, and the logging macro as defined to use thread-specific
 *  ThreadLog instances instead of the global LogStream whenever threading
 *  is enabled.
 */
class SIMDATA_EXPORT ThreadLog: public NonCopyable
{
	class StringBuf;
	typedef ScopedLock<LogStream> ScopedLogLock;
	ScopedPointer<StringBuf> m_stringbuf;
	ScopedPointer<std::ostream> m_logstream;
	LogStream &m_log;
	inline LogStream &getLogStream() const { return m_log; }

public:
	/** Default constructor, connects to the global simdata log.
	 */
	ThreadLog();

	/** Constructor, connects to the specified LogStream
	 */
	ThreadLog(LogStream &base);

	/** Destructor.
	 */
	~ThreadLog();

	/** Close the underlying (shared) LogStream.
	 */
	void _close() {
		ScopedLogLock lock(getLogStream());
		getLogStream()._close();
	}

	/** Set the output stream used by the underlying (shared) LogStream.
	 */
	void setOutput(std::ostream& out_) {
		ScopedLogLock lock(getLogStream());
		getLogStream().setOutput(out_);
	}

	/** Set the output file used by the underlying (shared) LogStream.
	 */
	void setOutput(std::string const &filename) {
		ScopedLogLock lock(getLogStream());
		getLogStream().setOutput(filename);
	}

	/** Set the logging priority threshold of the underlying (shared) LogStream.
	 */
	void setLogPriority(int p) {
		ScopedLogLock lock(getLogStream());
		getLogStream().setLogPriority(p);
	}

	/** Get the logging priority threshold of the underlying (shared) LogStream.
	 */
	int getLogPriority() const {
		return getLogStream().getLogPriority();
	}

	/** Set the logging category mask of the underlying (shared) LogStream.
	 */
	void setLogCategory(int c) {
		ScopedLogLock lock(getLogStream());
		getLogStream().setLogCategory(c);
	}

	/** Get the logging category mask of the underlying (shared) LogStream.
	 */
	int getLogCategory() {
		return getLogStream().getLogCategory();
	}

	/** Enable or disable point logging (source file and line number) by the
	 *  underlying (shared) LogStream.
	 */
	void setPointLogging(bool enabled) {
		ScopedLogLock lock(getLogStream());
		getLogStream().setPointLogging(enabled);
	}

	/** Get the point logging state of the underlying (shared) LogStream.
	 */
	bool getPointLogging() const {
		return getLogStream().getPointLogging();
	}

	/** Enable or disable time stamping of log entries written to the
	 *  underlying (shared) LogStream.
	 */
	void setTimeLogging(bool enabled) {
		ScopedLogLock lock(getLogStream());
		getLogStream().setTimeLogging(enabled);
	}

	/** Get the time logging state of the underlying (shared) LogStream.
	 */
	bool getTimeLogging() const {
		return getLogStream().getTimeLogging();
	}

	/** Test if a given priority and category are logable.
	 */
	inline bool isNoteworthy(int priority, int category=~0) {
		return getLogStream().isNoteworthy(priority, category);
	}

	/** Method for logging a message to the underlying (shared) LogStream.
	 *
	 *  @param priority priority of this message.
	 *  @param category category of this message (default ALL).
	 *  @param file source file that generated this message; typically __FILE__.
	 *  @param line line number of the code that generated this message (typically __LINE__).
	 *  @return an output stream to receive the message contents.
	 */
	std::ostream & entry(int priority, int category=LOG_ALL, const char *file=0, int line=0);

};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_THREADLOG_H__

#endif // SIMDATA_NOTHREADS
