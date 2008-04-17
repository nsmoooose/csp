#include <osg/AlphaFunc>
#include <osg/BlendFunc>

#include "CloudBox.h"
#include "CloudSprite.h"

CloudBox::CloudBox(void) : m_Depth(10.0f), m_Height(10.0f), m_Width(10.0f) {
	// This is the default color vector. Lower parts of the cloud will get a 
	// darker shade and higher parts will be more white.
	m_ColorLevels.push_back(ColorLevel(0.0, osg::Vec3(0.8, 0.8, 0.8)));
	m_ColorLevels.push_back(ColorLevel(0.25, osg::Vec3(0.82, 0.82, 0.82)));
	m_ColorLevels.push_back(ColorLevel(0.30, osg::Vec3(0.85, 0.85, 0.85)));
	m_ColorLevels.push_back(ColorLevel(0.40, osg::Vec3(0.87, 0.87, 0.87)));
	m_ColorLevels.push_back(ColorLevel(0.55, osg::Vec3(0.9, 0.9, 0.9)));
	m_ColorLevels.push_back(ColorLevel(0.60, osg::Vec3(0.93, 0.93, 0.93)));
	m_ColorLevels.push_back(ColorLevel(0.65, osg::Vec3(0.97, 0.97, 0.97)));
	m_ColorLevels.push_back(ColorLevel(0.70, osg::Vec3(1.0, 1.0, 1.0)));

	m_OpacityLevels.push_back(OpacityLevel(0.0, 0.1));
	m_OpacityLevels.push_back(OpacityLevel(0.1, 0.3));
	m_OpacityLevels.push_back(OpacityLevel(0.3, 0.7));
	m_OpacityLevels.push_back(OpacityLevel(0.4, 1.0));
}

CloudBox::~CloudBox(void) {
}

void CloudBox::setDepth(float depth) {
	m_Depth = depth;
}

float CloudBox::getDepth() {
	return m_Depth;
}

void CloudBox::setHeight(float height) {
	m_Height = height;
}

float CloudBox::getHeight() {
	return m_Height;
}

void CloudBox::setWidth(float width) {
	m_Width = width;
}

float CloudBox::getWidth() {
	return m_Width;
}

void CloudBox::UpdateModel() {
	// Hint osg that we must handle the cloud as a transparent object.
	osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// Must also tell osg that we need to blend the alpha channel with
	// the background.
	osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateset->setAttributeAndModes(blendFunction.get());

	osg::ref_ptr<osg::AlphaFunc> alphafunc = new osg::AlphaFunc;
	alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
	stateset->setAttributeAndModes(alphafunc.get(), osg::StateAttribute::ON);

	// Calculate the center of the cloud.
	osg::Vec3 cloudCenter(m_Width/2, m_Depth/2, m_Height/2);
	float maximumDistance = sqrt(cloudCenter.x() * cloudCenter.x() + 
		cloudCenter.y() * cloudCenter.y() + cloudCenter.z() * cloudCenter.z());

	// Randomly place a number of sprites.
	for(int i = 0;i<250;++i) {
		// Create a sprite and set the billboard type to use.
		osg::ref_ptr<CloudSprite> sprite = new CloudSprite();
		sprite->setMode(osg::Billboard::POINT_ROT_EYE);

		// Calculate the position of the sprite.
		osg::Vec3 position((rand() % (int)m_Width), 
			(rand() % (int)m_Depth), (rand() % (int)m_Height));

		// Calculate the distance to the sprite from the center of this cloud. 
		// The further away from the center of the cloud we add transparency.
		osg::Vec3 delta = position - cloudCenter;
		float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());
		float dissipation = 1.0f - distance / maximumDistance;

		OpacityLevel selectedOpacityLevel;
		for(OpacityLevelVector::size_type index = 0;index < m_OpacityLevels.size();++index) {
			if(dissipation >= m_OpacityLevels.at(index).threshold) {
				selectedOpacityLevel = m_OpacityLevels.at(index);
			}
			else {
				break;
			}
		}

		// Calculate the color level to use.
		float colorLevel = position.z() / m_Height;

		// Retreive the color using our color vector.
		ColorLevel selectedColorLevel;
		for(ColorLevelVector::size_type index = 0;index < m_ColorLevels.size();++index) {
			if(colorLevel >= m_ColorLevels.at(index).threshold) {
				selectedColorLevel = m_ColorLevels.at(index);
			}
			else {
				break;
			}
		}


		// Update the model with the calculated data.
		sprite->UpdateModel(position, selectedOpacityLevel.opacity, selectedColorLevel.color);

		addChild(sprite.get());
	}

	for(int i = getNumChildren()-1;i >= 0;--i) {
		// Get sprite and its position.
		osg::ref_ptr<CloudSprite> sprite = dynamic_cast<CloudSprite*>(getChild(i));
		osg::Vec3 spritePosition = sprite->getPosition(0);

		bool deleteSprite = false;

		for(int j = this->getNumChildren()-1;j >= 0;--j) {
			// Don't compare with myself...
			if(i == j)
				break;

			osg::ref_ptr<CloudSprite> spriteToTest = dynamic_cast<CloudSprite*>(getChild(j));
			osg::Vec3 spritePositionToTest = spriteToTest->getPosition(0);

			osg::Vec3 delta = spritePositionToTest - spritePosition;
			float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());

			// This value should be computed depending on the width and height of the sprite.
			if(distance < 2.66)
			{
				deleteSprite = true;
				break;
			}
		}

		if(deleteSprite)
			removeChild(i);
	}
}
