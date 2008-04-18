#include "RemoveRedundantCloudSpritesVisitor.h"
#include "../CloudBox.h"

namespace csp {
namespace clouds {
namespace visitors {

RemoveRedundantCloudSpritesVisitor::RemoveRedundantCloudSpritesVisitor(float threshold) : 
  osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_Threshold(threshold) {
}

void RemoveRedundantCloudSpritesVisitor::apply(osg::Node& node) {
	CloudBox* cloudBox = dynamic_cast<CloudBox*>(&node);
	if(cloudBox != NULL) {
		RemoveSprites(cloudBox);
	}
	traverse(node);
}

void RemoveRedundantCloudSpritesVisitor::RemoveSprites(CloudBox* cloudBox) {
	// There is probably a lot of sprites that are very close to each other.
	// These will draw over each other and generate some overhead. We loop
	// through all sprites and check distances between them. If any are closer
	// than a specified threshold we will remove it completely.
	for(int i = cloudBox->getNumDrawables()-1;i >= 0;--i) {
		// Get sprite and its position.
		osg::Vec3 spritePosition = cloudBox->getPosition(i);

		bool deleteSprite = false;

		for(int j = cloudBox->getNumDrawables()-1;j >= 0;--j) {
			// Don't compare with myself...
			if(i == j)
				break;

			osg::Vec3 spritePositionToTest = cloudBox->getPosition(j);

			osg::Vec3 delta = spritePositionToTest - spritePosition;
			float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());

			// This value should be computed depending on the width and height of the sprite.
			if(distance < m_Threshold)
			{
				deleteSprite = true;
				break;
			}
		}

		if(deleteSprite)
			cloudBox->removeDrawable(cloudBox->getDrawable(i));
	}
}

} // end namespace visitors
} // end namespace clouds
} // end namespace csp
