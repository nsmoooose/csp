/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

/*
 * NOTE:
 *
 * This file contains only selected portion of the full glib,
 * based on the specific needs of CSP.
 *
 * UNFORTUNATELY, a big piece of what makes this work is glib's
 * autoconf script that tests things like byteorder and type
 * size to generate glibconfig.h.  Unless we want to copy all
 * of that, it may be simpler just to depend on the full glib.
 *
 */

#ifndef __GLIB_CSP_H__
#define __GLIB_CSP_H__

namespace hw {
	
#if defined (__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8))
#  define G_GNUC_EXTENSION __extension__
#else
#  define G_GNUC_EXTENSION
#endif

#define G_HAVE_GINT64 1

#ifdef WIN32
	typedef __int64 LLONG;
#else
	G_GNUC_EXTENSION typedef long long LLONG;

#endif

struct IntSizes {
	enum { CHAR = sizeof(char),
	       SHORT = sizeof(short),
	       INT = sizeof(int),
	       LONG = sizeof(long),
	       LONG_LONG = sizeof(LLONG),
	};
};

template <int bytes> struct CharIs { };
template <int bytes> struct ShortIs { };
template <int bytes> struct IntIs { };
template <int bytes> struct LongIs { };
template <int bytes> struct LongLongIs { };

template<>
struct LongLongIs<8>
{
	typedef LLONG T64;
};

template<>
struct LongIs<8>
{
	typedef long T64;
};

template<>
struct LongIs<4>
{
	typedef long T32;
	typedef LongLongIs<IntSizes::LONG_LONG>::T64 T64;
};

template<>
struct CharIs<1>
{
	typedef char T8;
};

template<>
struct ShortIs<2>
{
	typedef CharIs<IntSizes::CHAR>::T8 T8;
	typedef short T16;
};

template<>
struct IntIs<2>
{
	typedef ShortIs<IntSizes::SHORT>::T8 T8;
	typedef int T16;
	typedef LongIs<IntSizes::LONG>::T32 T32;
	typedef LongIs<IntSizes::LONG>::T64 T64;
};

template<>
struct IntIs<4>
{
	typedef ShortIs<IntSizes::SHORT>::T8 T8;
	typedef ShortIs<IntSizes::SHORT>::T16 T16;
	typedef int T32;
	typedef LongIs<IntSizes::LONG>::T64 T64;
};



typedef IntIs<IntSizes::INT>::T8 i8;
typedef IntIs<IntSizes::INT>::T16 i16;
typedef IntIs<IntSizes::INT>::T32 i32;
typedef IntIs<IntSizes::INT>::T64 i64;

} // namespace hw


typedef signed hw::i8 gint8;
typedef unsigned hw::i8 guint8;
typedef signed hw:i16 gint16;
typedef unsigned hw:i16 guint16;
typedef signed hw::i32 gint32;
typedef unsigned hw::i32 guint32;
typedef signed hw::i64 gint64;
typedef unsigned hw::i64 guint64;


// 





#ifdef WIN32
  #define NATIVE_WIN32
#endif // WIN32

#ifdef NATIVE_WIN32

/* On native Win32, directory separator is the backslash, and search path
 * separator is the semicolon.
 */
#define G_DIR_SEPARATOR '\\'
#define G_DIR_SEPARATOR_S "\\"
#define G_SEARCHPATH_SEPARATOR ';'
#define G_SEARCHPATH_SEPARATOR_S ";"

#else  /* !NATIVE_WIN32 */

#ifndef __EMX__
/* Unix */

#define G_DIR_SEPARATOR '/'
#define G_DIR_SEPARATOR_S "/"
#define G_SEARCHPATH_SEPARATOR ':'
#define G_SEARCHPATH_SEPARATOR_S ":"

#else
/* EMX/OS2 */

#define G_DIR_SEPARATOR '/'
#define G_DIR_SEPARATOR_S "/"
#define G_SEARCHPATH_SEPARATOR ';'
#define G_SEARCHPATH_SEPARATOR_S ";"

#endif

#endif /* !NATIVE_WIN32 */


