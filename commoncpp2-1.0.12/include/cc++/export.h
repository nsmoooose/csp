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
//

/**
 * @file export.h
 * @short export/import definitions for DLL's on Win32.
 *
 * This is used to swap rtl definitions for export vs import on Win32
 * machines when building a dll so that the remaining headers for my
 * new dll are all "exports", while the headers for the included ccxx
 * dll are all "imports".
 **/
#ifdef	__DLLRTL
#undef	__EXPORT
#define	__EXPORT __DLLRTL

#ifdef	CYGWIN_IMPORTS
#undef	CYGWIN_IMPORTS
#endif

#undef  CCXX_EXPORT
#define CCXX_EXPORT(t) __EXPORT t
#undef  CCXX_MEMBER
#define CCXX_MEMBER(t) t
#undef  CCXX_MEMBER_EXPORT
#define CCXX_MEMBER_EXPORT(t) __EXPORT CCXX_MEMBER(t CCXX_EMPTY)
#undef  CCXX_CLASS_EXPORT
#define CCXX_CLASS_EXPORT __EXPORT
#endif
