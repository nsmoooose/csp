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
 * @author Mark Rose <mrose@stm.lbl.gov>
 */

#ifndef __SIMDATA_UNIFORM_H__
#define __SIMDATA_UNIFORM_H__

#include <SimData/Namespace.h>

NAMESPACE_SIMDATA


/** Size specific integer types.
 *
 *  Note that these are currently hard-coded, assuming
 *  that almost all modern compilers and architectures 
 *  will use these quasi-standard sizes.  If this is
 *  not true, these typedefs make it easy to adjust
 *  the mapping by hand (or by autoconf).  
 *
 *  @note SimData currently uses internal structures to 
 *  represent 64-bit integer types, but these may be 
 *  optimized in the future to use long ints on 64-bit 
 *  hardware.
 */
//@{
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
//@}

/** Test for big-endian byte order */
inline bool isBigEndian() { 
	static uint16 test=0x1234; 
	return (*reinterpret_cast<uint8*>(&test)) == 0x12; 
}

/** Test for little-endian byte order */
inline bool isLittleEndian() { 
	return !isBigEndian(); 
}


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_UNIFORM_H__