/* Provide definitions for some commonly used macros.
 *  Some of them are only provided if they haven't already
 *  been defined. It is assumed that if they are already
 *  defined then the current definition is correct.
 */
#ifndef	NULL
#define	NULL	((void*) 0)
#endif

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef	ABS
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

#undef	CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


/* Provide type definitions for commonly used types.
 *  These are useful because a "gint8" can be adjusted
 *  to be 1 byte (8 bits) on all platforms. Similarly and
 *  more importantly, "gint32" can be adjusted to be
 *  4 bytes (32 bits) on all platforms.
 */

typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;
typedef gint   gboolean;

typedef unsigned char	guchar;
typedef unsigned short	gushort;
typedef unsigned long	gulong;
typedef unsigned int	guint;

typedef float	gfloat;
typedef double	gdouble;

/* HAVE_LONG_DOUBLE doesn't work correctly on all platforms.
 * Since gldouble isn't used anywhere, just disable it for now */

#if 0
#ifdef HAVE_LONG_DOUBLE
typedef long double gldouble;
#else /* HAVE_LONG_DOUBLE */
typedef double gldouble;
#endif /* HAVE_LONG_DOUBLE */
#endif /* 0 */

typedef void* gpointer;
typedef const void *gconstpointer;


typedef gint32	gssize;
typedef guint32 gsize;
typedef guint32 GQuark;
typedef gint32	GTime;


/* Portable endian checks and conversions
 *
 * glibconfig.h defines G_BYTE_ORDER which expands to one of
 * the below macros.
 */
#define G_LITTLE_ENDIAN 1234
#define G_BIG_ENDIAN    4321
#define G_PDP_ENDIAN    3412		/* unused, need specific PDP check */	


/* Basic bit swapping functions
 */
#define GUINT16_SWAP_LE_BE_CONSTANT(val)	((guint16) ( \
    (((guint16) (val) & (guint16) 0x00ffU) << 8) | \
    (((guint16) (val) & (guint16) 0xff00U) >> 8)))
#define GUINT32_SWAP_LE_BE_CONSTANT(val)	((guint32) ( \
    (((guint32) (val) & (guint32) 0x000000ffU) << 24) | \
    (((guint32) (val) & (guint32) 0x0000ff00U) <<  8) | \
    (((guint32) (val) & (guint32) 0x00ff0000U) >>  8) | \
    (((guint32) (val) & (guint32) 0xff000000U) >> 24)))

/* Intel specific stuff for speed
 */
#if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#  define GUINT16_SWAP_LE_BE_X86(val) \
     (__extension__					\
      ({ register guint16 __v;				\
	 if (__builtin_constant_p (val))		\
	   __v = GUINT16_SWAP_LE_BE_CONSTANT (val);	\
	 else						\
	   __asm__ __const__ ("rorw $8, %w0"		\
			      : "=r" (__v)		\
			      : "0" ((guint16) (val)));	\
	__v; }))
#  define GUINT16_SWAP_LE_BE(val) (GUINT16_SWAP_LE_BE_X86 (val))
#  if !defined(__i486__) && !defined(__i586__) \
      && !defined(__pentium__) && !defined(__i686__) && !defined(__pentiumpro__)
#     define GUINT32_SWAP_LE_BE_X86(val) \
        (__extension__						\
         ({ register guint32 __v;				\
	    if (__builtin_constant_p (val))			\
	      __v = GUINT32_SWAP_LE_BE_CONSTANT (val);		\
	  else							\
	    __asm__ __const__ ("rorw $8, %w0\n\t"		\
			       "rorl $16, %0\n\t"		\
			       "rorw $8, %w0"			\
			       : "=r" (__v)			\
			       : "0" ((guint32) (val)));	\
	__v; }))
#  else /* 486 and higher has bswap */
#     define GUINT32_SWAP_LE_BE_X86(val) \
        (__extension__						\
         ({ register guint32 __v;				\
	    if (__builtin_constant_p (val))			\
	      __v = GUINT32_SWAP_LE_BE_CONSTANT (val);		\
	  else							\
	    __asm__ __const__ ("bswap %0"			\
			       : "=r" (__v)			\
			       : "0" ((guint32) (val)));	\
	__v; }))
