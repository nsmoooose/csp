// Copyright (C) 1999-2002 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

/**
 * @file exception.h
 * @short GNU Common C++ exception model base classes.
 **/

#ifndef	CCXX_EXCEPTION_H_
#define	CCXX_EXCEPTION_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef	CCXX_STRCHAR_H_
#include <cc++/strchar.h>
#endif

// see if we support useful and std exception handling, else we ignore
// it for the rest of the system.

#if defined(HAVE_EXCEPTION)
#define	COMMON_STD_EXCEPTION

#include <exception>

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

/**
 * Mainline exception handler, this is the root for all Common C++
 * exceptions and assures the ansi C++ exception class hierarchy is both
 * followed and imported into the gnu Common C++ class hierarchy.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Base exception class for all Common C++ exceptions.
 */
class CCXX_CLASS_EXPORT Exception : public std::exception 
{
private:
	std::string _what;

public:
	Exception(const std::string& what_arg) throw();
	virtual ~Exception() throw();
	virtual const char *getString() const; 
	virtual const char *what() const throw() { return _what.c_str(); };
};

/**
 * A sub-hierarchy for all Common C++ I/O related classes.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short I/O operation exception hierarchy.
 */
class CCXX_CLASS_EXPORT IOException : public Exception
{
public:
	IOException(const std::string &what_arg) throw();
	virtual ~IOException() throw();
};


#ifdef	CCXX_NAMESPACES
};
#endif

#endif

#endif
