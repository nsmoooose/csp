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

AC_DEFUN(OST_CC_DYNAMIC,[
	DYN_LOADER=''
	MODULE_FLAGS="-module -shared"
	ost_cv_dynloader=no
	AC_REQUIRE([OST_SYS_POSIX])
	AC_CHECK_HEADERS(dlfcn.h)
	AC_CHECK_LIB(dld, shl_load,
		[DYN_LOADER=-ldld,
		AC_DEFINE(HAVE_SHL_LOAD)])
	AC_CHECK_LIB(dl, dlopen, [DYN_LOADER=-ldl])
	if test ! -z "$DYN_LOADER" ; then
		ost_cv_dynloader=yes
		AC_DEFINE(HAVE_MODULES)
	else
		AC_CHECK_LIB(c, dlopen, [
			ost_cv_dynloader=yes
			AC_DEFINE(HAVE_MODULES)
			],[
				AC_CHECK_HEADERS(mach-o/dyld.h,[
					ost_cv_dynloader=yes
					MODULE_FLAGS=\
"-dynamic -bundle -undefined suppress -flat_namespace -read_only_relocs suppress"
					AC_DEFINE(HAVE_MODULES)
					AC_DEFINE(HAVE_MACH_DYLD)])
			])
	fi
	AC_SUBST(DYN_LOADER)	
	AC_SUBST(MODULE_FLAGS)
])

dnl ACCONFIG TEMPLATE
dnl #undef HAVE_DLOPEN
dnl #undef HAVE_MODULES
dnl #undef HAVE_DLFCN_H
dnl END ACCONFIG
