

// This file orginated from FlightGear, Modififed by Wolverine


// Stream based logging mechanism.
//
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
// $Id: LogStream.cpp,v 1.2 2003/03/21 20:07:55 mkrose Exp $

#include "stdinc.h"

bool            logbuf::logging_enabled = true;
cspDebugClass    logbuf::logClass        = CSP_NONE;
cspDebugPriority logbuf::logPriority     = CSP_INFO;
streambuf*      logbuf::sbuf            = NULL;

logbuf::logbuf()
{
//     if ( sbuf == NULL )
// 	sbuf = cerr.rdbuf();
}

logbuf::~logbuf()
{
    if ( sbuf )
	    sync();
}

void
logbuf::set_sb( streambuf* sb )
{
    if ( sbuf )
	    sync();

    sbuf = sb;
}

void
logbuf::set_log_level( cspDebugClass c, cspDebugPriority p )
{
    logClass = c;
    logPriority = p;
}

void
logbuf::set_log_classes (cspDebugClass c)
{
    logClass = c;
}

cspDebugClass
logbuf::get_log_classes ()
{
    return logClass;
}

void
logbuf::set_log_priority (cspDebugPriority p)
{
    logPriority = p;
}

cspDebugPriority
logbuf::get_log_priority ()
{
    return logPriority;
}

void
logstream::setLogLevels( cspDebugClass c, cspDebugPriority p )
{
    logbuf::set_log_level( c, p );
}


logstream::logstream( ostream& out )
		// : logstream_base(out.rdbuf()),
		: logstream_base(),
		  ostream(&lbuf),
		  m_out(NULL)
{ 
	lbuf.set_sb(out.rdbuf());
}

logstream::~logstream() {
	_close();
}

void logstream::_close() {
	if (m_out != NULL) {
		m_out->close();
		delete m_out;
		m_out = NULL;
	}
}

void logstream::set_output( ostream& out ) { 
	_close();
	lbuf.set_sb( out.rdbuf() ); 
}

void logstream::set_output( std::string const &fn ) { 
	_close();
	m_out = new std::ofstream(fn.c_str());
	lbuf.set_sb( m_out->rdbuf() ); 
}

logstream& csplog() {
	static logstream *logstrm = 0;
	if (!logstrm) {
		logstrm = new logstream( std::cerr );
	}
	return *logstrm;
}

