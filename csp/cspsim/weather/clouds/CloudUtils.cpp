#include <csp/cspsim/weather/clouds/CloudMath.h>
#include <csp/cspsim/weather/clouds/CloudUtils.h>

#include <osg/MatrixTransform>

namespace csp {
namespace weather {
namespace clouds {

void CloudUtils::addRandomClouds(osg::Group* model, CloudRegistry::CloudVector& clouds, osg::Vec3 dimensions, int count) {
	CloudRegistry::CloudVector::size_type cloudIndex = 0;
	for(int i=0;i<count;++i, ++cloudIndex) {
		// Wrap around when there is no more clouds.
		if(cloudIndex >= clouds.size()) 
			cloudIndex = 0;

		float x = CloudMath::GenerateRandomNumber(0 - dimensions.x(), dimensions.x());
		float y = CloudMath::GenerateRandomNumber(0 - dimensions.y(), dimensions.y());
		float z = CloudMath::GenerateRandomNumber(0 - dimensions.z(), dimensions.z());

		osg::ref_ptr<osg::MatrixTransform> transformation = new osg::MatrixTransform();
		transformation->setMatrix(osg::Matrix::translate(x, y, z));
		transformation->addChild(clouds.at(cloudIndex).get());
		model->addChild(transformation.get());
	}
}

} // End namespace clouds
} // End namespace weather
} // End namespace csp
