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
  lib_min_version="$3"
  lib_flags_opt="$4"
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
      $1[]_FLAGS=`$$1[]_CONFIG $lib_flags_opt`
      $1[]_LIBS=`$$1[]_CONFIG --libs`
    else
      AC_MSG_RESULT(no)
      no_lib=yes
    fi
   fi
   if test "$no_lib" = "yes"; then
	CSP_LIB_ERROR($5,$3,$6,$1[]_CONFIG)
   fi
   AC_SUBST($1[]_FLAGS)
   AC_SUBST($1[]_LIBS)
   LIBS="$$1[]_LIBS $LIBS"
])

AC_DEFUN(_CSP_CHECK_PKG_CONFIG, [
	AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
])
		
dnl Generic version check for libraries using 'pkg-config' scripts
AC_DEFUN(_CSP_PKG_CONFIG, [
  if test "$PKG_CONFIG" != "no"; then
  	CSP_LIB_VERSION=`$PKG_CONFIG --silence-errors --modversion $1`
  	CSP_LIB_LIBS=`$PKG_CONFIG --silence-errors --libs $1`
  	CSP_LIB_CFLAGS=`$PKG_CONFIG --silence-errors --cflags $1`
  fi
])

AC_DEFUN(_CSP_LIB_CONFIG, [
  AC_PATH_PROG(LIB_CONFIG, $1-config, no)
  if test "$LIB_CONFIG" != "no"; then
    CSP_LIB_VERSION=`$LIB_CONFIG --version` 
    CSP_LIB_LIBS=`$LIB_CONFIG --libs` 
    CSP_LIB_CFLAGS=`$LIB_CONFIG $2` 
  fi
])


AC_DEFUN(CSP_CONFIG, [
  lib_name="$1"
  lib_id="$2"
  lib_pkg_id="$3"
  lib_min_version="$4"
  lib_flags_opt="$5"
  lib_fullname="$6"
  lib_site="$7"
  lib=yes
  CSP_LIB_VERSION="0.0.0"
  CSP_LIB_LIBS=""
  CSP_LIB_CFLAGS=""
  _CSP_LIB_CONFIG($lib_id, $lib_flags_opt)
  if test "$CSP_LIB_VERSION" = "0.0.0"; then
  	_CSP_PKG_CONFIG($lib_pkg_id)
  fi
  AC_MSG_CHECKING(for $lib_pkg_id >= $lib_min_version)
  no_lib=""
  lib_version="$CSP_LIB_VERSION"
  if test "$lib_version" = "0.0.0"; then
    no_lib=yes
    AC_MSG_RESULT(no)
  else
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
    else
      AC_MSG_RESULT(no)
      no_lib=yes
    fi
   fi
   if test "$no_lib" = "yes"; then
	CSP_LIB_ERROR($lib_fullname,$lib_min_version,$lib_site,$1[]_CONFIG)
   fi
   $1[]_FLAGS="$CSP_LIB_CFLAGS"
   $1[]_LIBS="$CSP_LIB_LIBS"
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
	$3 version >= $2 needs to be installed.  OSG can be
	downloaded from http://www.openscenegraph.org/
	])
    ], 
    [])
])

dnl check for python
AC_DEFUN(CSP_PYTHON, [
  min_version=$1
  python=no
  AC_MSG_CHECKING(for python >= $min_version)
  version=`python -V 2>&1`
  major=`echo $version | sed 's/Python \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
  minor=`echo $version | sed 's/Python \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'` 
  micro=`echo $version | sed 's/Python \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'` 
  major_min=`echo $min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
  minor_min=`echo $min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  micro_min=`echo $min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
  version_proper=`expr \
    $major \> $major_min \| \
    $major \= $major_min \& \
    $minor \> $minor_min \| \
    $major \= $major_min \& \
    $minor \= $minor_min \& \
    $micro \>= $micro_min `
  if test "$version_proper" = "1" ; then
    AC_MSG_RESULT(yes)
    python=yes
    PYTHON_PREFIX=`python -c 'import sys; print sys.prefix'`
    dnl hopefully distutils is installed...
    PYTHON_INCLUDE=`python -c 'from distutils.sysconfig import get_python_inc; print get_python_inc()'`
    AC_SUBST(PYTHON_PREFIX)
    AC_SUBST(PYTHON_INCLUDE)
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([
      Python version >= $min_version must be installed.  Python is
      avalable in prepackaged binary and source distributions from
      http://www.python.org.
    ])
  fi
])

dnl check for simdata
AC_DEFUN(CSP_SIMDATA, [
  min_version=$1
  dnl AC_MSG_CHECKING(for simdata >= $min_version)
  AC_MSG_CHECKING(for simdata)
  python -c 'import SimData' 1>/dev/null 2>&1
  if test "$?" = "0" ; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([
      SimData does not appear to be installed.  CVS snapshots of SimData
      are avalable from http://sourceforge.net/projects/csp
    ])
  fi
])

dnl check for swig
AC_DEFUN(CHECK_SWIG, [
  min_version=$1
  AC_MSG_CHECKING(for swig)
  swig_version_output=`swig -version 2>&1`
  major=`echo $swig_version_output | sed 's/.* \([[0-9]]\)\.\([[0-9]]\+\)\.\([[0-9]]\+\) .*/\1/'`
  minor=`echo $swig_version_output | sed 's/.* \([[0-9]]\)\.\([[0-9]]\+\)\.\([[0-9]]\+\) .*/\2/'`
  micro=`echo $swig_version_output | sed 's/.* \([[0-9]]\)\.\([[0-9]]\+\)\.\([[0-9]]\+\) .*/\3/'`
  major_min=`echo $min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'` 
  minor_min=`echo $min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  micro_min=`echo $min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
  version_proper=`expr \
    $major \> $major_min \| \
    $major \= $major_min \& \
    $minor \> $minor_min \| \
    $major \= $major_min \& \
    $minor \= $minor_min \& \
    $micro \>= $micro_min `
  new_style=`expr \
    $major \> 1 \| \
    $major \= 1 \& \
    $minor \> 3 \| \
    $major \= 1 \& \
    $minor \= 3 \& \
    $micro \>= 20 `
  if test "$version_proper" = "1" ; then
    AC_MSG_RESULT(yes)
    AC_MSG_CHECKING(swig runtime)
    if test "$new_style" = "1" ; then
      AC_MSG_RESULT(new style)
      SWIG_SWIGPY=''
      SWIG_RUNTIME='-noruntime'
      SWIG_NORUNTIME='-noruntime'
    else
      AC_MSG_RESULT(old style)
      SWIG_SWIGPY='-lswigpy'
      SWIG_RUNTIME=''
      SWIG_NORUNTIME='-c'
    fi
    AC_SUBST(SWIG_SWIGPY)
    AC_SUBST(SWIG_RUNTIME)
    AC_SUBST(SWIG_NORUNTIME)
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([
      SWIG version >= $min_version must be installed.  SWIG is
      avalable in prepackaged binary and source distributions from
      http://www.swig.org.
    ])
  fi
])
  
  

