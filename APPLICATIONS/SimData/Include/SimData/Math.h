/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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

/*
 * @file Math.h
 *
 * Numerous utility functions implemented here are based on 
 * the Math header included in the OpenSceneGraph library, 
 * Copyright 1998-2003 Robert Osfield.  Source code from 
 * OpenSceneGraph is used here under the GNU General Public 
 * License Version 2 or later, as permitted under the 
 * OpenSceneGraph Public License Version 0.0 (exception 3) 
 * and the GNU Lesser Public License Version 2 (clause 3).
 **/

#ifndef __SIMDATA_MATH_H__
#define __SIMDATA_MATH_H__

#include <cmath>
#include <SimData/Namespace.h>


NAMESPACE_SIMDATA

class Vector3;

#ifdef PI
#undef PI
#undef PI_2
#undef PI_4
#endif
const double PI   = 3.14159265358979323846;
const double PI_2 = 1.57079632679489661923;
const double PI_4 = 0.78539816339744830962;

/** return the minimum of two values, equivilant to std::min.
  * std::min not used because of STL implementation under IRIX contains no std::min.*/
template<typename T>
inline T absolute(T v) { return v<(T)0?-v:v; }

/** return the minimum of two values, equivilant to std::min.
  * std::min not used because of STL implementation under IRIX contains no std::min.*/
template<typename T>
inline T minimum(T lhs,T rhs) { return lhs<rhs?lhs:rhs; }

/** return the maximum of two values, equivilant to std::max.
  * std::max not used because of STL implementation under IRIX contains no std::max.*/
template<typename T>
inline T maximum(T lhs,T rhs) { return lhs>rhs?lhs:rhs; }

template<typename T>
inline T clampTo(T v,T minimum,T maximum) { return v<minimum?minimum:v>maximum?maximum:v; }

template<typename T>
inline T clampAbove(T v,T minimum) { return v<minimum?minimum:v; }

template<typename T>
inline T clampBelow(T v,T maximum) { return v>maximum?maximum:v; }

template<typename T>
inline T sign(T v) { return v<(T)0?(T)-1:(T)1; }

template<typename T>
inline T square(T v) { return v*v; }

template<typename T>
inline T signedSquare(T v) { return v<(T)0?-v*v:v*v;; }

template<typename T>
inline T toRadians(T deg) { return deg * PI / ((T)180); }

template<typename T>
inline T toDegrees(T rad) { return rad * ((T)180) / PI; }

template<typename T>
inline T inDegrees(T angle) { return toRadians(angle); }

template<typename T>
inline T inRadians(T angle) { return angle; }

template<typename T>
inline void swap(T &a, T &b) { T t = a; a = b; b = t; }


#if defined(_MSC_VER) && (_MSC_VER >= 1300)
    #include <float.h>
#endif

#if (defined(WIN32) && !(defined(_MSC_VER) && (_MSC_VER >= 1300)) && !defined(__MINGW32__) ) ||  defined (sun)
    #ifndef isnanf
    #define isnanf (float)isnan
    #endif
#endif

#if defined(WIN32) && !defined(__CYGWIN__) && !defined(__MWERKS__)
    inline bool isNaN(float v) { return _isnan(v)!=0; }
    inline bool isNaN(double v) { return _isnan(v)!=0; }
#else
    # if defined(__DARWIN_OSX__) && !defined (DARWIN_OSX_PRE_10_2)
        inline bool isNaN(float v) { return __isnanf(v); }
        inline bool isNaN(double v) { return __isnand(v); }
    #else
        inline bool isNaN(float v) { return isnan(v); }
        inline bool isNaN(double v) { return isnan(v); }
    #endif
#endif


NAMESPACE_SIMDATA_END

#endif //__SIMDATA_MATH_H__
