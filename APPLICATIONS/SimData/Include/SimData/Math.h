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


#ifndef __SIMDATA_MATH_H__
#define __SIMDATA_MATH_H__

#include <cmath>
#include <SimData/ns-simdata.h>
#include <SimData/GlibCsp.h>


NAMESPACE_SIMDATA

class Vector3;

inline double DegreesToRadians(double deg)
{
        return deg * G_PI / 180.0f;
}

inline double RadiansToDegrees(double rad)
{
        return rad * 180.0f / G_PI;
}

inline double SignOf(double x)
{
	if (x > 0.0) return 1.0;
	if (x < 0.0) return -1.0;
	return 0.0;
}

double angleBetweenTwoVectors(const Vector3 & v1, const Vector3 & v2);


double const FtInM = 0.3048;
double const MInFt = 3.2808;

inline double FeetToMeters(double feet) {
	return FtInM * feet;
}

inline double MetersToFeet(double meters) {
	return MInFt * meters;
}

double const PdsToN = 4.44822;

double const NToPds = 0.22481;

inline double PoundsToNewtons(double pds) {
	return PdsToN * pds;
}

inline double NewtonsToPounds(double newtonw) {
	return NToPds * newtonw;
}

NAMESPACE_END // namespace simdata

#endif //__SIMDATA_MATH_H__
