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

AC_DEFUN(OST_CC_GETOPT,[
	ost_cv_libgetopt_long=false
	GETOPT_LIBS=""
	LIBGETOPTOBJS=""
	AC_REQUIRE([OST_SYS_POSIX])
	AC_CHECK_FUNCS(getopt,
		[AC_DEFINE(HAVE_GETOPT)],[
		AC_CHECK_LIB(m, getopt,
			[AC_DEFINE(HAVE_GETOPT)
		 	GETOPT_LIBS="-lm"])
		])
	AC_CHECK_HEADERS(getopt.h)
	AC_CHECK_LIB(gnugetopt, getopt_long,
		[AC_DEFINE(HAVE_GETOPT_LONG)
		 GETOPT_LIBS="-lgnugetopt $GETOPT_LIBS"
		 LIBS="$LIBS -lgnugetopt"],
		[AC_CHECK_FUNCS(getopt_long,
			[AC_DEFINE(HAVE_GETOPT_LONG)],
			[LIBGETOPTOBJS="getopt.c getopt1.c"])]
	)
	AC_SUBST(GETOPT_LIBS)
	AC_SUBST(LIBGETOPTOBJS)
])

dnl ACCONFIG TEMPLATE
dnl #undef HAVE_GETOPT_LONG
dnl #undef HAVE_GETOPT_H
dnl END ACCONFIG

dnl ACCONFIG BOTTOM
dnl 
dnl 
dnl END ACCONFIG

