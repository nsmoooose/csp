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

AC_DEFUN(OST_LIB_SOCKET,[
	AC_REQUIRE([OST_HEADER_SYSTIME])
	ost_cv_inet_sockets=no
	ost_cv_unix_sockets=no
	ost_cv_lib_socket="c"
	SOCKET_LIBS=""
	AC_CHECK_HEADERS(sys/socket.h,[
		AC_CHECK_HEADERS(select.h sys/select.h netinet/in_systm.h netinet/ip.h)
		AC_CHECK_HEADERS(arpa/inet.h,
			ost_cv_inet_sockets=yes)
		AC_CHECK_HEADERS(sys/sockio.h)
		AC_CHECK_HEADERS(sys/un.h,
			ost_cv_unix_sockets=yes)
		], [AC_CHECK_HEADERS(winsock.h,
			ost_cv_inet_sockets=yes)])
	AC_CHECK_LIB(socket, socket, [
		ost_cv_lib_socket="socket"
		LIBS="$LIBS -lsocket"
		SOCKET_LIBS="-lsocket"
		])

	if test $ost_cv_inet_sockets = yes ; then
		AC_CHECK_HEADERS(sys/libcsys.h)
		AC_DEFINE(HAVE_INET_SOCKETS)
		AC_CHECK_LIB(${ost_cv_lib_socket}, inet_aton,[
			AC_DEFINE(HAVE_INET_ATON)])
		AC_CACHE_CHECK([for socklen_t defined], ost_cv_socklen_t, [ 
			ost_cv_socklen_t='no'
                        AC_EGREP_HEADER(socklen_t, sys/socket.h,
                                ost_cv_socklen_t='yes',[
                                AC_EGREP_HEADER(socklen_t, cygwin/socket.h,
                                        ost_cv_socklen_t='yes')])
		])
		if test $ost_cv_socklen_t = yes ; then
			AC_DEFINE(HAVE_SOCKLEN_T)
		fi
	fi

	if test $ost_cv_unix_sockets = yes ; then
		AC_DEFINE(HAVE_UNIX_SOCKETS)
	fi
	AC_SUBST(SOCKET_LIBS)
])

dnl ACCONFIG TEMPLATE
dnl #define _POSIX_PII_SOCKET
dnl #undef HAVE_SYS_SOCKET_H
dnl #undef HAVE_ARPA_INET_H
dnl #undef HAVE_NETINET_IN_SYSTM_H
dnl #undef HAVE_NETINET_IP_H
dnl #undef HAVE_SYS_UN_H
dnl #undef HAVE_SELECT_H
dnl #undef HAVE_SYS_SELECT_H
dnl #undef HAVE_INET_ATON
dnl #undef HAVE_INET_SOCKETS
dnl #undef HAVE_UNIX_SOCKETS
dnl #undef HAVE_SOCKLEN_T
dnl #undef HAVE_SYS_LIBCSYS_H
dnl END ACCONFIG

dnl ACCONFIG BOTTOM
dnl 
dnl #ifdef HAVE_SYS_LIBCSYS_H
dnl #ifndef _AIX
dnl #include <sys/libcsys.h>
dnl #endif
dnl #endif
dnl  
dnl #ifdef HAVE_WINSOCK_H
dnl #include <winsock.h>
dnl #else
dnl #ifdef HAVE_SYS_SOCKET_H
dnl #include <sys/socket.h>
dnl #ifdef HAVE_SELECT_H
dnl #include <select.h>
dnl #else
dnl #ifdef HAVE_SYS_SELECT_H
dnl #include <sys/select.h>
dnl #endif
dnl #endif
dnl #ifdef HAVE_ARPA_INET_H
dnl #include <netinet/in.h>
dnl #include <arpa/inet.h>
dnl #include <netdb.h>
dnl #endif
dnl #ifdef HAVE_NETINET_IN_SYSTM_H
dnl #include <netinet/in_systm.h>
dnl #endif
dnl #ifdef HAVE_NETINET_IP_H
dnl #include <netinet/ip.h>
dnl #endif
dnl #ifdef HAVE_SYS_UN_H
dnl #include <sys/un.h>
dnl #endif
dnl #endif
dnl #endif
dnl 
dnl #ifndef HAVE_INET_ATON
dnl #define inet_aton(cp, addr) \
dnl 	(((*(unsigned long int *)(addr)) = inet_addr(cp)) != -1)
dnl #endif
dnl 
dnl #ifndef SUN_LEN
dnl #ifdef SCM_RIGHTS
dnl #define HAVE_UN_LEN
dnl #endif
dnl #ifdef __linux__
dnl #define HAVE_UN_LEN
dnl #endif
dnl #ifdef HAVE_UN_LEN
dnl #define SUN_LEN(ptr) sizeof(sockaddr_un.sun_len) + \
dnl 	sizeof(sockaddr_un.sun_family) + sizeof(sockaddr_un.sun_path) + 1
dnl #else
dnl #define SUN_LEN(ptr) ((size_t)((struct sockaddr_un *)0)->sun_path) \
dnl 	+ strlen((ptr)->sun_path))
dnl #endif
dnl #endif
dnl 
dnl #ifndef HAVE_SOCKLEN_T
dnl #if defined(i386) && defined(__svr4__)
dnl #define HAVE_SOCKLEN_U
dnl #endif
dnl #ifdef HAVE_SOCKLEN_U
dnl #if !defined(__CYGWIN32__) && !defined(__MINGW32__)
dnl typedef unsigned socklen_t;
dnl #else
dnl typedef int socklen_t;
dnl #endif
dnl #endif
dnl #endif
dnl 
dnl END ACCONFIG

