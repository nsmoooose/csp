dnl WARNING:
dnl   I have almost no prior experience with GNU autoconf/automake and this
dnl   is very much a work in progress.  The original Makefile for Linux can
dnl   be found in Source/Makefile.orig.  If you have trouble with building
dnl   CSPFlightSim with this configure script or autmake files, and know
dnl   how to fix them, please do so and commit the changes to CVS.  Thanks!
dnl
dnl   -OS


m4_pushdef([UP], m4_translit([$1], [a-z], [A-Z]))dnl
m4_pushdef([DOWN], m4_translit([$1], [A-Z], [a-z]))dnl
m4_pushdef([LDOWN], ac_path_lib_[]DOWN)dnl
m4_pushdef([CACHE], ac_cv_path_lib_[]DOWN)dnl
m4_pushdef([ERRORLOG], error.[]DOWN[]test)dnl

AC_DEFUN(CSP_SDL_ERROR, [
  AC_MSG_ERROR([
  Simple DirectMedia Layer version >= $lib_min_version must be installed.
  SDL can be downloaded from http://www.libsdl.org/  If you already have
  a sufficiently new version of SDL installed, this error probably means
  that the wrong copy of the sdl-config shell script is being found.
  ])
])

AC_DEFUN(CSP_LIB_ERROR, [
  AC_MSG_ERROR([
  $1 version >= $2 must be installed.
  This library can be downloaded from $3.  
  If you already have a sufficiently new version of this library installed, 
  this error probably means that the wrong copy of the $4 shell 
  script is being found.
  ])
])
      

dnl Generic version check for libraries using 'xxx-config' scripts
AC_DEFUN(CSP_LIB_CONFIG, [
  lib_min_version=$3
  lib=yes
  AC_PATH_PROG($1[]_CONFIG, $2-config, no)
  AC_MSG_CHECKING(for $1 >= $lib_min_version)
  no_lib=""
  if test "$$1[]_CONFIG" = "no"; then
    no_lib=yes
  else
    lib_version=`$$1[]_CONFIG --version` 
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
      $1[]_FLAGS=`$$1[]_CONFIG --cflags`
      $1[]_LIBS=`$$1[]_CONFIG --libs`
    else
      AC_MSG_RESULT(no)
      no_lib=yes
    fi
   fi
   if test "$no_lib" = "yes"; then
	CSP_LIB_ERROR($4,$3,$5,$1[]_CONFIG)
   fi
   AC_SUBST($1[]_FLAGS)
   AC_SUBST($1[]_LIBS)
   LIBS="$$1[]_LIBS $LIBS"
])


AC_DEFUN(CSP_OSG_CONFIG, [
  AC_MSG_CHECKING(for $1 >= $2)
  LIBS="-l$1  $LIBS"
  AC_TRY_RUN([
/* Override any gcc2 internal prototype to avoid an error.  */
#ifdef __cplusplus
extern "C"
#endif
/* We use char because int might match the return type of a gcc2
   builtin and then its argument prototype would still apply.  */
const char *$1GetVersion ();
int main() {
	const char *v = $1GetVersion();
	const char *need = "$2";
	int major, minor, micro;
	int need_major, need_minor, need_micro;
	int n;
	n = sscanf(need, "%d.%d.%d", &need_major, &need_minor, &need_micro);
	if (n<3) exit(1);
	n = sscanf(v, "%d.%d.%d", &major, &minor, &micro);
	if (n<3) exit(1);
	if (major < need_major) exit(1);
	if (major > need_major) exit(0);
	if (minor < need_minor) exit(1);
	if (minor > need_minor) exit(0);
	if (micro < need_micro) exit(1);
	exit(0);
	return 0;
}
    ], 
    [AC_MSG_RESULT(yes)], 
    [AC_MSG_RESULT(no)
     AC_MSG_ERROR([
	$3 version >= $1 needs to be installed.  OSG can be
	downloaded from http://www.openscenegraph.org/
	])
    ], 
    [])
])

