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
 * @file Conversions.h
 * 
 * Common unit conversion templates.
 * 
 */

#ifndef __CONVERSIONS_H__
#define __CONVERSIONS_H__


// SWIG template instantiations
#ifdef SWIG
	#define SIMDATA_CVT(a) %template(convert_##a) a<double>;
#else
	#define SIMDATA_CVT(a) 
#endif


#include <SimData/ns-simdata.h>

NAMESPACE_SIMDATA
namespace convert {

// This is currently just a small set of unit conversions.  Feel free to add
// additional routines.  Try not to include simple "prefix" conversions such 
// as meters to centimeters.  Use the dominant (i.e. most common) base unit 
// in each system of units if possible.
// 
// NOTE: be sure to add corresponding template instantiations for SWIG
	
	
/**
 * knots to meters per second
 */
template <typename T>
T kts_mps(T x) { return x * 0.514444; }
SIMDATA_CVT(kts_mps)


/**
 * meters per second to knots
 */
template <typename T>
T mps_kts(T x) { return x * 1.94385; }
SIMDATA_CVT(mps_kts)

/**
 * feet to meters
 */
template <typename T>
T ft_m(T x) { return x * 0.3048; }
SIMDATA_CVT(ft_m)

/**
 * meters to feet
 */
template <typename T>
T m_ft(T x) { return x * 3.2808398950; }
SIMDATA_CVT(m_ft)

/**
 * kilograms to pounds
 */
template <typename T>
T kg_lb(T x) { return x * 2.2046; }
SIMDATA_CVT(kg_lb)

/**
 * pounds to kilograms
 */
template <typename T>
T lb_kg(T x) { return x * 0.45360; }
SIMDATA_CVT(lb_kg)

/**
 * nautical miles to meters
 */
template <typename T>
T nm_m(T x) { return x * 1852.0; }
SIMDATA_CVT(nm_m)

/**
 * meters to nautical miles
 */
template <typename T>
T m_nm(T x) { return x * 0.0005399568; }
SIMDATA_CVT(m_nm)

/**
 * pascals to millimeters of mercury
 */
template <typename T>
T pa_mmhg(T x) { return x * 0.00752; }
SIMDATA_CVT(pa_mmhg)

/**
 * millimeters of mercury to pascals
 */
template <typename T>
T mmhg_pa(T x) { return x * 133.0; }
SIMDATA_CVT(mmhg_pa)


} // namespace convert
NAMESPACE_END


#endif // __CONVERSIONS_H__

