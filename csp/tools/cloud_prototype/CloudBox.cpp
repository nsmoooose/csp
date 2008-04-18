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

namespace csp {
namespace clouds {

	CloudBox::CloudBox(void) : m_Density(100) {
}

CloudBox::~CloudBox(void) {
}

void CloudBox::setDimensions(osg::Vec3 dimensions) {
	m_Dimensions = dimensions;
}

osg::Vec3 CloudBox::getDimensions() {
	return m_Dimensions;
}

void CloudBox::setColorLevels(const ColorLevelVector& levels) {
	m_ColorLevels = levels;
}

CloudBox::ColorLevelVector CloudBox::getColorLevels() {
	return m_ColorLevels;
}

void CloudBox::setOpacityLevels(const OpacityLevelVector& levels) {
	m_OpacityLevels = levels;
}

CloudBox::OpacityLevelVector CloudBox::getOpacityLevels() {
	return m_OpacityLevels;
}

void CloudBox::setDensity(int count) {
	m_Density = count;
}

int CloudBox::getDensity() {
	return m_Density;
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
	for(int i = 0;i<m_Density;++i) {
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
}

} // end namespace clouds
} // end namespace csp
