#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Billboard>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/osg.h>
#include <csp/cspsim/weather/clouds/CloudModel.h>
#include <csp/cspsim/weather/clouds/CloudTextureFactory.h>

namespace csp {
namespace weather {
namespace clouds {

CSP_XML_BEGIN(CloudLOD)
CSP_DEF("distance1", m_Distance1, false)
CSP_DEF("distance2", m_Distance2, false)
CSP_DEF("distance3", m_Distance3, false)
CSP_XML_END

CSP_XML_BEGIN(CloudColorLevels)
CSP_DEF("colors", m_Colors, false)
CSP_DEF("levels", m_Levels, false)
CSP_XML_END

CSP_XML_BEGIN(CloudOpacityLevels)
CSP_DEF("opacity", m_Opacity, false)
CSP_DEF("levels", m_Levels, false)
CSP_XML_END

CSP_XML_BEGIN(CloudSprite)
CSP_DEF("position", m_Position, false)
CSP_DEF("rotation", m_Rotation, false)
CSP_DEF("size", m_Size, false)
CSP_XML_END

CSP_XML_BEGIN(CloudBox)
CSP_DEF("size", m_Size, false)
CSP_DEF("sprites", m_Sprites, false)
CSP_DEF("color", m_ColorLevels, false)
CSP_DEF("opacity", m_OpacityLevels, false)
CSP_DEF("lod", m_LOD, false)
CSP_XML_END

CSP_XML_BEGIN(CloudModel)
CSP_DEF("cloud_boxes", m_CloudBoxes, false)
CSP_DEF("cloud_box_positions", m_CloudBoxPositions, false)
CSP_XML_END

template<class T>
T FindCorrectLevel(std::vector<float>& limits, std::vector<T>& values, float value) {
	T default_value;

	for(typename std::vector<float>::size_type index = 0;index < limits.size();++index) {
		if(value >= limits.at(index)) {
			default_value = values.at(index);
		}
		else {
			break;
		}
	}
	return default_value;
}

osg::Geometry* CreateCloudSprite(const float& alpha, const osg::Vec3& color,
								 const osg::Vec2& size, float rotation) {
	osg::Vec3 corner(0 - size.x() / 2, 0.0f, 0 - size.y() / 2);
	osg::Vec3 width(size.x(), 0.0f, 0.0f);
	osg::Vec3 height(0.0f, 0.0f, size.y());
	osg::Vec4 colorWithAlpha(color, alpha);

	osg::Geometry* geometry = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(corner);
	vertices->push_back(corner + width);
	vertices->push_back(corner + width + height);
	vertices->push_back(corner + height);
	geometry->setVertexArray(vertices.get());

	// A random rotation of the cloud sprite. The cloud looks
	// a little better then.
	osg::Matrix matrix = osg::Matrix::rotate(rotation, osg::Vec3(0, 1, 0));
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
	osg::ref_ptr<osg::Texture2D> texture = weather::clouds::CloudTextureFactory::Instance()->getTexture("weather/clouds/cloud_texture02.png");
	stateset->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
	geometry->setStateSet(stateset.get());

	return geometry;
}

osg::Node* CreateHighDetailGeometry(weather::clouds::CloudBox* box) {
	osg::Billboard* billboard = new osg::Billboard();
    billboard->setMode(osg::Billboard::POINT_ROT_EYE);

	osg::Vec3 cloudCenter(0.0, 0.0, 0.0);
	osg::Vec3 dimensions(box->m_Size.x(), box->m_Size.y(), box->m_Size.z());
	float maximumDistance = sqrt(
		dimensions.x() * dimensions.x() +
		dimensions.y() * dimensions.y() +
		dimensions.z() * dimensions.z());

	for(Link<weather::clouds::CloudSprite>::vector::size_type index=0;index < box->m_Sprites.size();++index) {
		weather::clouds::CloudSprite* sprite = box->m_Sprites.at(index).get();
		osg::Vec3 position(sprite->m_Position.x(), sprite->m_Position.y(), sprite->m_Position.z());
		osg::Vec3 delta = position - cloudCenter;
		float distance = sqrt(delta.x() * delta.x() + delta.y() * delta.y() + delta.z() * delta.z());
		float dissipation = 1.0f - distance / maximumDistance;
		float selectedOpacity = FindCorrectLevel(box->m_OpacityLevels->m_Levels, box->m_OpacityLevels->m_Opacity, dissipation);
		float colorLevel = (position.z() + dimensions.z()) / (dimensions.z() * 2);
		Vector3 tmp_color = FindCorrectLevel(box->m_ColorLevels->m_Levels, box->m_ColorLevels->m_Colors, colorLevel);
		osg::Vec3 selectedColor(tmp_color.x(), tmp_color.y(), tmp_color.z());

		// Update the model with the calculated data.
		osg::Vec2 size(sprite->m_Size.x(), sprite->m_Size.y());
		osg::ref_ptr<osg::Geometry> drawable = CreateCloudSprite(selectedOpacity, selectedColor, size, sprite->m_Rotation);
		billboard->addDrawable(drawable.get(), position);
	}
	return billboard;
}

osg::Node* CreateMediumDetailGeometry(weather::clouds::CloudBox* box) {
	osg::Billboard* billboard = new osg::Billboard();
    billboard->setMode(osg::Billboard::POINT_ROT_EYE);

    int numSpritesToCreate = box->m_Sprites.size() / 10;
    float distanceBetweenSprites = box->m_Size.x()*2 / static_cast<float>(numSpritesToCreate + 1);
    float xPosition = 0.0 - box->m_Size.x() + distanceBetweenSprites;

    for(int i = 0;i < numSpritesToCreate;++i) {
    	osg::Vec3 position(xPosition, 0.0, 0.0);
    	billboard->addDrawable(
			CreateCloudSprite(
				1.0, osg::Vec3(1.0, 1.0, 1.0),
				osg::Vec2(box->m_Size.x()*2, box->m_Size.x()*2), 0.0f
				), position);
    }

	return billboard;
}

osg::Node* CreateLowDetailGeometry(weather::clouds::CloudBox* box) {
	osg::Billboard* billboard = new osg::Billboard();
    billboard->setMode(osg::Billboard::POINT_ROT_EYE);
	billboard->addDrawable(
		CreateCloudSprite(
			1.0, osg::Vec3(1.0, 1.0, 1.0),
			osg::Vec2(box->m_Size.x()*2, box->m_Size.x()*2), 0.0f));
	return billboard;
}

void CloudColorLevels::addLevel(float level, Vector3 color) {
	m_Levels.push_back(level);
	m_Colors.push_back(color);
}

void CloudOpacityLevels::addLevel(float level, float opacity) {
	m_Levels.push_back(level);
	m_Opacity.push_back(opacity);
}

CloudBox::CloudBox() {}
CloudBox::~CloudBox() {}

void CloudBox::postCreate() {
}

osg::LOD* CloudBox::getNode() {
	if(!m_Node.valid()) {
		m_Node = new osg::LOD;
		m_Node->addChild(CreateHighDetailGeometry(this), 0.0, m_LOD->m_Distance1);
		m_Node->addChild(CreateMediumDetailGeometry(this), m_LOD->m_Distance1, m_LOD->m_Distance2);
		m_Node->addChild(CreateLowDetailGeometry(this), m_LOD->m_Distance2, m_LOD->m_Distance3);
	}
	return m_Node.get();
}

CloudModel::CloudModel() {}
CloudModel::~CloudModel() {}

void CloudModel::postCreate() {
}

osg::Group* CloudModel::getNode() {
	if(!m_Node.valid()) {
		assert(m_CloudBoxes.size() == m_CloudBoxPositions.size());

		m_Node = new osg::Group;
		osg::ref_ptr<osg::StateSet> stateset = m_Node->getOrCreateStateSet();
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
		stateset->setAttributeAndModes(blendFunction.get());
		osg::ref_ptr<osg::AlphaFunc> alphafunc = new osg::AlphaFunc;
		alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
		stateset->setAttributeAndModes(alphafunc.get(), osg::StateAttribute::ON);

		for(Link<CloudBox>::vector::size_type index = 0;index < m_CloudBoxes.size();++index) {
			/* Create a translate node and add the box to that one. */
			osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
			transform->setMatrix(osg::Matrix::translate(toOSG(m_CloudBoxPositions.at(index))));
			transform->addChild(m_CloudBoxes.at(index)->getNode());
			m_Node->addChild(transform.get());
		}
	}
	return m_Node.get();
}

}	// end namespace clouds
} 	// end namespace weather
}	// end namespace csp
