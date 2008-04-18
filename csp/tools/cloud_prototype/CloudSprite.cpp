#include "CloudSprite.h"
#include "CloudTextureFactory.h"

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

namespace csp {
namespace clouds {

CloudSprite::CloudSprite(void) {
}

CloudSprite::~CloudSprite(void) {
}

void CloudSprite::UpdateModel(const osg::Vec3& position, const float& alpha, const osg::Vec3& color) {
	osg::Vec3 corner(-0.5f, 0.0f, -0.5f);
	osg::Vec3 width(5.0f, 0.0f, 0.0f);
	osg::Vec3 height(0.0f, 0.0f, 5.0f);
	osg::Vec4 colorWithAlpha(color, alpha);

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(corner);
	vertices->push_back(corner + width);
	vertices->push_back(corner + width + height);
	vertices->push_back(corner + height);
	geometry->setVertexArray(vertices.get());

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

//    setMode(osg::Billboard::POINT_ROT_EYE);
    setMode(osg::Billboard::POINT_ROT_WORLD);
    addDrawable(geometry.get());

	setPosition(0, position);
}

} // end namespace clouds
} // end namespace csp
