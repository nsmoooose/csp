# generated automatically by aclocal 1.7.2 -*- Autoconf -*-

# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


AC_DEFUN(CSP_SDL_ERROR, [
  AC_MSG_ERROR([
  Simple DirectMedia Layer version >= $sdl_min_version must be installed.
  SDL can be downloaded from http://www.libsdl.org/  If you already have
  a sufficiently new version of SDL installed, this error probably means
  that the wrong copy of the sdl-config shell script is being found.
  ])
])
      

dnl Generic version check for libraries using 'xxx-config' scripts
AC_DEFUN(CSP_LIB_CONFIG, [
  lib_min_version=ifelse([$3], , 0.0.0, $3)
  lib=yes
  LIB_FLAGS=''
  AC_PATH_PROG(LIB_CONFIG, $2-config, no)
  AC_MSG_CHECKING(for $1 > $lib_min_version)
  no_lib=""
  if test "$LIB_CONFIG" = "no"; then
    no_lib=yes
  else
    lib_version=`$LIB_CONFIG --version` 
    lib_major=`echo $lib_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
    lib_minor=`echo $lib_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    lib_micro=`echo $lib_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    lib_major_min=`echo $lib_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
    lib_minor_min=`echo $lib_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    lib_micro_min=`echo $lib_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    lib_version_proper=`expr \
      $lib_major \> $lib_major_min \| \
      $lib_major \= $lib_major_min \& \
      $lib_minor \> $lib_minor_min \| \
      $lib_major \= $lib_major_min \& \
      $lib_minor \= $lib_minor_min \& \
      $lib_micro \>= $lib_micro_min `
    if test "$lib_version_proper" = "1" ; then
      AC_MSG_RESULT(yes)
      LIB_FLAGS=`$LIB_CONFIG --cflags`
    else
      AC_MSG_RESULT(no)
      no_lib=yes
    fi
   fi
   if test "$no_lib" = "yes"; then
	$4
   fi
   $2_FLAGS=$LIB_FLAGS
   AC_SUBST($2_FLAGS)
])

