/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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
 * @file ns-simdata
 */


#ifndef __NS_SIMDATA_H__
#define __NS_SIMDATA_H__

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
  #define NAMESPACE_END }
  #define SIMDATA(x) simdata::x
#else
  #define USING_SIMDATA
  #define NAMESPACE_SIMDATA
  #define NAMESPACE_END
  #define SIMDATA(x) x
#endif

#endif //__NS_SIMDATA_H__


