#include "Cloud.h"
#include "CloudBox.h"
#include "CloudRegistry.h"

namespace csp {
namespace clouds {

osg::ref_ptr<Cloud> CloudRegistry::CreateModel01() {
	osg::ref_ptr<Cloud> cloud = new Cloud();
	cloud->setSpriteRemovalThreshold(1.0);

	osg::ref_ptr<CloudBox> cloudBox01 = new CloudBox();
	cloudBox01->setDimensions(osg::Vec3(15.0, 15.0, 5.0));
	cloudBox01->setColorLevels(CreateDefaultColorLevels());
	cloudBox01->setOpacityLevels(CreateDefaultOpacityLevels());
	cloudBox01->setDensity(250);
	cloud->addChild(cloudBox01.get());
	cloud->UpdateModel();
	return cloud;
}

osg::ref_ptr<Cloud> CloudRegistry::CreateModel02() {
	osg::ref_ptr<Cloud> cloud = new Cloud();
	cloud->setSpriteRemovalThreshold(1.0);

	osg::ref_ptr<CloudBox> cloudBox01 = new CloudBox();
	cloudBox01->setDimensions(osg::Vec3(20.0, 15.0, 5.0));
	cloudBox01->setColorLevels(CreateDefaultColorLevels());
	cloudBox01->setOpacityLevels(CreateDefaultOpacityLevels());
	cloudBox01->setDensity(300);
	cloud->addChild(cloudBox01.get());

	osg::ref_ptr<CloudBox> cloudBox02 = new CloudBox();
	cloudBox02->setDimensions(osg::Vec3(5.0, 5.0, 8.0));
	cloudBox02->setColorLevels(CreateDefaultColorLevels());
	cloudBox02->setOpacityLevels(CreateDefaultOpacityLevels());
	cloudBox02->setMatrix(osg::Matrix::translate(0, 0, 4));
	cloudBox01->setDensity(100);
	cloud->addChild(cloudBox02.get());

	cloud->UpdateModel();
	return cloud;
}

CloudRegistry::CloudVector CloudRegistry::CreateDefaultClouds() {
	CloudVector clouds;
	clouds.push_back(CreateModel01());
	clouds.push_back(CreateModel02());

	return clouds;
}


CloudBox::ColorLevelVector CloudRegistry::CreateDefaultColorLevels() {
	// Lower parts of the cloud will get a  darker shade and higher parts will be more white.
	CloudBox::ColorLevelVector levels;
	levels.push_back(CloudBox::ColorLevel(0.0, osg::Vec3(0.8, 0.8, 0.8)));
	levels.push_back(CloudBox::ColorLevel(0.25, osg::Vec3(0.82, 0.82, 0.82)));
	levels.push_back(CloudBox::ColorLevel(0.30, osg::Vec3(0.85, 0.85, 0.85)));
	levels.push_back(CloudBox::ColorLevel(0.40, osg::Vec3(0.87, 0.87, 0.87)));
	levels.push_back(CloudBox::ColorLevel(0.55, osg::Vec3(0.9, 0.9, 0.9)));
	levels.push_back(CloudBox::ColorLevel(0.60, osg::Vec3(0.93, 0.93, 0.93)));
	levels.push_back(CloudBox::ColorLevel(0.65, osg::Vec3(0.97, 0.97, 0.97)));
	levels.push_back(CloudBox::ColorLevel(0.70, osg::Vec3(1.0, 1.0, 1.0)));
	return levels;
}

CloudBox::OpacityLevelVector CloudRegistry::CreateDefaultOpacityLevels() {
	CloudBox::OpacityLevelVector levels;
	levels.push_back(CloudBox::OpacityLevel(0.0, 0.1));
	levels.push_back(CloudBox::OpacityLevel(0.1, 0.3));
	levels.push_back(CloudBox::OpacityLevel(0.3, 0.7));
	levels.push_back(CloudBox::OpacityLevel(0.4, 1.0));
	return levels;
}

} // end namespace clouds
} // end namespace csp
