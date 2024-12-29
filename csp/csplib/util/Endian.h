#pragma once
/* Combat Simulator Project
 * Copyright (C) 2004 Mark Rose <mkrose@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/**
 * @file Endian.h
 * @brief Machine/compiler specific endian conversions.
 */


/*
 * The bulk of this code is a minimal translation of gtypes.h in GLIB.
 * The GLIB code used here is distributed under the terms of the GNU
 * General Public License, as allowed under section 2 of the GNU Lesser
 * General Public License.  The original copyright notices appear
 * below:
 *
 * GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#include <csp/csplib/util/Uniform.h>


/* Basic bit swapping functions
 */
#define CSP_UINT16_SWAP_LE_BE_CONSTANT(val) (static_cast<uint16_t>( \
	static_cast<uint16_t>(static_cast<uint16_t>(val) >> 8) | \
	static_cast<uint16_t>(static_cast<uint16_t>(val) << 8)))

#define CSP_UINT32_SWAP_LE_BE_CONSTANT(val) (static_cast<uint32_t>( \
	(static_cast<uint32_t>(val) & static_cast<uint32_t>(0x000000ffU) << 24) | \
	(static_cast<uint32_t>(val) & static_cast<uint32_t>(0x0000ff00U) <<  8) | \
	(static_cast<uint32_t>(val) & static_cast<uint32_t>(0x00ff0000U) >>  8) | \
	(static_cast<uint32_t>(val) & static_cast<uint32_t>(0xff000000U) >> 24)))

#define CSP_UINT64_SWAP_LE_BE_CONSTANT(val) (static_cast<uint64_t>( \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x00000000000000ffULL) << 56) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x000000000000ff00ULL) << 40) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x0000000000ff0000ULL) << 24) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x00000000ff000000ULL) <<  8) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x000000ff00000000ULL) >>  8) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x0000ff0000000000ULL) >> 24) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0x00ff000000000000ULL)) >> 40) | \
	(static_cast<uint64_t>(val) & \
		static_cast<uint64_t>(0xff00000000000000ULL) >> 56)))

/* Arch specific stuff for speed
 */
