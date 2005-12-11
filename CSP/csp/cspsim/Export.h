/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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
 * @file Export.h
 * @brief Macros needed to export/import symbols from shared libraries.
 */

#ifndef __CSP_EXPORT_H__
#define __CSP_EXPORT_H__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__) || defined( __MWERKS__)
	#  ifdef SWIG
	#    undef  __declspec
	#    define __declspec(x)
	#  endif // SWIG
	#  ifdef CSP_EXPORTS
	#    define CSP_EXPORT   __declspec(dllexport)
	#  else
	#    define CSP_EXPORT   __declspec(dllimport)
	#  endif // CSP_EXPORTS
#else
	#  define CSP_EXPORT
#endif

# if defined(_MSC_VER) && (_MSC_VER <= 1400)
// 4251, level 1: 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
// 4290, level 3: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
// 4786, level 3: 'identifier' : identifier was truncated to 'number' characters in the debug information
// 4996, level 1: 'function': was declared deprecated
#pragma warning (disable : 4786 4251 4290 4996)
# endif


#endif // __CSP_EXPORT_H__

