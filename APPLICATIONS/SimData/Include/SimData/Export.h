#ifndef __EXPORT_H__
#define __EXPORT_H__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__) || defined( __MWERKS__)
	#  ifdef SIMDATA_EXPORTS
	#    define SIMDATA_EXPORT   __declspec(dllexport)
	#  else
	#    define SIMDATA_EXPORT   __declspec(dllimport)
	#  endif /*SIMDATA_EXPORTS */
#else
	#  define SIMDATA_EXPORT
#endif

#endif // __EXPORT_H__

