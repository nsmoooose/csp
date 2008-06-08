#ifndef __CSPSIM_CLOUDS_CLOUDMATH_H__
#define __CSPSIM_CLOUDS_CLOUDMATH_H__

#include <osg/Vec3>

namespace csp {
namespace weather {
namespace clouds {

class CloudMath
{
public:
	// Returns true if point is inside the ellipsoid.
	static bool InsideEllipsoid(osg::Vec3 point, osg::Vec3 ellipsoid);
	static float GenerateRandomNumber(float minimum, float maximum);
};

}	// end namespace clouds
} 	// end namespace weather
}	// end namespace csp

#endif
