#include "RemoveRedundantCloudSpritesVisitor.h"
#include "../CloudBox.h"
#include "../CloudSprite.h"

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
	for(int i = cloudBox->getNumChildren()-1;i >= 0;--i) {
		// Get sprite and its position.
		osg::ref_ptr<CloudSprite> sprite = dynamic_cast<CloudSprite*>(cloudBox->getChild(i));
		osg::Vec3 spritePosition = sprite->getPosition(0);

		bool deleteSprite = false;

		for(int j = cloudBox->getNumChildren()-1;j >= 0;--j) {
			// Don't compare with myself...
			if(i == j)
				break;

			osg::ref_ptr<CloudSprite> spriteToTest = dynamic_cast<CloudSprite*>(cloudBox->getChild(j));
			if(!spriteToTest.valid())
				continue;
			osg::Vec3 spritePositionToTest = spriteToTest->getPosition(0);

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
			cloudBox->removeChild(i);
	}
}

} // end namespace visitors
} // end namespace clouds
} // end namespace csp
