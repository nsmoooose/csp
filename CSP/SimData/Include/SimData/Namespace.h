/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Namespace.h
 * @brief Macros for defining and accessing the simdata namespace.
 */

/**
 * @namespace simdata
 *
 * @brief The primary namespace for all classes in SimData.
 */

#ifndef __SIMDATA_NAMESPACE_H__
#define __SIMDATA_NAMESPACE_H__

// turn on the simdata namespace by default
#define USE_NAMESPACE_SIMDATA

#ifdef NO_NAMESPACE_SIMDATA
#undef USE_NAMESPACE_SIMDATA
#endif // USE_NAMESPACE_SIMDATA

// simdata namespace macros, to easily turn namespace support
// on or off.

#ifdef USE_NAMESPACE_SIMDATA
  #define USING_SIMDATA using namespace simdata;
  #define NAMESPACE_SIMDATA namespace simdata {
  #define NAMESPACE_SIMDATA_END }
  #define SIMDATA(x) simdata::x
#else
  #define USING_SIMDATA
  #define NAMESPACE_SIMDATA
  #define NAMESPACE_SIMDATA_END
  #define SIMDATA(x) x
#endif

#endif //__SIMDATA_NAMESPACE_H__


