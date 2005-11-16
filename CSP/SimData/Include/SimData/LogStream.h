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
 * @file LogStream.h
 * @brief Stream based logging mechanism.
 */


#ifndef __SIMDATA_LOGSTREAM_H__
#define __SIMDATA_LOGSTREAM_H__

#include <iosfwd>
#include <string>
#include <ostream>
#include <fstream>
#include <cassert>

#include <SimData/Namespace.h>
#include <SimData/Export.h>
#include <SimData/Synchronization.h>
#include <SimData/Uniform.h>


NAMESPACE_SIMDATA

class StackTrace;


/** Class to manage the debug logging stream.
 */
class SIMDATA_EXPORT LogStream {
public:
	/// Flags controlling log metadata.
	enum { cTerse=0, cPriority=1, cDatestamp=2, cTimestamp=4, cLinestamp=8, cFullPath=16, cThread=32, cVerbose=~0 };
	enum { cDebug=0, cInfo=1, cWarning=2, cError=3, cFatal=4 };

	/** Create a new log stream that defaults to stderr.
	 */
	LogStream();

	/** Create a new log stream attached to the specified output stream.
	 */
	LogStream(std::ostream& stream);

	/** Destructor; closes the output stream (if not cout or cerr).
	 */
	~LogStream();

	/** Configure logging parameters and destination base on environment variables.
	 *  This method is not called by the constructor; call it explicitly if needed.
	 *
	 *  @param log_file the environment variable specifying the log
	 *    output path (e.g. "SIMDATA_LOGFILE").
	 *  @param log_priority the environment variable specifying the log
	 *    priority threshold (e.g. "SIMDATA_LOGPRIORITY").
	 *  @param log_flags the environment variable specifying the log
	 *    flags (e.g. "SIMDATA_LOGFLAGS").
	 */
	void initFromEnvironment(const char *log_file, const char *log_priority, const char *log_flags);

	void setFlags(int flags) { m_flags = flags; }
	int getFlags() const { return m_flags; }

	void setPriority(int priority) { m_priority = priority; }
	int getPriority() const { return m_priority; }

	void setCategories(int categories) { m_categories = categories; }
	int getCategories() const { return m_categories; }

	void setStream(std::ostream &stream);
	std::ostream &getStream() { assert(m_stream); return *m_stream; }

	/** Write log messages to the specified file.
	 */
	void logToFile(std::string const &filename);

	void endl() { if (m_stream) *m_stream << std::endl; }
	void flush() { if (m_stream) m_stream->flush(); }
	void trace(StackTrace const *stacktrace=0);

#ifdef SIMDATA_NOTHREADS
	void setLocking(bool) { }
	void lock() { }
	void unlock() { }
#else
	void setLocking(bool thread_safe=true) { m_threadsafe = thread_safe; }
	void lock() { if (m_threadsafe) m_mutex.lock(); }
	void unlock() { if (m_threadsafe) m_mutex.unlock(); }
	pthread_t initialThread() const { return m_initial_thread; }
#endif // SIMDATA_NOTHREADS

	/** Test if a given priority and category should be logged.
	 */
	inline bool isNoteworthy(int priority, int category=~0) {
		return m_stream && (priority >= m_priority) && (category & m_categories);
	}

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


	/** Helper class used to write a single entry to the log stream.  The
	 *  class constructs the log entry as a string internally, then writes
	 *  it to the LogStream when it goes out of scope.
	 */
	class LogEntry;

private:
	void init();
	void close();

	int m_flags;
	int m_priority;
	int m_categories;
	std::ostream *m_stream;
	std::ofstream *m_fstream;

#ifndef SIMDATA_NOTHREADS
	ThreadMutex m_mutex;
	bool m_threadsafe;
	pthread_t m_initial_thread;
#endif
};


/** A trivial string stream that writes to a fixed size internal buffer.
 */
template <int SIZE>
class FixedStringBuffer: public std::basic_streambuf<char, std::char_traits<char> > {
public:
	FixedStringBuffer() { setp(m_buffer, m_buffer + SIZE); }
	char *get() {
		// ensure null termination
		sputc(0); m_buffer[SIZE-1] = 0;
		return m_buffer;
	}
private:
	char m_buffer[SIZE];
};


/** A helper class for writing a single entry to a log stream.  Assembles
 *  the log line in an internal buffer, which is written to the log when
 *  the LogEntry instance goes out of scope.  LogEntry is intended to be
 *  created anonymously on the stack, like this:
 *
 *  LogEntry(mylog) << "log entry message number " << count;
 *
 *  Often macros are used to create LogEntries, which allows the current
 *  file name and line number to be automatically recorded in the log.
 *  See the various SIMDATA_LOG macros for more information.
 */
class SIMDATA_EXPORT LogStream::LogEntry {
public:
	/** Create a new log entry in the specified log stream.
	 *
	 *  @param stream the logstream to write to.
	 *  @param priority priority of this message.
	 *    If non-negative and the logstream's flags include cPriority, this
	 *    priority will be recorded in the message.  Note that this is only
	 *    an advisory value; it is up to the creator of the LogEntry to
	 *    filter messages based on priority.
	 */
	LogEntry(LogStream &stream, int priority): m_stream(stream), m_priority(priority) {
		prefix(NULL, 0);
	}

	/** Create a new log entry in the specified log stream.
	 *
	 *  @param stream the logstream to write to.
	 *  @param priority priority of this message.
	 *    If non-negative and the logstream's flags includes cPriority, this
	 *    priority will be recorded in the message.  Note that this is only
	 *    an advisory value; it is up to the creator of the LogEntry to
	 *    filter messages based on priority.
	 *  @param filename source file that generated this message (typically __FILE__).
	 *  @param linenum line number of the code that generated this message (typically __LINE__).
	 */
	LogEntry(LogStream &stream, int priority, const char *filename, int linenum): m_stream(stream), m_priority(priority) {
		prefix(filename, linenum);
	}

	/** Write the buffered log entry to the logstream.  Locks the stream during the
	 *  write operation if running in a multithreaded environment.  If the priority
	 *  is cFatal, records a stack trace and aborts the program.
	 */
	~LogEntry() {
		m_stream.lock();
		m_stream.getStream() << m_buffer.get() << "\n";
		if (m_priority >= LogStream::cWarning) m_stream.flush();
		m_stream.unlock();
		if (m_priority == LogStream::cFatal) die();
	}

	/** Stream operator for recording messages in the log entry.
	 */
	template <typename T> LogEntry & operator<<(T const& x) {
		m_buffer << x;
		return *this;
	}

	/** Handle various ios formating objects (e.g., std::hex).
	 */
	template <typename T> LogEntry & operator<<(T& (*formatter)(T&)) {
		m_buffer << formatter;
		return *this;
	}

private:
	void prefix(const char *file, int linenum);
	void die();

	/** A fixed size string stream used to buffer the log entry text internally
	 *  before writing the completed string to the log stream.
	 */
	class BufferStream: public std::basic_ostream<char> {
	public:
		BufferStream(): std::basic_ostream<char>(&m_buffer) { }
		char *get() { return m_buffer.get(); }
	private:
		FixedStringBuffer<512> m_buffer;
	};

	LogStream &m_stream;
	int m_priority;
	BufferStream m_buffer;
};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_LOGSTREAM_H__


