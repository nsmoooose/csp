#ifndef __CSPSIM_CLOUDS_CLOUDREGISTRY_H__
#define __CSPSIM_CLOUDS_CLOUDREGISTRY_H__

#include <osg/ref_ptr>
#include "Cloud.h"
#include "CloudBox.h"

namespace csp {
namespace clouds {

/* This class can be used to create several default clouds. These default clouds
can then be used to populate the scene at random places. */
class CloudRegistry
{
public:
	typedef std::vector<osg::ref_ptr<Cloud> > CloudVector;

	static osg::ref_ptr<Cloud> CreateModel01();
	static osg::ref_ptr<Cloud> CreateModel02();

	static CloudVector CreateDefaultClouds();

	static CloudBox::ColorLevelVector CloudRegistry::CreateDefaultColorLevels();
	static CloudBox::OpacityLevelVector CloudRegistry::CreateDefaultOpacityLevels();
};

}	// end namespace clouds
}	// end namespace csp

#endif
