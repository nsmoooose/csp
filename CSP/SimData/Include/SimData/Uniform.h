/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
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
 * @file Uniform.h
 * @brief Provides fixed size integer typedefs and endian tests.
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

#ifndef __SIMDATA_UNIFORM_H__
#define __SIMDATA_UNIFORM_H__

// generated using Source/Config.cpp during the build.
#include <SimData/Config.h>

#include <SimData/Namespace.h>

NAMESPACE_SIMDATA;


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

#ifdef SIMDATA_I16
typedef signed SIMDATA_I16 int16;
typedef unsigned SIMDATA_I16 uint16;
#else
typedef signed short int16;
typedef unsigned short uint16;
#endif

#ifdef SIMDATA_I32
typedef signed SIMDATA_I32 int32;
typedef unsigned SIMDATA_I32 uint32;
#else
typedef signed int int32;
typedef unsigned int uint32;
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1310)
# define SIMDATA_STATIC_CONST_DEF(x)
# define SIMDATA_PACKED
typedef __int64 int64;
typedef unsigned __int64 uint64;
# define SIMDATA_ULL(x) x##ull
# define SIMDATA_LL(x) x##ll
#else
# define SIMDATA_STATIC_CONST_DEF(x) const x
# define SIMDATA_PACKED __attribute__((packed))
// use __extension__ to avoid G++ errors with -pedantic
#ifndef SWIG
__extension__
#endif // SWIG
typedef long long int64;
#ifndef SWIG
__extension__
#endif // SWIG
typedef unsigned long long uint64;
# define SIMDATA_ULL(x) x##ULL
# define SIMDATA_LL(x) x##LL
#endif

//@}

NAMESPACE_SIMDATA_END

#endif // __SIMDATA_UNIFORM_H__

