#include "stdinc.h"

#include "TypesMath.h"



/*! \fn double AngleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2)
 *
 * Calculates the angle between two vectors (with orientation)
 */
double angleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2)
{
  double l1 = v1.Length(), l = l1 * v2.Length();

  if (l == 0.0)
   return 0.0;

  double cosa = Dot(v1, v2) / l;
  StandardVector3 crossVec = v1^v2;

  double orientation = Dot(StandardVector3(0,0,1),crossVec);
  if (orientation == 0.0)
	  orientation = Dot (StandardVector3(1,0,0), crossVec);
  if (orientation == 0.0)
      orientation = - Dot (StandardVector3(0,1,0), crossVec);
  // if the orientation is negative then the angle is negative.
  if (orientation > 0.0)
    return acos(cosa);
  else
    return -acos(cosa);
}

double SignOf(double p_t)
{ // return 1 if p_t >0, 0 if p_t = 0, -1 otherwise
	if (p_t > 0.0)
		return 1.0;
	else
		return (p_t < 0.0)?-1.0:0.0;
}

double Atan(double const p_a, double const p_b)
{ // returns atan ( p_b / p_a ) with some conventions (even if | p_a | < epsilon)
	if ( p_a == 0.0 )
		return SignOf(p_b) * pi / 2.0;
	else if ( fabs( p_b / p_a ) > 1.0e37 )
		return SignOf(p_a) * SignOf(p_b) * pi / 2.0;
	else 
		return atan( p_b / p_a );
}