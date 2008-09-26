#ifndef __CSPSIM_WEATHER_CLOUDS_GAMESCREEN_H__
#define __CSPSIM_GAMESCREEN_H__

#include <csp/cspsim/Export.h>
#include <csp/cspsim/weather/clouds/CloudRegistry.h>

namespace csp {
namespace weather {
namespace clouds {

class CSPSIM_EXPORT CloudUtils {
public:
	static void addRandomClouds(osg::Group* model, CloudRegistry::CloudVector& clouds, osg::Vec3 dimensions, int count);
};

} // End namespace clouds
} // End namespace weather
} // End namespace csp

#endif
