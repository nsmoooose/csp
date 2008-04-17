#include <osg/AlphaFunc>
#include <osg/BlendFunc>

#include "CloudBox.h"
#include "CloudMath.h"
#include "CloudSprite.h"

template<class T>
T FindCorrectLevel(std::vector<std::pair<float, T> >& v, float value) {
	T default_value;

	for(std::vector<std::pair<float, T> >::size_type index = 0;index < v.size();++index) {
		if(value >= v.at(index).first) {
			default_value = v.at(index).second;
		}
		else {
			break;
		}
	}
	return default_value;
}

CloudBox::CloudBox(void) : m_SpriteRemovalThreshold(2.0) {
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

void CloudBox::setDimensions(osg::Vec3 dimensions) {
	m_Dimensions = dimensions;
}

osg::Vec3 CloudBox::getDimensions() {
	return m_Dimensions;
}

void CloudBox::setSpriteRemovalThreshold(float threshold) {
	m_SpriteRemovalThreshold = threshold;
}

float CloudBox::getSpriteRemovalThreshold() {
	return m_SpriteRemovalThreshold;
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
	osg::Vec3 cloudCenter(0.0, 0.0, 0.0);
	float maximumDistance = sqrt(m_Dimensions.x() * m_Dimensions.x() + 
		m_Dimensions.y() * m_Dimensions.y() + m_Dimensions.z() * m_Dimensions.z());

	// Randomly place a number of sprites.
	for(int i = 0;i<250;++i) {
		// Create a sprite and set the billboard type to use.
		osg::ref_ptr<CloudSprite> sprite = new CloudSprite();
		sprite->setMode(osg::Billboard::AXIAL_ROT);

		// Calculate the position of the sprite.
		osg::Vec3 position(
			CloudMath::GenerateRandomNumber(0 - m_Dimensions.x(), m_Dimensions.x()), 
			CloudMath::GenerateRandomNumber(0 - m_Dimensions.y(), m_Dimensions.y()), 
			CloudMath::GenerateRandomNumber(0 - m_Dimensions.z(), m_Dimensions.z()));

		if(CloudMath::InsideEllipsoid(position, m_Dimensions)) {
			// Calculate the distance to the sprite from the center of this cloud. 
			// The further away from the center of the cloud we add transparency.
			osg::Vec3 delta = position - cloudCenter;
			float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());
			float dissipation = 1.0f - distance / maximumDistance;
			float selectedOpacity = FindCorrectLevel(m_OpacityLevels, dissipation);

			// Calculate the color level to use. Retreive the color using our color vector.
			float colorLevel = (position.z() + m_Dimensions.z()) / (m_Dimensions.z() * 2);
			osg::Vec3 selectedColor = FindCorrectLevel(m_ColorLevels, colorLevel);

			// Update the model with the calculated data.
			sprite->UpdateModel(position, selectedOpacity, selectedColor);

			addChild(sprite.get());
		}
	}

	// There is probably a lot of sprites that are very close to each other.
	// These will draw over each other and generate some overhead. We loop
	// through all sprites and check distances between them. If any are closer
	// than a specified threshold we will remove it completely.
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
			if(!spriteToTest.valid())
				continue;
			osg::Vec3 spritePositionToTest = spriteToTest->getPosition(0);

			osg::Vec3 delta = spritePositionToTest - spritePosition;
			float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());

			// This value should be computed depending on the width and height of the sprite.
			if(distance < m_SpriteRemovalThreshold)
			{
				deleteSprite = true;
				break;
			}
		}

		if(deleteSprite)
			removeChild(i);
	}
}
