#ifndef TYPESMATHTAG
#define TYPESMATHTAG

#ifndef HALF_PI
#define HALF_PI 1.57f
#endif

#ifndef PI
#define PI 3.1415f
#endif


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

inline	float	DegreesToRadians(float deg)
{
	return deg * PI / 180.0f;
}

inline	float	RadiansToDegrees(float rad)
{	
	return rad * 180.0f / PI;
}




#endif