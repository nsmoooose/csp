/* Combat Simulator Project
 * Copyright (C) 2002, 2003 Mark Rose <mkrose@users.sf.net>
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
 * @file Uniform.h
 * @brief Provides fixed size integer typedefs and endian tests.
 */

#ifndef __CSPLIB_UTIL_UNIFORM_H__
#define __CSPLIB_UTIL_UNIFORM_H__

// generated using Source/Config.cpp during the build.
#include <csp/csplib/util/Config.h>
#include <csp/csplib/util/Namespace.h>

#include <cstring>  // for memcpy

CSP_NAMESPACE


//@{
/** Size specific integer types.
 *
 *  Note that these are currently hard-coded, assuming
 *  that almost all modern compilers and architectures
 *  will use these quasi-standard sizes.  If this is
 *  not true, these typedefs make it easy to adjust
 *  the mapping by hand (or by autoconf).
 *
 *  @name Integer Types
 */
typedef signed char int8;
typedef unsigned char uint8;

#ifdef CSP_I16
typedef signed CSP_I16 int16;
typedef unsigned CSP_I16 uint16;
#else
typedef signed short int16;
typedef unsigned short uint16;
#endif

#ifdef CSP_I32
typedef signed CSP_I32 int32;
typedef unsigned CSP_I32 uint32;
#else
typedef signed int int32;
typedef unsigned int uint32;
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1310)
typedef __int64 int64;
typedef unsigned __int64 uint64;
# define CSP_ULL(x) x##ull
# define CSP_LL(x) x##ll
#else
// use __extension__ to avoid G++ errors with -pedantic
#ifndef SWIG
__extension__
#endif // SWIG
typedef long long int64;
#ifndef SWIG
__extension__
#endif // SWIG
typedef unsigned long long uint64;
# define CSP_ULL(x) x##ULL
# define CSP_LL(x) x##LL
#endif

#if defined(__GNUC__)
# define CSP_STATIC_CONST_DEF(x) const x
# define CSP_PACKED __attribute__ ((packed))
# define CSP_UNUSED __attribute__ ((__unused__))
#else
# define CSP_STATIC_CONST_DEF(x)
# define CSP_PACKED
# define CSP_UNUSED
#endif

/* Borrow a minimal version of Boost's STATIC_ASSERT mechanism.
 */
template <bool> struct static_assert_failure;
template <> struct static_assert_failure<true> {};
template <int> struct static_assert_test {};
#define CSP_STATIC_ASSERT(expr) \
	typedef static_assert_test<sizeof(static_assert_failure<(bool)(expr)>)> static_assertion_##__LINE__

/* Portable cast operator for type-punning.  Optimizes extremely well
 * under gcc; haven't checked msvc assembly though.
 */
template <typename T1, typename T2>
inline T1 alias_cast(T2 src) {
	CSP_STATIC_ASSERT(sizeof(T1) == sizeof(T2));
	T1 dst;
	std::memcpy(&dst, &src, sizeof(dst));
	return dst;
}

//@}

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_UNIFORM_H__

