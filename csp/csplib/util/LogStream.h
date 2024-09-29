#pragma once
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
 * @file LogStream.h
 * @brief Stream based logging mechanism.
 */

#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Uniform.h>

#include <iosfwd>
#include <string>
#include <ostream>
#include <cassert>


namespace csp {

class Mutex;
class StackTrace;

/** Thrown when logging a message at cFatal priority iff the LogStream has been
 *  configured to throw exceptions (rather than directly abort) by calling
 *  setThrowOnFatal.
 */
CSP_EXCEPTION(FatalException)


/** Class to manage the debug logging stream.
 */
class CSPLIB_EXPORT LogStream {
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
	 *    output path (e.g. "CSPLOG_FILE").
	 *  @param log_priority the environment variable specifying the log
	 *    priority threshold (e.g. "CSPLOG_PRIORITY").
	 *  @param log_flags the environment variable specifying the log
	 *    flags (e.g. "CSPLOG_FLAGS").
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

	void endl();
	void flush();
	void trace(StackTrace const *stacktrace=0);

	void setLocking(bool thread_safe=true) { m_threadsafe = thread_safe; }
	void lock();
	void unlock();

	uint64 initialThread() const { return m_initial_thread; }

	/** Test whether FATAL log messages generate exceptions or cause an immediate abort.
	 */
	bool getThrowOnFatal() const  { return m_throw_on_fatal; }

	/** Set whether FATAL log messages generate exceptions or cause an immediate abort.
	 */
	void setThrowOnFatal(bool throw_on_fatal=true) { m_throw_on_fatal = throw_on_fatal; }

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
     *  @param created an optional flag set to true if the named log did not
     *    exist before this call.
	 *  @return the existing logstream associated with the identifier, or a
	 *    new logstream.
	 */
	static LogStream *getOrCreateNamedLog(const std::string &name, bool *created=NULL);

	/** Helper class used to write a single entry to the log stream.  The
	 *  class constructs the log entry as a string internally, then writes
	 *  it to the LogStream when it goes out of scope.
	 */
	class LogEntry;

	/** Call for logstreams that are allocated on the heap and never deleted
	 *  to ensure that log messages will be properly flushed during static
	 *  destruction at program exit.
	 */
	void setNeverDeleted();

	/** Call to force the log to be flushed after each entry.  Note that
	 *  constant flushing is very inefficient.  This method is primarily
	 *  used internally in conjunction with setNeverDeleted.
	 */
	void setAlwaysFlush(bool flush) { m_autoflush = flush; }
	inline bool autoflush() const { return m_autoflush; }

	/** Returns the log filename set by logToFile, or an empty string if
     *  no filename has been assigned.
     */
	std::string const &filename() const { return m_filename; }

private:
	void init();
	void close();

	int m_flags;
	int m_priority;
	int m_categories;
	std::ostream *m_stream;
	std::ofstream *m_fstream;

	Mutex *m_mutex;
	bool m_threadsafe;
	uint64 m_initial_thread;
	bool m_throw_on_fatal;

	bool m_autoflush;
	bool m_never_deleted;

	std::string m_filename;
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
 *  See the various CSPLOG macros for more information.
 */
class CSPLIB_EXPORT LogStream::LogEntry {
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
	~LogEntry();

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

private:
	void prefix(const char *file, int linenum);
	void die();

	LogStream &m_stream;
	int m_priority;
	BufferStream m_buffer;
};


} // namespace csp
