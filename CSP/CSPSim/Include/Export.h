 // Combat Simulator Project
 // Copyright (C) 2002-2005 The Combat Simulator Project
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
 //


/**
 * @file Export.h
 * @brief Macros needed to export/import symbols from shared libraries.
 */

#ifndef __CSPSIM_EXPORT_H__
#define __CSPSIM_EXPORT_H__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__) || defined( __MWERKS__)
	#  ifdef SWIG
	#    undef  __declspec
	#    define __declspec(x)
	#  endif // SWIG
	#  ifdef CSPSIMDLL_EXPORTS
	#    define CSPSIM_EXPORT   __declspec(dllexport)
	#  else
	#    define CSPSIM_EXPORT   __declspec(dllimport)
	#  endif // CSPSIMDLL_EXPORTS
#else
	#  define CSPSIM_EXPORT
#endif

# if defined(_MSC_VER) && (_MSC_VER <= 1310)
#pragma warning (disable : 4786 4251 4290)
# endif


#endif // __CSPSIM_EXPORT_H__

