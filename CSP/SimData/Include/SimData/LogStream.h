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

#include <SimData/Namespace.h>
#include <SimData/Export.h>

#if defined(_MSC_VER)
# pragma warning(disable: 4275)
#endif

NAMESPACE_SIMDATA

using std::ostream;

// TODO:
//
// 1. Change output destination. Done.
// 2. Make logbuf thread safe.
// 3. Read environment for default debugClass and debugPriority.

/** An output-only, category-based log stream.
 *
 *  logbuf is an output-only streambuf with the ability to disable sets of
 *  messages at runtime. Only messages with priority >= logbuf::logPriority
 *  and debugClass == logbuf::logClass are output.
 */
class SIMDATA_EXPORT logbuf : public std::streambuf
{
public:

	/** Constructor */
	logbuf();

	/** Destructor */
	~logbuf();

	/** Is logging enabled?
	 *
	 * @return true or false
	 */
	bool enabled() { return logging_enabled; }

	/** Set the logging level of subsequent messages.
	 *
	 *  @param c debug class
	 *  @param p priority
	 */
	void set_log_state(int c, int p);

	/** Set the global logging level.
	 *
	 *  @param c debug class
	 *  @param p priority
	 */
	void set_log_level(int c, int p);


	/** Set the allowed logging classes.
	 *
	 *  @param c All enabled logging classes anded together.
	 */
	void set_log_classes(int c);


	/** Get the logging classes currently enabled.
	 *  @return All enabled debug logging anded together.
	 */
	int get_log_classes();


	/** Set the logging priority.
	 *
	 *  @param p The priority cutoff for logging messages.
	 */
	void set_log_priority(int p);


	/** Get the current logging priority.
	 *
	 *  @return The priority cutoff for logging messages.
	 */
	int get_log_priority ();


	/** Set the stream buffer
	 *
	 *  @param sb stream buffer
	 */
	void set_sb(std::streambuf* sb);

protected:

	/** sync/flush */
	inline virtual int sync();

	/** overflow */
	int overflow(int ch);

private:

	// The streambuf used for actual output. Defaults to cerr.rdbuf().
	std::streambuf* sbuf;

	bool logging_enabled;
	int logClass;
	int logPriority;

private:

	// Not defined.
	logbuf( const logbuf& );
	void operator= ( const logbuf& );
};


inline int logbuf::sync() {
	if (!sbuf) return -1;
	return sbuf->pubsync();
}


inline void logbuf::set_log_state(int c, int p) {
	logging_enabled = ((c & logClass) != 0 && p >= logPriority);
}


//inline logbuf::int_type
inline int logbuf::overflow(int c) {
	return logging_enabled ? sbuf->sputc(static_cast<char>(c)) : (EOF == 0 ? 1: 0);
}

/** LogStream manipulator for setting the log level of a message.
 */
struct loglevel {
	loglevel(int c, int p): logClass(c), logPriority(p) {}

	int logClass;
	int logPriority;
};


/** A helper class for logstream construction.
 *
 *  A helper class that ensures a streambuf and ostream are constructed and
 *  destroyed in the correct order.  The streambuf must be created before the
 *  ostream but bases are constructed before members.  Thus, making this class
 *  a private base of logstream, declared to the left of ostream, we ensure the
 *  correct order of construction and destruction.
 */
struct SIMDATA_EXPORT logstream_base
{
	logstream_base() {}
	logbuf lbuf;
};


/** Class to manage the debug logging stream.
 */
class SIMDATA_EXPORT logstream : private logstream_base, public std::ostream
{
	std::ofstream *m_out;
public:
	/** The default is to send messages to cerr.
	 *
	 *  @param out_ output stream
	 */
	logstream(std::ostream& out_)
	    : logstream_base(),
	      ostream(&lbuf), // msvc6 accepts ostream(&lbuf) _using std::ostream_, but not std::ostream(&lbuf) ...
	      m_out(NULL)
	{
		lbuf.set_sb(out_.rdbuf());
	}

	~logstream() {
		_close();
	}

	void _close() {
		lbuf.set_sb(NULL);
		if (m_out != NULL) {
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
		lbuf.set_sb(out_.rdbuf());
	}

	/** Set the output stream
	 *
	 *  @param fn output file path
	 */
	void setOutput(std::string const &fn) {
		_close();
		m_out = new std::ofstream(fn.c_str());
		assert(m_out != NULL);
		lbuf.set_sb(m_out->rdbuf());
	}

	/** Set the global log class and priority level.
	 *
	 *  @param c debug class
	 *  @param p priority
	 */
	void setLogLevels(int c, int p);

	/** Set the global log class.
	 *
	 *  @param c debug class
	 */
	void setLogClasses(int c);

	/** Output operator to capture the debug level and priority of a message.
	 *  @param l log level
	 */
	inline std::ostream& operator<< (const loglevel& l);
};


inline std::ostream& logstream::operator<< (const loglevel& l) {
	lbuf.set_log_state(l.logClass, l.logPriority);
	return *this;
}

NAMESPACE_SIMDATA_END


#endif // __SIMDATA_LOGSTREAM_H__


