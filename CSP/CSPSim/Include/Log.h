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
 * @file Log.h
 *
 **/


#ifndef __CSP_LOG_H__
#define __CSP_LOG_H__

#include <SimData/Log.h>


/**
 * Define the possible classes/categories of logging messages
 */
enum {
	CSP_NONE        = 0x00000000,

	CSP_AUDIO       = 0x00000001,
	CSP_OPENGL      = 0x00000002,
	CSP_PROFILE     = 0x00000004,
	CSP_FILE        = 0x00000008,
	CSP_INPUT       = 0x00000010,
	CSP_MATH        = 0x00000020,
	CSP_NETWORK     = 0x00000040,
	CSP_SHARED      = 0x00000080,
	CSP_WINDOW      = 0x00000100,
	CSP_FACTORY     = 0x00000200,
	CSP_FRAMEWORK   = 0x00000400,
	CSP_LOG         = 0x00000800,
	CSP_TERRAIN     = 0x00001000,
	CSP_APP         = 0x00002000,
	CSP_GEOMETRY    = 0x00004000,
	CSP_PHYSICS     = 0x00008000,
	CSP_UNDEFD      = 0x00010000, // for range checking
	CSP_SCENE       = 0x00020000,
	CSP_BATTLEFIELD = 0x00040000,
	CSP_OBJECT      = 0x00080000,

	CSP_ALL         = 0xFFFFFFFF
};



/**
 * @relates LogStream
 * Return the one and only csp LogStream instance.
 * We use a function together with lazy construction so we are assured
 * that cerr has been initialised.
 * @return csp LogStream
 */
inline simdata::LogStream& csplog() {
	static simdata::LogStream *log_stream = 0;
	if (log_stream == 0) log_stream = new simdata::LogStream(std::cerr);
	return *log_stream;
}


#ifdef CSP_NDEBUG
# define CSP_LOG(C,P,M)
#else
# define CSP_LOG(C,P,M) \
	::csplog().entry(simdata::LOG_##P, CSP_##C, __FILE__, __LINE__) << M << std::endl
#endif


#endif // __CSP_LOG_H__


