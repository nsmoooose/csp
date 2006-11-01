// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Default.cpp
 *
 **/

#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/Point.h>
#include <csp/cspsim/wf/Size.h>
#include <csp/cspsim/wf/Window.h>
#include <csp/cspsim/wf/themes/Default.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgText/Text>

CSP_NAMESPACE

namespace wf {
namespace themes {

Default::Default() {
}

Default::~Default() {
}

osg::Geometry* Default::BuildRectangle(
	float x1, float y1, float x2, float y2, float z,
	const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3, const osg::Vec4& c4) const
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x2, z, y1));//top right
	vertices->push_back(osg::Vec3(x2, z, y2));//bottom right
	vertices->push_back(osg::Vec3(x1, z, y2));//bottom left
	vertices->push_back(osg::Vec3(x1, z, y1));//top left
	geom->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(c1);
	colors->push_back(c2);
	colors->push_back(c3);
	colors->push_back(c4);
	
	geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	// setup state
	osg::StateSet* stateset = geom->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->setStateSet(stateset);

	return geom.release();
}

osg::Geometry* Default::buildRightShadow(float x1, float y1, float x2, float y2, float z, const osg::Vec4& color, float shadowSpace, float shadowSize) const {
    const float shadow_space = getShadowSpace();
    const float shadow_size = getShadowSize();

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x2, z, y1-shadow_space));
	vertices->push_back(osg::Vec3(x2, z, y2));
	vertices->push_back(osg::Vec3(x2 + shadow_size, z, y2-shadow_size));
	vertices->push_back(osg::Vec3(x2 + shadow_size, z, y1-shadow_space));
	geom->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    float alpha = color.w() * 0.5f;
    const osg::Vec3 black(0, 0, 0);

	colors->push_back(osg::Vec4(black, alpha));
	colors->push_back(osg::Vec4(black, alpha));
	colors->push_back(osg::Vec4(black, 0));
	colors->push_back(osg::Vec4(black, 0));

	geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	return geom.release();
}

osg::Geometry* Default::buildBottomShadow(float x1, float y1, float x2, float y2, float z, const osg::Vec4& color, float shadowSpace, float shadowSize) const {
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x1 + shadowSpace, z, y2-shadowSize));
	vertices->push_back(osg::Vec3(x2 + shadowSize, z, y2-shadowSize));
	vertices->push_back(osg::Vec3(x2, z, y2));
	vertices->push_back(osg::Vec3(x1 + shadowSpace, z, y2));
	geom->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    float alpha = color.w() * 0.5f;
    const osg::Vec3 black(0, 0, 0);

	colors->push_back(osg::Vec4(black, 0));
	colors->push_back(osg::Vec4(black, 0));
	colors->push_back(osg::Vec4(black, alpha));
	colors->push_back(osg::Vec4(black, alpha));

	geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	return geom.release();
}

osg::Group* Default::buildWindow(const Window& window) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	// Fetch all necesarry data that we need.
	const Size& size = window.getSize();
	const float borderWidth = getBorderWidth();
	const float captionHeight = getCaptionHeight();
	const osg::Vec4 windowBgColor = window.getBackgroundColor();

	// Build up the background of the entire window.
	float x1 = 0 - (size.m_W/2);
	float y1 = size.m_H/2;
	float x2 = (size.m_W/2);
	float y2 = 0 - size.m_H/2;
	{
		osg::ref_ptr<osg::Geometry> geometry = BuildRectangle(
			x1, y1, x2, y2, 0.0f,
			windowBgColor, windowBgColor, windowBgColor, windowBgColor);
		geode->addDrawable(geometry.get());
	}
	
	geode->addDrawable(buildRightShadow(x1, y1, x2, y2, 0.0f, windowBgColor, getShadowSpace(), getShadowSize()));
	geode->addDrawable(buildBottomShadow(x1, y1, x2, y2, 0.0f, windowBgColor, getShadowSpace(), getShadowSize()));
	
	// Background of the window name
	float caption_x1 = x1 + borderWidth;
	float caption_y1 = y1 - borderWidth;
	float caption_x2 = x2 - borderWidth;
	float caption_y2 = y1 - borderWidth - captionHeight;
	{
		osg::Vec4 captionBackground(0,0,0,1);
		osg::Vec4 captionBackground2(0,0,0,0.3);
		osg::ref_ptr<osg::Geometry> geometry = BuildRectangle(caption_x1, caption_y1, caption_x2, caption_y2, -0.1f,
			captionBackground2, captionBackground2, captionBackground, captionBackground);
		geode->addDrawable(geometry.get());
	}

	// Text of the window
	{
		osg::ref_ptr<osgText::Text> caption_text = new osgText::Text;
		caption_text->setText(window.getCaption());
		caption_text->setColor(osg::Vec4(1, 1, 1, 1));
		caption_text->setAlignment(osgText::Text::CENTER_CENTER);
		caption_text->setFont(getCaptionFont().c_str());
		caption_text->setCharacterSize(5);
		caption_text->setFontResolution(30, 30);
		caption_text->setAxisAlignment(osgText::Text::SCREEN);
		caption_text->setPosition(osg::Vec3(0, -0.2f, caption_y1 - (captionHeight/2)));
		geode->addDrawable(caption_text.get());
	}
	
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());

	return group.release();
}

osg::Group* Default::buildButton(const Button& button) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	// Fetch all necesarry data that we need.
	const Size& size = button.getSize();
	const osg::Vec4 buttonColor(0.0f, 0.0f, 0.0f, 0.5f);

	float x1 = 0 - (size.m_W/2);
	float y1 = size.m_H/2;
	float x2 = (size.m_W/2);
	float y2 = 0 - size.m_H/2;
	{
		osg::ref_ptr<osg::Geometry> geometry = BuildRectangle(
			x1, y1, x2, y2, 0.0f,
			buttonColor, buttonColor, buttonColor, buttonColor);
		geode->addDrawable(geometry.get());
	}
	
	geode->addDrawable(buildRightShadow(x1, y1, x2, y2, 0.0f, buttonColor, getShadowSpace(), getShadowSize()));
	geode->addDrawable(buildBottomShadow(x1, y1, x2, y2, 0.0f, buttonColor, getShadowSpace(), getShadowSize()));

    osg::ref_ptr<osgText::Text> button_text = new osgText::Text;
    button_text->setText(button.getText());
    button_text->setColor(osg::Vec4(1, 1, 1, 1));
    button_text->setAlignment(osgText::Text::CENTER_CENTER);
    button_text->setFont("screeninfo.ttf");
    button_text->setCharacterSize(5);
    button_text->setFontResolution(30, 30);
	button_text->setAxisAlignment(osgText::Text::SCREEN);
    button_text->setPosition(osg::Vec3(0, -0.1f, 0));
    geode->addDrawable(button_text.get());
	
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());

	return group.release();
}

float Default::getBorderWidth() const {
	return 1.0f;
}

float Default::getCaptionHeight() const {
	return 7.0f;
}

float Default::getShadowSpace() const {
	return 1.0f;
}

float Default::getShadowSize() const {
	return 2.0f;
}

std::string Default::getCaptionFont() const {
	return "screeninfo.ttf";
}

} // namespace themes
} // namespace wf

CSP_NAMESPACE_END
