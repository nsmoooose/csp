/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2004 Mark Rose <tm2@stm.lbl.gov>
 *
 * This file is part of SimData.
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

#ifndef __SIMDATA_ENDIAN_H__
#define __SIMDATA_ENDIAN_H__

#include <SimData/Uniform.h>


/* Basic bit swapping functions
 */
#define SIMDATA_UINT16_SWAP_LE_BE_CONSTANT(val) (static_cast<SIMDATA(uint16)> ( \
	static_cast<SIMDATA(uint16)> (static_cast<SIMDATA(uint16)> (val) >> 8) | \
	static_cast<SIMDATA(uint16)> (static_cast<SIMDATA(uint16)> (val) << 8)))

#define SIMDATA_UINT32_SWAP_LE_BE_CONSTANT(val) (static_cast<SIMDATA(uint32)> ( \
	((static_cast<SIMDATA(uint32)> (val) & static_cast<SIMDATA(uint32)> 0x000000ffU) << 24) | \
	((static_cast<SIMDATA(uint32)> (val) & static_cast<SIMDATA(uint32)> 0x0000ff00U) <<  8) | \
	((static_cast<SIMDATA(uint32)> (val) & static_cast<SIMDATA(uint32)> 0x00ff0000U) >>  8) | \
	((static_cast<SIMDATA(uint32)> (val) & static_cast<SIMDATA(uint32)> 0xff000000U) >> 24)))

#define SIMDATA_UINT64_SWAP_LE_BE_CONSTANT(val) (static_cast<SIMDATA(uint64)> ( \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x00000000000000ffU)) << 56) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x000000000000ff00U)) << 40) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x0000000000ff0000U)) << 24) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x00000000ff000000U)) <<  8) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x000000ff00000000U)) >>  8) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x0000ff0000000000U)) >> 24) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0x00ff000000000000U)) >> 40) | \
	((static_cast<SIMDATA(uint64)> (val) & \
		static_cast<SIMDATA(uint64)> SIMDATA_ULL (0xff00000000000000U)) >> 56)))

/* Arch specific stuff for speed
 */
