
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
