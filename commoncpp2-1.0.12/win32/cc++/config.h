// Copyright (C) 1999-2001 Open Source Telecom Corporation.
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

#ifndef CCXX_CONFIG_H_
#define CCXX_CONFIG_H_

// define automatically WIN32 for windows application compiled with Borland
#ifndef WIN32
# if defined(__BORLANDC__) && defined(_Windows)
#  define WIN32
# elif defined(_MSC_VER) && defined(_WIN32)
#  define WIN32
# endif
#endif

// check multithreading
#if defined(__BORLANDC__) && !defined(__MT__)
#  error Please enable multithreading
#endif
#if defined(_MSC_VER) && !defined(_MT)
#  error Please enable multithreading (Project -> Settings -> C/C++ -> Code Generation -> Use Runtime Library)
#endif

// check DLL compiling
#ifdef _MSC_VER
# ifndef _DLL
#  error Please enable DLL linking (Project -> Settings -> C/C++ -> Code Generation -> Use Runtime Library)
# endif
#endif

#ifndef CCXX_WIN32
#define CCXX_WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#ifdef WIN32
#ifndef ssize_t
#define ssize_t int
#endif
#endif

#undef	__DLLRTL
#undef CCXX_EXPORT
#undef CCXX_MEMBER
#undef CCXX_MEMBER_EXPORT
#undef CCXX_CLASS_EXPORT
#undef CCXX_EMPTY
#define CCXX_EMPTY


#if defined(__MINGW32__) && !defined(__MSVCRT__)
#define	CCXX_NOMSVCRT
#endif

#if defined(__MINGW32__) || defined(__CYGWIN32__)

#define	HAVE_OLD_IOSTREAM

#undef __EXPORT
#undef __stdcall
#define __stdcall
#define	__EXPORT
#define CCXX_EXPORT(t) t
#define CCXX_MEMBER(t) t
#define CCXX_MEMBER_EXPORT(t) t
#define CCXX_CLASS_EXPORT
typedef char int8;
typedef short int16;
typedef long int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
#ifdef __MINGW32__
# define HAVE_MODULES   1
# define alloca(x)      __builtin_alloca(x)
# define THROW(x)       throw x
# define THROWS(x)      throw(x)
  typedef unsigned int  uint;
# define        snprintf            _snprintf
# ifndef ETC_PREFIX
#   define ETC_PREFIX   "c:/"
# endif
#else
typedef DWORD size_t;
#endif

#else /* !defined(__MINGW32__) && !defined(__CYGWIN32__) */

#define	__DLLRTL  __declspec(dllexport)
#define	__EXPORT  __declspec(dllimport)

#define	snprintf	_snprintf

#define CCXX_EXPORT(t) __EXPORT t
#define CCXX_MEMBER(t) t
#define CCXX_MEMBER_EXPORT(t) __EXPORT CCXX_MEMBER(t CCXX_EMPTY)
#define CCXX_CLASS_EXPORT __EXPORT

typedef __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned int uint;
typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

#define THROW(x) throw x
#define THROWS(x) throw(x)
#define USING(x)

#define HAVE_MODULES 1
#undef  HAVE_PTHREAD_RWLOCK
#undef  PTHREAD_MUTEXTYPE_RECURSIVE

// define endian macros
#define __BYTE_ORDER __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321

#pragma warning (disable:4786)
#if _MSC_VER >= 1300
#pragma warning (disable:4290)
#endif

#ifndef	ETC_PREFIX
#define	ETC_PREFIX "c:/"
#endif

#endif

// use exception
#define HAVE_EXCEPTION 1
// use namespace
#define CCXX_NAMESPACES 1

#define	COMMON_TPPORT_TYPE_DEFINED

#define CCXX_HAVE_NEW_INIT

#define COMMON_XML_PARSING
#define HAVE_LIBXML

#define HAVE_SSTREAM

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
