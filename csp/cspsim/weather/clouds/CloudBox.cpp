#include <osg/Texture2D>

#include <csp/cspsim/weather/clouds/CloudBox.h>
#include <csp/cspsim/weather/clouds/CloudMath.h>
#include <csp/cspsim/weather/clouds/CloudTextureFactory.h>

template<class T>
T FindCorrectLevel(std::vector<std::pair<float, T> >& v, float value) {
	T default_value;

	for(typename std::vector<std::pair<float, T> >::size_type index = 0;index < v.size();++index) {
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
namespace weather {
namespace clouds {

const osg::Billboard::Mode CloudBox::BillboardMode = osg::Billboard::POINT_ROT_EYE; 
const float LODDistance1 = 3000;
const float LODDistance2 = 5000;
const float LODDistance3 = 8000;
const float LODDistance4 = 10000;

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
	addChild(CreateHighDetailGeometry().get(), 0.0, LODDistance1);
	addChild(CreateMediumDetailGeometry().get(), LODDistance1, LODDistance2);
	addChild(CreateLowDetailGeometry().get(), LODDistance2, LODDistance3);
}

osg::ref_ptr<osg::Geometry> CloudBox::CreateCloudSprite(const float& alpha, const osg::Vec3& color, const osg::Vec2& size) {
	osg::Vec3 corner(0 - size.x() / 2, 0.0f, 0 - size.y() / 2);
	osg::Vec3 width(size.x(), 0.0f, 0.0f);
	osg::Vec3 height(0.0f, 0.0f, size.y());
	osg::Vec4 colorWithAlpha(color, alpha);

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(corner);
	vertices->push_back(corner + width);
	vertices->push_back(corner + width + height);
	vertices->push_back(corner + height);
	geometry->setVertexArray(vertices.get());

	// Apply a random rotation of the cloud sprite. The cloud looks
	// a little better then.
	float q = osg::DegreesToRadians(CloudMath::GenerateRandomNumber(0, 360));
	osg::Matrix matrix = osg::Matrix::rotate(q, osg::Vec3(0, 1, 0));
	vertices->at(0) = osg::Matrix::transform3x3(vertices->at(0), matrix);
	vertices->at(1) = osg::Matrix::transform3x3(vertices->at(1), matrix);
	vertices->at(2) = osg::Matrix::transform3x3(vertices->at(2), matrix);
	vertices->at(3) = osg::Matrix::transform3x3(vertices->at(3), matrix);
	
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(colorWithAlpha);
	colors->push_back(colorWithAlpha);
	colors->push_back(colorWithAlpha);
	colors->push_back(colorWithAlpha);

	geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array();
	norms->push_back((width^height));
	norms->at(0).normalize();
    
    geometry->setNormalArray(norms.get());
    geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Vec2Array> textureCoordinates = new osg::Vec2Array();
	textureCoordinates->push_back(osg::Vec2(0.0f, 0.0f));
    textureCoordinates->push_back(osg::Vec2(1.0f, 0.0f));
    textureCoordinates->push_back(osg::Vec2(1.0f, 1.0f));
    textureCoordinates->push_back(osg::Vec2(0.0f, 1.0f));
    geometry->setTexCoordArray(0, textureCoordinates.get());

	geometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	osg::ref_ptr<osg::Texture2D> texture = CloudTextureFactory::Instance()->getTexture("cloud_texture02.png");
	stateset->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
	geometry->setStateSet(stateset.get());

	return geometry;
}

osg::ref_ptr<osg::Node> CloudBox::CreateHighDetailGeometry() {
	osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
    billboard->setMode(BillboardMode);

	// Calculate the center of the cloud.
	osg::Vec3 cloudCenter(0.0, 0.0, 0.0);
	float maximumDistance = sqrt(m_Dimensions.x() * m_Dimensions.x() + 
		m_Dimensions.y() * m_Dimensions.y() + m_Dimensions.z() * m_Dimensions.z());

	billboard->addDrawable(CreateCloudSprite(1.0, osg::Vec3(1.0, 1.0, 1.0), osg::Vec2(m_Dimensions.x(), m_Dimensions.x())).get());

	// Randomly place a number of sprites.
	for(int i = 0;i<m_Density;++i) {
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
			osg::Vec2 size(CloudMath::GenerateRandomNumber(4, 9), CloudMath::GenerateRandomNumber(5, 7));
			osg::ref_ptr<osg::Geometry> drawable = CreateCloudSprite(selectedOpacity, selectedColor, size);
			billboard->addDrawable(drawable.get(), position);
		}
	}
	return billboard.get();
}

osg::ref_ptr<osg::Node> CloudBox::CreateMediumDetailGeometry() {
	osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
    billboard->setMode(BillboardMode);
	
    int numSpritesToCreate = std::max(m_Density / 10, 3);
    float distanceBetweenSprites = m_Dimensions.x()*2 / static_cast<float>(numSpritesToCreate + 1); 
    float xPosition = 0.0 - m_Dimensions.x() + distanceBetweenSprites;
    
    for(int i = 0;i < numSpritesToCreate;++i) {
    	osg::Vec3 position(xPosition, 0.0, 0.0);
    	billboard->addDrawable(CreateCloudSprite(1.0, osg::Vec3(1.0, 1.0, 1.0), osg::Vec2(m_Dimensions.x()*2, m_Dimensions.x()*2)).get(), position);    	
    }
    
	return billboard.get();
}

osg::ref_ptr<osg::Node> CloudBox::CreateLowDetailGeometry() {
	osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
    billboard->setMode(BillboardMode);
	
	billboard->addDrawable(CreateCloudSprite(1.0, osg::Vec3(1.0, 1.0, 1.0), osg::Vec2(m_Dimensions.x()*2, m_Dimensions.x()*2)).get());

	return billboard.get();
}

} // end namespace clouds
} // end namespace weather
} // end namespace csp