#if defined (__GNUC__) && (__GNUC__ >= 2) && defined (__OPTIMIZE__)
#  if defined (__i386__)
#    define CSP_UINT16_SWAP_LE_BE_IA32(val) \
       (__extension__ \
        ({ register uint16_t __v, __x = (static_cast<uint16_t> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = CSP_UINT16_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ ("rorw $8, %w0" \
                      : "=r" (__v) \
                      : "0" (__x) \
                      : "cc"); \
            __v; }))
#    if !defined (__i486__) && !defined (__i586__) \
        && !defined (__pentium__) && !defined (__i686__) \
        && !defined (__pentiumpro__) && !defined (__pentium4__)
#       define CSP_UINT32_SWAP_LE_BE_IA32(val) \
          (__extension__ \
           ({ register uint32_t __v, __x = (static_cast<uint32_t> (val)); \
              if (__builtin_constant_p (__x)) \
                __v = CSP_UINT32_SWAP_LE_BE_CONSTANT (__x); \
              else \
                __asm__ ("rorw $8, %w0\n\t" \
                         "rorl $16, %0\n\t" \
                         "rorw $8, %w0" \
                         : "=r" (__v) \
                         : "0" (__x) \
                         : "cc"); \
              __v; }))
#    else /* 486 and higher has bswap */
#       define CSP_UINT32_SWAP_LE_BE_IA32(val) \
          (__extension__ \
           ({ register uint32_t __v, __x = (static_cast<uint32_t> (val)); \
              if (__builtin_constant_p (__x)) \
                __v = CSP_UINT32_SWAP_LE_BE_CONSTANT (__x); \
              else \
                __asm__ ("bswap %0" \
                         : "=r" (__v) \
                         : "0" (__x)); \
              __v; }))
#    endif /* processor specific 32-bit stuff */
#    define CSP_UINT64_SWAP_LE_BE_IA32(val) \
       (__extension__ \
        ({ union { uint64_t __ll; \
                   uint32_t __l[2]; } __w, __r; \
           __w.__ll = (static_cast<uint64_t> (val)); \
           if (__builtin_constant_p (__w.__ll)) \
             __r.__ll = CSP_UINT64_SWAP_LE_BE_CONSTANT (__w.__ll); \
           else \
             { \
               __r.__l[0] = CSP_UINT32_SWAP_LE_BE (__w.__l[1]); \
               __r.__l[1] = CSP_UINT32_SWAP_LE_BE (__w.__l[0]); \
             } \
           __r.__ll; }))
     /* Possibly just use the constant version and let gcc figure it out? */
#    define CSP_UINT16_SWAP_LE_BE(val) (CSP_UINT16_SWAP_LE_BE_IA32 (val))
#    define CSP_UINT32_SWAP_LE_BE(val) (CSP_UINT32_SWAP_LE_BE_IA32 (val))
#    define CSP_UINT64_SWAP_LE_BE(val) (CSP_UINT64_SWAP_LE_BE_IA32 (val))
#  elif defined (__ia64__)
#    define CSP_UINT16_SWAP_LE_BE_IA64(val) \
       (__extension__ \
        ({ register uint16_t __v, __x = (static_cast<uint16_t> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = CSP_UINT16_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ __volatile__ ("shl %0 = %1, 48 ;;" \
                                   "mux1 %0 = %0, @rev ;;" \
                                    : "=r" (__v) \
                                    : "r" (__x)); \
            __v; }))
#    define CSP_UINT32_SWAP_LE_BE_IA64(val) \
       (__extension__ \
         ({ register uint32_t __v, __x = (static_cast<uint32_t> (val)); \
            if (__builtin_constant_p (__x)) \
              __v = CSP_UINT32_SWAP_LE_BE_CONSTANT (__x); \
            else \
             __asm__ __volatile__ ("shl %0 = %1, 32 ;;" \
                                   "mux1 %0 = %0, @rev ;;" \
                                    : "=r" (__v) \
                                    : "r" (__x)); \
            __v; }))
#    define CSP_UINT64_SWAP_LE_BE_IA64(val) \
       (__extension__ \
        ({ register uint64_t __v, __x = (static_cast<uint64_t> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = CSP_UINT64_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ __volatile__ ("mux1 %0 = %1, @rev ;;" \
                                   : "=r" (__v) \
                                   : "r" (__x)); \
           __v; }))
#    define CSP_UINT16_SWAP_LE_BE(val) (CSP_UINT16_SWAP_LE_BE_IA64 (val))
#    define CSP_UINT32_SWAP_LE_BE(val) (CSP_UINT32_SWAP_LE_BE_IA64 (val))
#    define CSP_UINT64_SWAP_LE_BE(val) (CSP_UINT64_SWAP_LE_BE_IA64 (val))
#  elif defined (__x86_64__)
#    define CSP_UINT32_SWAP_LE_BE_X86_64(val) \
       (__extension__ \
         ({ register uint32_t __v, __x = (static_cast<uint32_t> (val)); \
            if (__builtin_constant_p (__x)) \
              __v = CSP_UINT32_SWAP_LE_BE_CONSTANT (__x); \
            else \
             __asm__ ("bswapl %0" \
                      : "=r" (__v) \
                      : "0" (__x)); \
            __v; }))
#    define CSP_UINT64_SWAP_LE_BE_X86_64(val) \
       (__extension__ \
        ({ register uint64_t __v, __x = (static_cast<uint64_t> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = CSP_UINT64_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ ("bswapq %0" \
                      : "=r" (__v) \
                      : "0" (__x)); \
           __v; }))
     /* gcc seems to figure out optimal code for this on its own */
#    define CSP_UINT16_SWAP_LE_BE(val) (CSP_UINT16_SWAP_LE_BE_CONSTANT (val))
#    define CSP_UINT32_SWAP_LE_BE(val) (CSP_UINT32_SWAP_LE_BE_X86_64 (val))
#    define CSP_UINT64_SWAP_LE_BE(val) (CSP_UINT64_SWAP_LE_BE_X86_64 (val))
#  else /* generic gcc */
#    define CSP_UINT16_SWAP_LE_BE(val) (CSP_UINT16_SWAP_LE_BE_CONSTANT (val))
#    define CSP_UINT32_SWAP_LE_BE(val) (CSP_UINT32_SWAP_LE_BE_CONSTANT (val))
#    define CSP_UINT64_SWAP_LE_BE(val) (CSP_UINT64_SWAP_LE_BE_CONSTANT (val))
#  endif
#else /* generic */
#  define CSP_UINT16_SWAP_LE_BE(val) (CSP_UINT16_SWAP_LE_BE_CONSTANT (val))
#  define CSP_UINT32_SWAP_LE_BE(val) (CSP_UINT32_SWAP_LE_BE_CONSTANT (val))
#  define CSP_UINT64_SWAP_LE_BE(val) (CSP_UINT64_SWAP_LE_BE_CONSTANT (val))
#endif /* generic */


#if (CSP_BYTE_ORDER == CSP_LE)
#  define CSP_INT16_TO_LE(val) (static_cast<int16_t> (val))
#  define CSP_UINT16_TO_LE(val) (static_cast<uint16_t> (val))
#  define CSP_INT16_TO_BE(val) (static_cast<int16_t>(CSP_UINT16_SWAP_LE_BE (val)))
#  define CSP_UINT16_TO_BE(val) (static_cast<uint16_t>(CSP_UINT16_SWAP_LE_BE (val)))
#  define CSP_INT32_TO_LE(val) (static_cast<int32_t> (val))
#  define CSP_UINT32_TO_LE(val) (static_cast<uint32_t> (val))
#  define CSP_INT32_TO_BE(val) (static_cast<int32_t>(CSP_UINT32_SWAP_LE_BE (val)))
#  define CSP_UINT32_TO_BE(val) (static_cast<uint32_t>(CSP_UINT32_SWAP_LE_BE (val)))
#  define CSP_INT64_TO_LE(val) (static_cast<int64_t> (val))
#  define CSP_UINT64_TO_LE(val) (static_cast<uint64_t> (val))
#  define CSP_INT64_TO_BE(val) (static_cast<int64_t>(CSP_UINT64_SWAP_LE_BE (val)))
#  define CSP_UINT64_TO_BE(val) (static_cast<uint64_t>(CSP_UINT64_SWAP_LE_BE (val)))
#else
#  define CSP_INT16_TO_LE(val) (static_cast<int16_t>(CSP_UINT16_SWAP_LE_BE (val)))
#  define CSP_UINT16_TO_LE(val) (static_cast<uint16_t>(CSP_UINT16_SWAP_LE_BE (val)))
#  define CSP_INT16_TO_BE(val) (static_cast<int16_t> (val))
#  define CSP_UINT16_TO_BE(val) (static_cast<uint16_t> (val))
#  define CSP_INT32_TO_LE(val) (static_cast<int32_t>(CSP_UINT32_SWAP_LE_BE (val)))
#  define CSP_UINT32_TO_LE(val) (static_cast<uint32_t>(CSP_UINT32_SWAP_LE_BE (val)))
#  define CSP_INT32_TO_BE(val) (static_cast<int32_t> (val))
#  define CSP_UINT32_TO_BE(val) (static_cast<uint32_t> (val))
#  define CSP_INT64_TO_LE(val) (static_cast<int64_t>(CSP_UINT64_SWAP_LE_BE (val)))
#  define CSP_UINT64_TO_LE(val) (static_cast<uint64_t>(CSP_UINT64_SWAP_LE_BE (val)))
#  define CSP_INT64_TO_BE(val) (static_cast<int64_t> (val))
#  define CSP_UINT64_TO_BE(val) (static_cast<uint64_t> (val))
#endif

/* The transformation is symmetric, so the FROM just maps to the TO.
 */
#define CSP_INT16_FROM_LE(val) (CSP_INT16_TO_LE (val))
#define CSP_UINT16_FROM_LE(val) (CSP_UINT16_TO_LE (val))
#define CSP_INT16_FROM_BE(val) (CSP_INT16_TO_BE (val))
#define CSP_UINT16_FROM_BE(val) (CSP_UINT16_TO_BE (val))

#define CSP_INT32_FROM_LE(val) (CSP_INT32_TO_LE (val))
#define CSP_UINT32_FROM_LE(val) (CSP_UINT32_TO_LE (val))
#define CSP_INT32_FROM_BE(val) (CSP_INT32_TO_BE (val))
#define CSP_UINT32_FROM_BE(val) (CSP_UINT32_TO_BE (val))

#define CSP_INT64_FROM_LE(val) (CSP_INT64_TO_LE (val))
#define CSP_UINT64_FROM_LE(val) (CSP_UINT64_TO_LE (val))
#define CSP_INT64_FROM_BE(val) (CSP_INT64_TO_BE (val))
#define CSP_UINT64_FROM_BE(val) (CSP_UINT64_TO_BE (val))

/* Portable versions of host-network order stuff
 */
#define CSP_NTOHL(val) (CSP_UINT32_FROM_BE (val))
#define CSP_NTOHS(val) (CSP_UINT16_FROM_BE (val))
#define CSP_HTONL(val) (CSP_UINT32_TO_BE (val))
#define CSP_HTONS(val) (CSP_UINT16_TO_BE (val))


namespace csp {

/** Template wrappers for endian macros.
 */
template<typename T> struct endian { };
template<> struct endian<uint8_t> {
	inline static uint8_t to_le(uint8_t x) { return x; }
	inline static uint8_t from_le(uint8_t x) { return x; }
};
template<> struct endian<int8_t> {
	inline static int8_t to_le(int8_t x) { return x; }
	inline static int8_t from_le(int8_t x) { return x; }
};
template<> struct endian<char> {
	inline static char to_le(char x) { return x; }
	inline static char from_le(char x) { return x; }
};
#define CSP_ENDIAN_TEMPLATE(TYPE, CAPTYPE) \
	template<> struct endian<TYPE> { \
		inline static TYPE to_le(TYPE x) { return static_cast<TYPE>(CSP_##CAPTYPE##_TO_LE(x)); } \
		inline static TYPE from_le(TYPE x) { return static_cast<TYPE>(CSP_##CAPTYPE##_FROM_LE(x)); } \
	};
CSP_ENDIAN_TEMPLATE(int16_t, INT16)
CSP_ENDIAN_TEMPLATE(int32_t, INT32)
CSP_ENDIAN_TEMPLATE(int64_t, INT64)
CSP_ENDIAN_TEMPLATE(uint16_t, UINT16)
CSP_ENDIAN_TEMPLATE(uint32_t, UINT32)
CSP_ENDIAN_TEMPLATE(uint64_t, UINT64)
CSP_ENDIAN_TEMPLATE(float, UINT32)
CSP_ENDIAN_TEMPLATE(double, UINT64)
#undef CSP_ENDIAN_TEMPLATE

} // namespace csp
