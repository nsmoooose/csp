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
{
	if (p_t > 0.0)
		return 1.0;
	else
	 if (p_t < 0.0)
		return -1.0;
	 else
		return 0.0;
}