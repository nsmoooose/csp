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

#ifndef	CCXX_STRCHAR_H_
#define	CCXX_STRCHAR_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#include <cctype>
#include <string>
#include <cstring>

#ifdef	HAVE_STRINGS_H
extern "C" {
#include <strings.h>
};
#endif

#ifdef	HAVE_STRCASECMP
#ifndef	stricmp
#define	stricmp(x, y) strcasecmp(x, y)
#endif
#ifndef strnicmp
#define	strnicmp(x, y, n) strncasecmp(x, y, n)
#endif
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

class keystring
{
private:
	const char *c_str;

public:
	inline keystring(const char *s):
		c_str(s) {;};

	inline keystring(const keystring &copy):
		c_str(copy.c_str) {;};

	inline keystring():
		c_str(NULL) {;};

	virtual int compare(const char *s2)
		{return stricmp(c_str, s2);};

	inline const char *operator =(const char *s)
		{return c_str = s;};

	friend inline int operator==(keystring k1, const char *c)
		{return (k1.compare(c) == 0);};

	friend inline int operator!=(keystring k1, const char *c)
		{return (k1.compare(c) != 0);};

	friend inline int operator==(const char *c, keystring k1)
		{return (k1.compare(c) == 0);};

	friend inline int operator!=(const char *c, keystring k1)
		{return (k1.compare(c) != 0);};

	friend inline int operator!=(keystring k1, keystring k2)
		{return (k1.compare(k2) != 0);};

	friend inline int operator<(keystring k1, keystring k2)
		{return (k1.compare(k2) < 0);};

	friend inline int operator>(keystring k1, keystring k2)
		{return (k1.compare(k2) > 0);};

	friend inline int operator<=(keystring k1, keystring k2)
		{return (k1.compare(k2) <= 0);};

	friend inline int operator>=(keystring k1, keystring k2)
		{return (k1.compare(k2) >= 0);};

	friend inline int operator!(keystring k1)
		{return k1.c_str == NULL;};

	friend inline int length(keystring k1)
		{return static_cast<int>(strlen(k1.c_str));};

	friend inline const char *text(keystring k1)
		{return k1.c_str;};

	inline const char *operator ()()
		{return c_str;};

	inline operator const char *()
		{return c_str;};
};

#ifdef	CCXX_NAMESPACES
};
#endif

#endif
