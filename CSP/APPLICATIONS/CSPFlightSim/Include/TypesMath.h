// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file TypesMath.h
 *
 **/

#ifndef TYPESMATHTAG
#define TYPESMATHTAG

const double pi = 3.1415926536;

#ifndef HALF_PI
#define HALF_PI 1.57f
#endif

#include "math.h"
#include "TypesVector3.h"
#include "TypesVector4.h"
#include "TypesMatrix3.h"
#include "TypesMatrix4.h"
#include "TypesQuaternion.h"

/**
 * class StandardMath
 *
 * @author unknown
 */
  class StandardMath
  {

    private:

    public:
  
  };

inline double RadiansToDegrees(double rad) 
{ 
	return rad * 180.0 / pi;
}

inline double DegreesToRadians(double deg) 
{ 
	return deg * pi / 180.0;
}

inline double MetersPerSecondToKnots(double p_speed)
{
	return p_speed * 1.94385;
}

inline double MetersPerSecondToMachs(double p_speed)
{
	return p_speed / 331.622;
}

inline double MetersToFeets(double p_length)
{
	return p_length * 3.28084;
}

double angleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2);

double SignOf(double p_t);

double Atan(double const p_a, double const p_b);

float UnsignedRandomNumber();

float SignedRandomNumber();

#endif // __TYPESMATH_H__