#  endif /* processor specific 32-bit stuff */
#  define GUINT32_SWAP_LE_BE(val) (GUINT32_SWAP_LE_BE_X86 (val))
#else /* !__i386__ */
#  define GUINT16_SWAP_LE_BE(val) (GUINT16_SWAP_LE_BE_CONSTANT (val))
#  define GUINT32_SWAP_LE_BE(val) (GUINT32_SWAP_LE_BE_CONSTANT (val))
#endif /* __i386__ */

#ifdef G_HAVE_GINT64
#  define GUINT64_SWAP_LE_BE_CONSTANT(val)	((guint64) ( \
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((guint64) (val) &						\
	(guint64) G_GINT64_CONSTANT(0xff00000000000000U)) >> 56)))
#  if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#    define GUINT64_SWAP_LE_BE_X86(val) \
	(__extension__						\
	 ({ union { guint64 __ll;				\
		    guint32 __l[2]; } __r;			\
	    if (__builtin_constant_p (val))			\
	      __r.__ll = GUINT64_SWAP_LE_BE_CONSTANT (val);	\
	    else						\
	      {							\
	 	union { guint64 __ll;				\
			guint32 __l[2]; } __w;			\
		__w.__ll = ((guint64) val);			\
		__r.__l[0] = GUINT32_SWAP_LE_BE (__w.__l[1]);	\
		__r.__l[1] = GUINT32_SWAP_LE_BE (__w.__l[0]);	\
	      }							\
	  __r.__ll; }))
#    define GUINT64_SWAP_LE_BE(val) (GUINT64_SWAP_LE_BE_X86 (val))
#  else /* !__i386__ */
#    define GUINT64_SWAP_LE_BE(val) (GUINT64_SWAP_LE_BE_CONSTANT(val))
#  endif
#endif

#define GUINT16_SWAP_LE_PDP(val)	((guint16) (val))
#define GUINT16_SWAP_BE_PDP(val)	(GUINT16_SWAP_LE_BE (val))
#define GUINT32_SWAP_LE_PDP(val)	((guint32) ( \
    (((guint32) (val) & (guint32) 0x0000ffffU) << 16) | \
    (((guint32) (val) & (guint32) 0xffff0000U) >> 16)))
#define GUINT32_SWAP_BE_PDP(val)	((guint32) ( \
    (((guint32) (val) & (guint32) 0x00ff00ffU) << 8) | \
    (((guint32) (val) & (guint32) 0xff00ff00U) >> 8)))

/* The G*_TO_?E() macros are defined in glibconfig.h.
 * The transformation is symmetric, so the FROM just maps to the TO.
 */
#define GINT16_FROM_LE(val)	(GINT16_TO_LE (val))
#define GUINT16_FROM_LE(val)	(GUINT16_TO_LE (val))
#define GINT16_FROM_BE(val)	(GINT16_TO_BE (val))
#define GUINT16_FROM_BE(val)	(GUINT16_TO_BE (val))
#define GINT32_FROM_LE(val)	(GINT32_TO_LE (val))
#define GUINT32_FROM_LE(val)	(GUINT32_TO_LE (val))
#define GINT32_FROM_BE(val)	(GINT32_TO_BE (val))
#define GUINT32_FROM_BE(val)	(GUINT32_TO_BE (val))

#ifdef G_HAVE_GINT64
#define GINT64_FROM_LE(val)	(GINT64_TO_LE (val))
#define GUINT64_FROM_LE(val)	(GUINT64_TO_LE (val))
#define GINT64_FROM_BE(val)	(GINT64_TO_BE (val))
#define GUINT64_FROM_BE(val)	(GUINT64_TO_BE (val))
#endif

#define GLONG_FROM_LE(val)	(GLONG_TO_LE (val))
#define GULONG_FROM_LE(val)	(GULONG_TO_LE (val))
#define GLONG_FROM_BE(val)	(GLONG_TO_BE (val))
#define GULONG_FROM_BE(val)	(GULONG_TO_BE (val))

