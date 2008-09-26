#ifndef __CSPSIM_CLOUDS_CLOUDREGISTRY_H__
#define __CSPSIM_CLOUDS_CLOUDREGISTRY_H__

#include <osg/ref_ptr>
#include <csp/cspsim/Export.h>
#include <csp/cspsim/weather/clouds/Cloud.h>
#include <csp/cspsim/weather/clouds/CloudBox.h>

namespace csp {
namespace weather {
namespace clouds {

/* This class can be used to create several default clouds. These default clouds
can then be used to populate the scene at random places. */
class CSPSIM_EXPORT CloudRegistry
{
public:
	typedef std::vector<osg::ref_ptr<Cloud> > CloudVector;

	static osg::ref_ptr<Cloud> CreateModel01();
	static osg::ref_ptr<Cloud> CreateModel02();

	static CloudVector CreateDefaultClouds();

	static CloudBox::ColorLevelVector CreateDefaultColorLevels();
	static CloudBox::OpacityLevelVector CreateDefaultOpacityLevels();
};

}	// end namespace clouds
} 	// end namespace weather
}	// end namespace csp

#endif
