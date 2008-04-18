#ifndef __CSPSIM_CLOUDS_CLOUDMATH_H__
#define __CSPSIM_CLOUDS_CLOUDMATH_H__

#include <osg/vec3>

namespace csp {
namespace clouds {

class CloudMath
{
public:
	// Returns true if point is inside the ellipsoid.
	static bool InsideEllipsoid(osg::Vec3 point, osg::Vec3 ellipsoid);
	static float GenerateRandomNumber(float minimum, float maximum);
};

}	// end namespace clouds
}	// end namespace csp

#endif