#define GINT_FROM_LE(val)	(GINT_TO_LE (val))
#define GUINT_FROM_LE(val)	(GUINT_TO_LE (val))
#define GINT_FROM_BE(val)	(GINT_TO_BE (val))
#define GUINT_FROM_BE(val)	(GUINT_TO_BE (val))


/* Portable versions of host-network order stuff
 */
#define g_ntohl(val) (GUINT32_FROM_BE (val))
#define g_ntohs(val) (GUINT16_FROM_BE (val))
#define g_htonl(val) (GUINT32_TO_BE (val))
#define g_htons(val) (GUINT16_TO_BE (val))


#ifdef NATIVE_WIN32
#  ifdef GLIB_COMPILATION
#    define GUTILS_C_VAR __declspec(dllexport)
#  else /* !GLIB_COMPILATION */
#    define GUTILS_C_VAR extern __declspec(dllimport)
#  endif /* !GLIB_COMPILATION */
#else /* !NATIVE_WIN32 */
#  define GUTILS_C_VAR extern
#endif /* !NATIVE_WIN32 */



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	
/* GDate
 *
 * Date calculations (not time for now, to be resolved). These are a
 * mutant combination of Steffen Beyer's DateCalc routines
 * (http://www.perl.com/CPAN/authors/id/STBEY/) and Jon Trowbridge's
 * date routines (written for in-house software).  Written by Havoc
 * Pennington <hp@pobox.com> 
 */

typedef guint16 GDateYear;
typedef guint8  GDateDay;   /* day of the month */
typedef struct _GDate GDate;
/* make struct tm known without having to include time.h */
struct tm;

/* enum used to specify order of appearance in parsed date strings */
typedef enum
{
  G_DATE_DAY   = 0,
  G_DATE_MONTH = 1,
  G_DATE_YEAR  = 2
} GDateDMY;

/* actual week and month values */
typedef enum
{
  G_DATE_BAD_WEEKDAY  = 0,
  G_DATE_MONDAY       = 1,
  G_DATE_TUESDAY      = 2,
  G_DATE_WEDNESDAY    = 3,
  G_DATE_THURSDAY     = 4,
  G_DATE_FRIDAY       = 5,
  G_DATE_SATURDAY     = 6,
  G_DATE_SUNDAY       = 7
} GDateWeekday;
typedef enum
{
  G_DATE_BAD_MONTH = 0,
  G_DATE_JANUARY   = 1,
  G_DATE_FEBRUARY  = 2,
  G_DATE_MARCH     = 3,
  G_DATE_APRIL     = 4,
  G_DATE_MAY       = 5,
  G_DATE_JUNE      = 6,
  G_DATE_JULY      = 7,
  G_DATE_AUGUST    = 8,
  G_DATE_SEPTEMBER = 9,
  G_DATE_OCTOBER   = 10,
  G_DATE_NOVEMBER  = 11,
  G_DATE_DECEMBER  = 12
} GDateMonth;

#define G_DATE_BAD_JULIAN 0U
#define G_DATE_BAD_DAY    0U
#define G_DATE_BAD_YEAR   0U

/* Note: directly manipulating structs is generally a bad idea, but
 * in this case it's an *incredibly* bad idea, because all or part
 * of this struct can be invalid at any given time. Use the functions,
 * or you will get hosed, I promise.
 */
struct _GDate
{ 
  guint julian_days : 32; /* julian days representation - we use a
                           *  bitfield hoping that 64 bit platforms
                           *  will pack this whole struct in one big
                           *  int 
                           */

  guint julian : 1;    /* julian is valid */
  guint dmy    : 1;    /* dmy is valid */

  /* DMY representation */
  guint day    : 6;  
  guint month  : 4; 
  guint year   : 16; 
};

/* g_date_new() returns an invalid date, you then have to _set() stuff 
 * to get a usable object. You can also allocate a GDate statically,
 * then call g_date_clear() to initialize.
 */
GDate*       g_date_new                   (void);
GDate*       g_date_new_dmy               (GDateDay     day, 
                                           GDateMonth   month, 
                                           GDateYear    year);
