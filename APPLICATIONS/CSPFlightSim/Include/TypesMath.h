#ifndef TYPESMATHTAG
#define TYPESMATHTAG

const double pi = 3.1415926536;

#ifndef HALF_PI
#define HALF_PI 1.57f
#endif

#ifndef PI
#define PI 3.1415f
#endif



#include "math.h"
#include "TypesVector3.h"
#include "TypesVector4.h"
#include "TypesMatrix3.h"
#include "TypesMatrix4.h"
#include "TypesQuaternion.h"

  class StandardMath
  {

    private:

    public:
  
  };



inline	double	DegreesToRadians(double deg)
{
	return deg * pi / 180.0f;
}

inline	double	RadiansToDegrees(double rad)
{	
	return rad * 180.0f / pi;
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

#endif



