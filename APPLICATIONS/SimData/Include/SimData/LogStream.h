// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file LogStream.h
 *
 * Stream based logging mechanism.
 **/


// Written by Bernie Bright, 1998
//
// Copyright (C) 1998  Bernie Bright - bbright@c031.aone.net.au
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA  02111-1307, USA.
//
// $Id: LogStream.h,v 1.3 2003/03/26 06:38:03 mkrose Exp $

#ifndef __LOGSTREAM_H__
#define __LOGSTREAM_H__

#include <iostream>
#include <fstream>

#include <SimData/ns-simdata.h>
#include <SimData/Export.h>

NAMESPACE_SIMDATA

using std::ostream;

//
// TODO:
//
// 1. Change output destination. Done.
// 2. Make logbuf thread safe.
// 3. Read environment for default debugClass and debugPriority.
//

/**
 * logbuf is an output-only streambuf with the ability to disable sets of
 * messages at runtime. Only messages with priority >= logbuf::logPriority
 * and debugClass == logbuf::logClass are output.
 */
class SIMDATA_EXPORT logbuf : public std::streambuf
{
public:

	/** Constructor */
	logbuf();

	/** Destructor */
	~logbuf();

	/**
	* Is logging enabled?
	* @return true or false*/
	bool enabled() { return logging_enabled; }

	/**
	* Set the logging level of subsequent messages.
	* @param c debug class
	* @param p priority
	*/
	void set_log_state(int c, int p);

	/**
	* Set the global logging level.
	* @param c debug class
	* @param p priority
	*/
	static void set_log_level(int c, int p);


	/**
	* Set the allowed logging classes.
	* @param c All enabled logging classes anded together.
	*/
	static void set_log_classes(int c);


	/**
	* Get the logging classes currently enabled.
	* @return All enabled debug logging anded together.
	*/
	static int get_log_classes ();


	/**
	* Set the logging priority.
	* @param p The priority cutoff for logging messages.
	*/
	static void set_log_priority(int p);


	/**
	* Get the current logging priority.
	* @return The priority cutoff for logging messages.
	*/
	static int get_log_priority ();


	/**
	* Set the stream buffer
	* @param sb stream buffer
	*/
	void set_sb( std::streambuf* sb );

protected:

	/** sync/flush */
	inline virtual int sync();

	/** overflow */
	int overflow( int ch );

private:

	// The streambuf used for actual output. Defaults to cerr.rdbuf().
	static std::streambuf* sbuf;

	static bool logging_enabled;
	static int logClass;
	static int logPriority;

private:

	// Not defined.
	logbuf( const logbuf& );
	void operator= ( const logbuf& );
};

inline int
logbuf::sync()
{
	if (!sbuf) return -1;
	return 0;
}

inline void
logbuf::set_log_state(int c, int p)
{
	logging_enabled = ((c & logClass) != 0 && p >= logPriority);
}

///inline logbuf::int_type
inline int
logbuf::overflow( int c )
{
	return logging_enabled ? sbuf->sputc(c) : (EOF == 0 ? 1: 0);
}

/**
 * logstream manipulator for setting the log level of a message.
 */
struct loglevel
{
	loglevel(int c, int p)
		: logClass(c), logPriority(p) {}

	int logClass;
	int logPriority;
};

/**
 * A helper class that ensures a streambuf and ostream are constructed and
 * destroyed in the correct order.  The streambuf must be created before the
 * ostream but bases are constructed before members.  Thus, making this class
 * a private base of logstream, declared to the left of ostream, we ensure the
 * correct order of construction and destruction.
 */
struct SIMDATA_EXPORT logstream_base
{
	logstream_base() {}
	logbuf lbuf;
};
 
/**
 * Class to manage the debug logging stream.
 */
class SIMDATA_EXPORT logstream : private logstream_base, public std::ostream
{
	std::ofstream *m_out;
public:
	/**
	* The default is to send messages to cerr.
	* @param out output stream
	*/
	logstream( std::ostream& out )
		: logstream_base(),
		ostream(&lbuf), // msvc6 accepts ostream(&lbuf) _using std::ostream_, but not std::ostream(&lbuf) ...
		  m_out(NULL)
	{ 
		lbuf.set_sb(out.rdbuf());
	}

	~logstream() {
		_close();
	}

	void _close() {
		if (m_out != NULL) {
			m_out->close();
			delete m_out;
			m_out = NULL;
		}
	}

	/**
	* Set the output stream
	* @param out output stream
	*/
	void set_output( std::ostream& out ) { 
		_close();
		lbuf.set_sb( out.rdbuf() ); 
	}

	/**
	* Set the output stream
	* @param fn output file path
	*/
	void set_output( std::string const &fn ) { 
		_close();
		m_out = new std::ofstream(fn.c_str());
		lbuf.set_sb( m_out->rdbuf() ); 
	}

	/**
	* Set the global log class and priority level.
	* @param c debug class
	* @param p priority
	*/
	void setLogLevels(int c, int p);

	/**
	* Output operator to capture the debug level and priority of a message.
	* @param l log level
	*/
	inline std::ostream& operator<< ( const loglevel& l );
};

inline std::ostream&
logstream::operator<< ( const loglevel& l )
{
	lbuf.set_log_state( l.logClass, l.logPriority );
	return *this;
}

NAMESPACE_END // namespace simdata


#endif // __LOGSTREAM_H__


