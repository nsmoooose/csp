dnl Copyright (C) 1999-2001 Open Source Telecom Corporation.
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software 
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl As a special exception to the GNU General Public License, if you 
dnl distribute this file as part of a program that contains a configuration 
dnl script generated by Autoconf, you may include it under the same 
dnl distribution terms that you use for the rest of that program.

AC_DEFUN(OST_WINVER,[
	WINVERSION="`echo $VERSION | sed 's/\./,/g'`,0"
	AC_SUBST(WINVERSION)
])

AC_DEFUN(OST_WIN32,
[
AC_REQUIRE([OST_PROG_CC_POSIX])
AC_CACHE_CHECK(for Win32 gnu environment, ost_cv_gnuwin32,
[
	AC_TRY_COMPILE([
		#ifdef	__WIN32__
		#ifdef __MINGW32__
		#define	__RETURN_WIN32__ __MINGW32__
		#endif
		#ifdef __CYGWIN32__
		#define __RETURN_WIN32__ __CYGWIN32__
		#endif
		#endif
	],[return __RETURN_WIN32__;],
	[ost_cv_gnuwin32=yes],[
	ost_cv_gnuwin32=no])
	rm -f conftest*
])
])

dnl ACCONFIG TEMPLATE
dnl #define __DLL
dnl #define __EXPORT
dnl #define __DLLRTL
dnl /* FIXME: old macro, should be removed */
dnl #define __MEMBER_EXPORT
dnl #undef CCXX_EMPTY
dnl #define CCXX_EMPTY
dnl #define CCXX_EXPORT(t) t
dnl #define CCXX_MEMBER(t) t
dnl #define CCXX_MEMBER_EXPORT(t) t
dnl #define CCXX_CLASS_EXPORT
dnl END ACCONFIG
