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
 **/

/** \file logstream.hxx
 * Stream based logging mechanism.
 */

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
// $Id: LogStream.h,v 1.3 2003/02/02 20:53:27 mkrose Exp $

#ifndef __LOGSTREAM_H__
#define __LOGSTREAM_H__

#include <compiler.h>
#include <debug_types.h>

// At least Irix needs this
#ifdef SG_HAVE_NATIVE_SGI_COMPILERS
#include <char_traits.h>
#endif

#ifdef __GCC_3_0__
  #ifdef SG_HAVE_STD_INCLUDES
    # include <streambuf>
    # include <iostream>
  #else
    # include <iostream.h>
    # include <simgear/sg_traits.hxx>
  #endif
#else
  #include <iostream>
  #include STL_SSTREAM
#endif

#include <fstream>
#include <string>

#ifndef SG_HAVE_NATIVE_SGI_COMPILERS
SG_USING_STD(streambuf);
SG_USING_STD(ostream);
SG_USING_STD(cerr);
SG_USING_STD(endl);
#else
SG_USING_STD(char_traits);
#endif

#ifdef __MWERKS__
SG_USING_STD(iostream);
#endif

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
class logbuf : public streambuf
{
public:

#ifndef SG_HAVE_STD_INCLUDES
	///typedef char_traits<char>           traits_type;
	///typedef char_traits<char>::int_type int_type;
	// typedef char_traits<char>::pos_type pos_type;
	// typedef char_traits<char>::off_type off_type;
#endif
	// logbuf( streambuf* sb ) : sbuf(sb) {}
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
	void set_log_state( cspDebugClass c, cspDebugPriority p );

	/**
	* Set the global logging level.
	* @param c debug class
	* @param p priority
	*/
	static void set_log_level( cspDebugClass c, cspDebugPriority p );


	/**
	* Set the allowed logging classes.
	* @param c All enabled logging classes anded together.
	*/
	static void set_log_classes (cspDebugClass c);


	/**
	* Get the logging classes currently enabled.
	* @return All enabled debug logging anded together.
	*/
	static cspDebugClass get_log_classes ();


	/**
	* Set the logging priority.
	* @param c The priority cutoff for logging messages.
	*/
	static void set_log_priority (cspDebugPriority p);


	/**
	* Get the current logging priority.
	* @return The priority cutoff for logging messages.
	*/
	static cspDebugPriority get_log_priority ();


	/**
	* Set the stream buffer
	* @param sb stream buffer
	*/
	void set_sb( streambuf* sb );

protected:

	/** sync/flush */
	inline virtual int sync();

	/** overflow */
	///int_type overflow( int ch );
	int overflow( int ch );
	// int xsputn( const char* s, istreamsize n );

private:

	// The streambuf used for actual output. Defaults to cerr.rdbuf().
	static streambuf* sbuf;

	static bool logging_enabled;
	static cspDebugClass logClass;
	static cspDebugPriority logPriority;

private:

	// Not defined.
	logbuf( const logbuf& );
	void operator= ( const logbuf& );
};

inline int
logbuf::sync()
{
#ifdef SG_HAVE_STD_INCLUDES
	return sbuf->pubsync();
#else
	return sbuf->sync();
#endif
}

inline void
logbuf::set_log_state( cspDebugClass c, cspDebugPriority p )
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
	loglevel( cspDebugClass c, cspDebugPriority p )
		: logClass(c), logPriority(p) {}

	cspDebugClass logClass;
	cspDebugPriority logPriority;
};

/**
 * A helper class that ensures a streambuf and ostream are constructed and
 * destroyed in the correct order.  The streambuf must be created before the
 * ostream but bases are constructed before members.  Thus, making this class
 * a private base of logstream, declared to the left of ostream, we ensure the
 * correct order of construction and destruction.
 */
struct logstream_base
{
	// logstream_base( streambuf* sb ) : lbuf(sb) {}
	logstream_base() {}

	logbuf lbuf;
};

/**
 * Class to manage the debug logging stream.
 */
class logstream : private logstream_base, public ostream
{
	std::ofstream *m_out;
public:
	/**
	* The default is to send messages to cerr.
	* @param out output stream
	*/
	logstream( ostream& out )
		// : logstream_base(out.rdbuf()),
		: logstream_base(),
		  ostream(&lbuf),
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
	void set_output( ostream& out ) { 
		_close();
		lbuf.set_sb( out.rdbuf() ); 
	}

	/**
	* Set the output stream
	* @param out output stream
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
	void setLogLevels( cspDebugClass c, cspDebugPriority p );

	/**
	* Output operator to capture the debug level and priority of a message.
	* @param l log level
	*/
	inline ostream& operator<< ( const loglevel& l );
};

inline ostream&
logstream::operator<< ( const loglevel& l )
{
	lbuf.set_log_state( l.logClass, l.logPriority );
	return *this;
}


/**
 * \relates logstream
 * Return the one and only logstream instance.
 * We use a function instead of a global object so we are assured that cerr
 * has been initialised.
 * @return current logstream
 */
inline logstream&
csplog()
{
	static logstream logstrm( cerr );
	return logstrm;
}


/** \def SG_LOG(C,P,M)
 * Log a message.
 * @param C debug class
 * @param P priority
 * @param M message
 */
#ifdef CSP_NDEBUG
# define CSP_LOG(C,P,M)
#elif defined( __MWERKS__ )
# define CSP_LOG(C,P,M) ::csplog() << ::loglevel(C,P) << M << std::endl
#else
# define CSP_LOG(C,P,M) csplog() << loglevel(C,P) << M << std::endl
#endif


#endif // __LOGSTREAM_H__