#if defined (__GNUC__) && (__GNUC__ >= 2) && defined (__OPTIMIZE__)
#  if defined (__i386__)
#    define SIMDATA_UINT16_SWAP_LE_BE_IA32(val) \
       (__extension__ \
        ({ register SIMDATA(uint16) __v, __x = (static_cast<SIMDATA(uint16)> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = SIMDATA_UINT16_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ ("rorw $8, %w0" \
                      : "=r" (__v) \
                      : "0" (__x) \
                      : "cc"); \
            __v; }))
#    if !defined (__i486__) && !defined (__i586__) \
        && !defined (__pentium__) && !defined (__i686__) \
        && !defined (__pentiumpro__) && !defined (__pentium4__)
#       define SIMDATA_UINT32_SWAP_LE_BE_IA32(val) \
          (__extension__ \
           ({ register SIMDATA(uint32) __v, __x = (static_cast<SIMDATA(uint32)> (val)); \
              if (__builtin_constant_p (__x)) \
                __v = SIMDATA_UINT32_SWAP_LE_BE_CONSTANT (__x); \
              else \
                __asm__ ("rorw $8, %w0\n\t" \
                         "rorl $16, %0\n\t" \
                         "rorw $8, %w0" \
                         : "=r" (__v) \
                         : "0" (__x) \
                         : "cc"); \
              __v; }))
#    else /* 486 and higher has bswap */
#       define SIMDATA_UINT32_SWAP_LE_BE_IA32(val) \
          (__extension__ \
           ({ register SIMDATA(uint32) __v, __x = (static_cast<SIMDATA(uint32)> (val)); \
              if (__builtin_constant_p (__x)) \
                __v = SIMDATA_UINT32_SWAP_LE_BE_CONSTANT (__x); \
              else \
                __asm__ ("bswap %0" \
                         : "=r" (__v) \
                         : "0" (__x)); \
              __v; }))
#    endif /* processor specific 32-bit stuff */
#    define SIMDATA_UINT64_SWAP_LE_BE_IA32(val) \
       (__extension__ \
        ({ union { SIMDATA(uint64) __ll; \
                   SIMDATA(uint32) __l[2]; } __w, __r; \
           __w.__ll = (static_cast<SIMDATA(uint64)> (val)); \
           if (__builtin_constant_p (__w.__ll)) \
             __r.__ll = SIMDATA_UINT64_SWAP_LE_BE_CONSTANT (__w.__ll); \
           else \
             { \
               __r.__l[0] = SIMDATA_UINT32_SWAP_LE_BE (__w.__l[1]); \
               __r.__l[1] = SIMDATA_UINT32_SWAP_LE_BE (__w.__l[0]); \
             } \
           __r.__ll; }))
     /* Possibly just use the constant version and let gcc figure it out? */
#    define SIMDATA_UINT16_SWAP_LE_BE(val) (SIMDATA_UINT16_SWAP_LE_BE_IA32 (val))
#    define SIMDATA_UINT32_SWAP_LE_BE(val) (SIMDATA_UINT32_SWAP_LE_BE_IA32 (val))
#    define SIMDATA_UINT64_SWAP_LE_BE(val) (SIMDATA_UINT64_SWAP_LE_BE_IA32 (val))
#  elif defined (__ia64__)
#    define SIMDATA_UINT16_SWAP_LE_BE_IA64(val) \
       (__extension__ \
        ({ register SIMDATA(uint16) __v, __x = (static_cast<SIMDATA(uint16)> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = SIMDATA_UINT16_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ __volatile__ ("shl %0 = %1, 48 ;;" \
                                   "mux1 %0 = %0, @rev ;;" \
                                    : "=r" (__v) \
                                    : "r" (__x)); \
            __v; }))
#    define SIMDATA_UINT32_SWAP_LE_BE_IA64(val) \
       (__extension__ \
         ({ register SIMDATA(uint32) __v, __x = (static_cast<SIMDATA(uint32)> (val)); \
            if (__builtin_constant_p (__x)) \
              __v = SIMDATA_UINT32_SWAP_LE_BE_CONSTANT (__x); \
            else \
             __asm__ __volatile__ ("shl %0 = %1, 32 ;;" \
                                   "mux1 %0 = %0, @rev ;;" \
                                    : "=r" (__v) \
                                    : "r" (__x)); \
            __v; }))
#    define SIMDATA_UINT64_SWAP_LE_BE_IA64(val) \
       (__extension__ \
        ({ register SIMDATA(uint64) __v, __x = (static_cast<SIMDATA(uint64)> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = SIMDATA_UINT64_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ __volatile__ ("mux1 %0 = %1, @rev ;;" \
                                   : "=r" (__v) \
                                   : "r" (__x)); \
           __v; }))
#    define SIMDATA_UINT16_SWAP_LE_BE(val) (SIMDATA_UINT16_SWAP_LE_BE_IA64 (val))
#    define SIMDATA_UINT32_SWAP_LE_BE(val) (SIMDATA_UINT32_SWAP_LE_BE_IA64 (val))
#    define SIMDATA_UINT64_SWAP_LE_BE(val) (SIMDATA_UINT64_SWAP_LE_BE_IA64 (val))
#  elif defined (__x86_64__)
#    define SIMDATA_UINT32_SWAP_LE_BE_X86_64(val) \
       (__extension__ \
         ({ register SIMDATA(uint32) __v, __x = (static_cast<SIMDATA(uint32)> (val)); \
            if (__builtin_constant_p (__x)) \
              __v = SIMDATA_UINT32_SWAP_LE_BE_CONSTANT (__x); \
            else \
             __asm__ ("bswapl %0" \
                      : "=r" (__v) \
                      : "0" (__x)); \
            __v; }))
#    define SIMDATA_UINT64_SWAP_LE_BE_X86_64(val) \
       (__extension__ \
        ({ register SIMDATA(uint64) __v, __x = (static_cast<SIMDATA(uint64)> (val)); \
           if (__builtin_constant_p (__x)) \
             __v = SIMDATA_UINT64_SWAP_LE_BE_CONSTANT (__x); \
           else \
             __asm__ ("bswapq %0" \
                      : "=r" (__v) \
                      : "0" (__x)); \
           __v; }))
     /* gcc seems to figure out optimal code for this on its own */
#    define SIMDATA_UINT16_SWAP_LE_BE(val) (SIMDATA_UINT16_SWAP_LE_BE_CONSTANT (val))
#    define SIMDATA_UINT32_SWAP_LE_BE(val) (SIMDATA_UINT32_SWAP_LE_BE_X86_64 (val))
#    define SIMDATA_UINT64_SWAP_LE_BE(val) (SIMDATA_UINT64_SWAP_LE_BE_X86_64 (val))
#  else /* generic gcc */
#    define SIMDATA_UINT16_SWAP_LE_BE(val) (SIMDATA_UINT16_SWAP_LE_BE_CONSTANT (val))
#    define SIMDATA_UINT32_SWAP_LE_BE(val) (SIMDATA_UINT32_SWAP_LE_BE_CONSTANT (val))
#    define SIMDATA_UINT64_SWAP_LE_BE(val) (SIMDATA_UINT64_SWAP_LE_BE_CONSTANT (val))
#  endif
#else /* generic */
#  define SIMDATA_UINT16_SWAP_LE_BE(val) (SIMDATA_UINT16_SWAP_LE_BE_CONSTANT (val))
#  define SIMDATA_UINT32_SWAP_LE_BE(val) (SIMDATA_UINT32_SWAP_LE_BE_CONSTANT (val))
#  define SIMDATA_UINT64_SWAP_LE_BE(val) (SIMDATA_UINT64_SWAP_LE_BE_CONSTANT (val))
#endif /* generic */


#if (SIMDATA_BYTE_ORDER == SIMDATA_LE)
#  define SIMDATA_INT16_TO_LE(val) (static_cast<SIMDATA(int16)> (val))
#  define SIMDATA_UINT16_TO_LE(val) (static_cast<SIMDATA(uint16)> (val))
#  define SIMDATA_INT16_TO_BE(val) (static_cast<SIMDATA(int16)>(SIMDATA_UINT16_SWAP_LE_BE (val)))
#  define SIMDATA_UINT16_TO_BE(val) (static_cast<SIMDATA(uint16)>(SIMDATA_UINT16_SWAP_LE_BE (val)))
#  define SIMDATA_INT32_TO_LE(val) (static_cast<SIMDATA(int32)> (val))
#  define SIMDATA_UINT32_TO_LE(val) (static_cast<SIMDATA(uint32)> (val))
#  define SIMDATA_INT32_TO_BE(val) (static_cast<SIMDATA(int32)>(SIMDATA_UINT32_SWAP_LE_BE (val)))
#  define SIMDATA_UINT32_TO_BE(val) (static_cast<SIMDATA(uint32)>(SIMDATA_UINT32_SWAP_LE_BE (val)))
#  define SIMDATA_INT64_TO_LE(val) (static_cast<SIMDATA(int64)> (val))
#  define SIMDATA_UINT64_TO_LE(val) (static_cast<SIMDATA(uint64)> (val))
#  define SIMDATA_INT64_TO_BE(val) (static_cast<SIMDATA(int64)>(SIMDATA_UINT64_SWAP_LE_BE (val)))
#  define SIMDATA_UINT64_TO_BE(val) (static_cast<SIMDATA(uint64)>(SIMDATA_UINT64_SWAP_LE_BE (val)))
#else
#  define SIMDATA_INT16_TO_LE(val) (static_cast<SIMDATA(int16)>(SIMDATA_UINT16_SWAP_LE_BE (val)))
#  define SIMDATA_UINT16_TO_LE(val) (static_cast<SIMDATA(uint16)>(SIMDATA_UINT16_SWAP_LE_BE (val)))
#  define SIMDATA_INT16_TO_BE(val) (static_cast<SIMDATA(int16)> (val))
#  define SIMDATA_UINT16_TO_BE(val) (static_cast<SIMDATA(uint16)> (val))
#  define SIMDATA_INT32_TO_LE(val) (static_cast<SIMDATA(int32)>(SIMDATA_UINT32_SWAP_LE_BE (val)))
#  define SIMDATA_UINT32_TO_LE(val) (static_cast<SIMDATA(uint32)>(SIMDATA_UINT32_SWAP_LE_BE (val)))
#  define SIMDATA_INT32_TO_BE(val) (static_cast<SIMDATA(int32)> (val))
#  define SIMDATA_UINT32_TO_BE(val) (static_cast<SIMDATA(uint32)> (val))
#  define SIMDATA_INT64_TO_LE(val) (static_cast<SIMDATA(int64)>(SIMDATA_UINT64_SWAP_LE_BE (val)))
#  define SIMDATA_UINT64_TO_LE(val) (static_cast<SIMDATA(uint64)>(SIMDATA_UINT64_SWAP_LE_BE (val)))
#  define SIMDATA_INT64_TO_BE(val) (static_cast<SIMDATA(int64)> (val))
#  define SIMDATA_UINT64_TO_BE(val) (static_cast<SIMDATA(uint64)> (val))
#endif

/* The transformation is symmetric, so the FROM just maps to the TO.
 */
#define SIMDATA_INT16_FROM_LE(val) (SIMDATA_INT16_TO_LE (val))
#define SIMDATA_UINT16_FROM_LE(val) (SIMDATA_UINT16_TO_LE (val))
#define SIMDATA_INT16_FROM_BE(val) (SIMDATA_INT16_TO_BE (val))
#define SIMDATA_UINT16_FROM_BE(val) (SIMDATA_UINT16_TO_BE (val))

#define SIMDATA_INT32_FROM_LE(val) (SIMDATA_INT32_TO_LE (val))
#define SIMDATA_UINT32_FROM_LE(val) (SIMDATA_UINT32_TO_LE (val))
#define SIMDATA_INT32_FROM_BE(val) (SIMDATA_INT32_TO_BE (val))
#define SIMDATA_UINT32_FROM_BE(val) (SIMDATA_UINT32_TO_BE (val))

#define SIMDATA_INT64_FROM_LE(val) (SIMDATA_INT64_TO_LE (val))
#define SIMDATA_UINT64_FROM_LE(val) (SIMDATA_UINT64_TO_LE (val))
#define SIMDATA_INT64_FROM_BE(val) (SIMDATA_INT64_TO_BE (val))
#define SIMDATA_UINT64_FROM_BE(val) (SIMDATA_UINT64_TO_BE (val))

/* Portable versions of host-network order stuff
 */
#define SIMDATA_NTOHL(val) (SIMDATA_UINT32_FROM_BE (val))
#define SIMDATA_NTOHS(val) (SIMDATA_UINT16_FROM_BE (val))
#define SIMDATA_HTONL(val) (SIMDATA_UINT32_TO_BE (val))
#define SIMDATA_HTONS(val) (SIMDATA_UINT16_TO_BE (val))


#endif /* __SIMDATA_ENDIAN_H__ */

