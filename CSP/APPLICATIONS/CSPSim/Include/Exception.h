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
 * @file Exception.h
 *
 **/

#ifndef __CSP_EXCEPTION_H__
#define __CSP_EXCEPTION_H__


#include <SimData/Exception.h>
#include "DemeterException.h"


namespace csp {

/**
 * Base class for all CSP exceptions
 */
class Exception: public simdata::ExceptionBase
{
public:
	Exception::Exception(std::string type="Exception", 
	                     std::string msg=""): 
		simdata::ExceptionBase(type, msg) {}
};

#define CSP_EXCEPTION(a) \
class a: public Exception { \
public: \
	a(std::string msg=""): \
		Exception(#a, msg) { } \
};


// Some common CSP exception classes
CSP_EXCEPTION(DataError);
CSP_EXCEPTION(DataArchiveError);

// Fatal error messages
void FatalException(Exception &e, std::string const &location);
void DemeterFatalException(DemeterException &e,  std::string const &location);
void SimDataFatalException(simdata::Exception &e,  std::string const &location);
void OtherFatalException(std::string const &location);


} // namespace csp


#endif // __CSP_EXCEPTION_H__