GDate*       g_date_new_julian            (guint32      julian_day);
void         g_date_free                  (GDate       *date);

/* check g_date_valid() after doing an operation that might fail, like
 * _parse.  Almost all g_date operations are undefined on invalid
 * dates (the exceptions are the mutators, since you need those to
 * return to validity).  
 */
gboolean     g_date_valid                 (GDate       *date);
gboolean     g_date_valid_day             (GDateDay     day);
gboolean     g_date_valid_month           (GDateMonth   month);
gboolean     g_date_valid_year            (GDateYear    year);
gboolean     g_date_valid_weekday         (GDateWeekday weekday);
gboolean     g_date_valid_julian          (guint32      julian_date);
gboolean     g_date_valid_dmy             (GDateDay     day,
                                           GDateMonth   month,
                                           GDateYear    year);

GDateWeekday g_date_weekday               (GDate       *date);
GDateMonth   g_date_month                 (GDate       *date);
GDateYear    g_date_year                  (GDate       *date);
GDateDay     g_date_day                   (GDate       *date);
guint32      g_date_julian                (GDate       *date);
guint        g_date_day_of_year           (GDate       *date);

/* First monday/sunday is the start of week 1; if we haven't reached
 * that day, return 0. These are not ISO weeks of the year; that
 * routine needs to be added.
 * these functions return the number of weeks, starting on the
 * corrsponding day
 */
guint        g_date_monday_week_of_year   (GDate      *date);
guint        g_date_sunday_week_of_year   (GDate      *date);

/* If you create a static date struct you need to clear it to get it
 * in a sane state before use. You can clear a whole array at
 * once with the ndates argument.
 */
void         g_date_clear                 (GDate       *date, 
                                           guint        n_dates);

/* The parse routine is meant for dates typed in by a user, so it
 * permits many formats but tries to catch common typos. If your data
 * needs to be strictly validated, it is not an appropriate function.
 */
void         g_date_set_parse             (GDate       *date,
                                           const gchar *str);
void         g_date_set_time              (GDate       *date, 
                                           GTime        time);
void         g_date_set_month             (GDate       *date, 
                                           GDateMonth   month);
void         g_date_set_day               (GDate       *date, 
                                           GDateDay     day);
void         g_date_set_year              (GDate       *date,
                                           GDateYear    year);
void         g_date_set_dmy               (GDate       *date,
                                           GDateDay     day,
                                           GDateMonth   month,
                                           GDateYear    y);
void         g_date_set_julian            (GDate       *date,
                                           guint32      julian_date);
gboolean     g_date_is_first_of_month     (GDate       *date);
gboolean     g_date_is_last_of_month      (GDate       *date);

/* To go forward by some number of weeks just go forward weeks*7 days */
void         g_date_add_days              (GDate       *date, 
                                           guint        n_days);
void         g_date_subtract_days         (GDate       *date, 
                                           guint        n_days);

/* If you add/sub months while day > 28, the day might change */
void         g_date_add_months            (GDate       *date,
                                           guint        n_months);
void         g_date_subtract_months       (GDate       *date,
                                           guint        n_months);

/* If it's feb 29, changing years can move you to the 28th */
void         g_date_add_years             (GDate       *date,
                                           guint        n_years);
void         g_date_subtract_years        (GDate       *date,
                                           guint        n_years);
gboolean     g_date_is_leap_year          (GDateYear    year);
guint8       g_date_days_in_month         (GDateMonth   month, 
                                           GDateYear    year);
guint8       g_date_monday_weeks_in_year  (GDateYear    year);
guint8       g_date_sunday_weeks_in_year  (GDateYear    year);

/* qsort-friendly (with a cast...) */
gint         g_date_compare               (GDate       *lhs,
                                           GDate       *rhs);
void         g_date_to_struct_tm          (GDate       *date,
                                           struct tm   *tm);

/* Just like strftime() except you can only use date-related formats.
 *   Using a time format is undefined.
 */
gsize        g_date_strftime              (gchar       *s,
                                           gsize        slen,
                                           const gchar *format,
                                           GDate       *date);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __GLIB_CSP_H__
