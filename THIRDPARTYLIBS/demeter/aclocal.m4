dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.


AC_DEFUN(CSP_SDL_ERROR, [
  AC_MSG_ERROR([
  Simple DirectMedia Layer version >= $sdl_min_version must be installed.
  SDL can be downloaded from http://www.libsdl.org/  If you already have
  a sufficiently new version of SDL installed, this error probably means
  that the wrong copy of the sdl-config shell script is being found.
  ])
])
      

AC_DEFUN(CSP_TEST_SDL, [
  sdl_min_version=ifelse([$1], , 0.0.0, $1)
  sdl=yes
  SDL_FLAGS=''
  AC_PATH_PROG(SDL_CONFIG, sdl-config, no)
  AC_MSG_CHECKING(for SDL > $sdl_min_version)
  no_sdl=""
  if test "$SDL_CONFIG" = "no"; then
    no_sdl=yes
  else
    sdl_version=`$SDL_CONFIG --version` 
    sdl_major=`echo $sdl_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
    sdl_minor=`echo $sdl_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro=`echo $sdl_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    sdl_major_min=`echo $sdl_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
    sdl_minor_min=`echo $sdl_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro_min=`echo $sdl_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    sdl_version_proper=`expr \
      $sdl_major \> $sdl_major_min \| \
      $sdl_major \= $sdl_major_min \& \
      $sdl_minor \> $sdl_minor_min \| \
      $sdl_major \= $sdl_major_min \& \
      $sdl_minor \= $sdl_minor_min \& \
      $sdl_micro \>= $sdl_micro_min `
    if test "$sdl_version_proper" = "1" ; then
      AC_MSG_RESULT(yes)
      SDL_FLAGS=`$SDL_CONFIG --cflags`
    else
      AC_MSG_RESULT(no)
      no_sdl=yes
    fi
   fi
   if test "$no_sdl" = "yes"; then
	CSP_SDL_ERROR
   fi
   AC_SUBST(SDL_FLAGS)
])

  

