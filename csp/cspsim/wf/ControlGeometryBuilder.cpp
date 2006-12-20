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
 * @file ControlGeometryBuilder.cpp
 *
 **/

#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/ListBoxItem.h>
#include <csp/cspsim/wf/Point.h>
#include <csp/cspsim/wf/Size.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TabPage.h>
#include <csp/cspsim/wf/Window.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osgText/Text>

CSP_NAMESPACE

namespace wf {

ControlGeometryBuilder::ControlGeometryBuilder() {
}

ControlGeometryBuilder::~ControlGeometryBuilder() {
}

osg::Geometry* ControlGeometryBuilder::buildRectangle(
	float x1, float y1, float x2, float y2, float z,
	const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3, const osg::Vec4& c4) const {
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

osg::Geometry* ControlGeometryBuilder::buildRectangle(
	float x1, float y1, float x2, float y2, float z, float lineWidth,
	const osg::Vec4& outerColor, const osg::Vec4& innerColor,
	bool left, bool top, bool right, bool bottom) const {

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	int verticeCount = 0;

	// Left rectangle
	if(left) {
		vertices->push_back(osg::Vec3(x1 + lineWidth, z, y1 - (top ? lineWidth : 0.0f)));//top right
		vertices->push_back(osg::Vec3(x1 + lineWidth, z, y2 + (bottom ? lineWidth : 0.0f)));//bottom right
		vertices->push_back(osg::Vec3(x1, z, y2));//bottom left
		vertices->push_back(osg::Vec3(x1, z, y1));//top left
		verticeCount+=4;
		colors->push_back(innerColor);
		colors->push_back(innerColor);
		colors->push_back(outerColor);
		colors->push_back(outerColor);
	}

	// Top rectangle
	if(top) {
		vertices->push_back(osg::Vec3(x2, z, y1));//top right
		vertices->push_back(osg::Vec3(x2 - (right ? lineWidth : 0.0f), z, y1 - lineWidth));//bottom right
		vertices->push_back(osg::Vec3(x1 + (left ? lineWidth : 0.0f), z, y1 - lineWidth));//bottom left
		vertices->push_back(osg::Vec3(x1, z, y1));//top left
		verticeCount+=4;
		colors->push_back(outerColor);
		colors->push_back(innerColor);
		colors->push_back(innerColor);
		colors->push_back(outerColor);
	}
		
	// Right rectangle
	if(right) {
		vertices->push_back(osg::Vec3(x2, z, y1));//top right
		vertices->push_back(osg::Vec3(x2, z, y2));//bottom right
		vertices->push_back(osg::Vec3(x2 - lineWidth, z, y2 + (bottom ? lineWidth : 0.0f)));//bottom left
		vertices->push_back(osg::Vec3(x2 - lineWidth, z, y1 - (top ? lineWidth : 0.0f)));//top left
		verticeCount+=4;
		colors->push_back(outerColor);
		colors->push_back(outerColor);
		colors->push_back(innerColor);
		colors->push_back(innerColor);
	}

	// Bottom rectangle
	if(bottom) {
		vertices->push_back(osg::Vec3(x2 - (right ? lineWidth : 0.0f), z, y2 + lineWidth));//top right
		vertices->push_back(osg::Vec3(x2, z, y2));//bottom right
		vertices->push_back(osg::Vec3(x1, z, y2));//bottom left
		vertices->push_back(osg::Vec3(x1 + (left ? lineWidth : 0.0f), z, y2 + lineWidth));//top left
		verticeCount+=4;
		colors->push_back(innerColor);
		colors->push_back(outerColor);
		colors->push_back(outerColor);
		colors->push_back(innerColor);
	}

	geom->setVertexArray(vertices.get());
	geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, verticeCount));

	// setup state
	osg::StateSet* stateset = geom->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->setStateSet(stateset);

	return geom.release();
}

osg::Geode* ControlGeometryBuilder::buildStar() const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	
//	osg::Vec4 color1 = osg::Vec4(0.5f, 0.0f, 0.0f, 0.0f);
//	osg::Vec4 color2 = osg::Vec4(0.5f, 0.0f, 0.0f, 1.0f);
	osg::Vec4 color1 = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	osg::Vec4 color2 = osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	const double halfHeight = m_Colors.checkBoxHeight / 2;
	const double thirdHeight = halfHeight / 3;

	// Top arrow. Left triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, halfHeight), // Top
			Point(0.0f - thirdHeight, thirdHeight), // Left 
			Point(0.0f, 0.0f), // bottom
			-0.2f, color1, color1, color2));

	// Left arrow. Top triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f - halfHeight, 0.0f), // Left 
			Point(0.0f - thirdHeight, thirdHeight), // top
			-0.2f, color2, color1, color1));

	// Left arrow. Bottom triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f - halfHeight, 0.0f), // Left 
			Point(0.0f - thirdHeight, 0.0f - thirdHeight), // bottom
			-0.2f, color2, color1, color1));

	// Bottom arrow. Left triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f, 0.0f - halfHeight), // Left 
			Point(0.0f - thirdHeight, 0.0f - thirdHeight), // bottom
			-0.2f, color2, color1, color1));

	// Bottom arrow. Right triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f, 0.0f - halfHeight), // Left 
			Point(thirdHeight, 0.0f - thirdHeight), // bottom
			-0.2f, color2, color1, color1));

	// Right arrow. Bottom triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(halfHeight, 0.0f), // Left 
			Point(thirdHeight, 0.0f - thirdHeight), // bottom
			-0.2f, color2, color1, color1));

	// Right arrow. Top triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(halfHeight, 0.0f), // Left 
			Point(thirdHeight, thirdHeight), // top
			-0.2f, color2, color1, color1));

	// Top arrow. Right triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f, halfHeight), // Left 
			Point(thirdHeight, thirdHeight), // bottom
			-0.2f, color2, color1, color1));
	return geode.release();
}

osg::Geometry* ControlGeometryBuilder::buildTriangle(
	const Point& p1, const Point& p2, const Point& p3, float z,
	const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3) const {

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	vertices->push_back(osg::Vec3(p1.m_X, z, p1.m_Y));
	vertices->push_back(osg::Vec3(p2.m_X, z, p2.m_Y));
	vertices->push_back(osg::Vec3(p3.m_X, z, p3.m_Y));
	colors->push_back(c1);
	colors->push_back(c2);
	colors->push_back(c3);

	geom->setVertexArray(vertices.get());
	geom->setColorArray(colors.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 3));

	// setup state
	osg::StateSet* stateset = geom->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->setStateSet(stateset);

	return geom.release();
}

osg::Group* ControlGeometryBuilder::buildTab(const Tab* tab) const {
	osg::ref_ptr<osg::Group> group = new osg::Group;

	// Build up the background of the entire tab.
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	const Size& size = tab->getSize();
	float x1 = 0 - (size.m_W/2);
	float y1 = (size.m_H / 2) - m_Colors.tabButtonHeight;
	float x2 = (size.m_W / 2);
	float y2 = 0 - size.m_H / 2;
	{
		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, -0.1f,
			m_Colors.tabPageBackgroundColor1, m_Colors.tabPageBackgroundColor2, 
			m_Colors.tabPageBackgroundColor2, m_Colors.tabPageBackgroundColor1);
		geode->addDrawable(geometry.get());
	}

	// Add a border to the button.
	geode->addDrawable(buildRectangle(x1, y1, x2, y2, -0.2, m_Colors.tabBorderWidth,
		m_Colors.tabBorderColor1, m_Colors.tabBorderColor2,
		true, false, true, true));

	group->addChild(geode.get());

	return group.release();
}

osg::Switch* ControlGeometryBuilder::buildTabButton(const Tab* tab, const TabPage* page, int index) const {
	osg::ref_ptr<osg::Switch> group = new osg::Switch;

	// Test to see if this button should be visible or not.
	bool currentPage = (page == tab->getCurrentPage() ? true : false);

	osg::ref_ptr<osg::Geode> geodeCurrentPage = new osg::Geode;	
	{
		const double buttonWidth = 40.0f;
		const Size& size = tab->getSize();
		float x1 = 0 - (size.m_W/2) + (index * buttonWidth);
		float y1 = (size.m_H / 2);
		float x2 = x1 + buttonWidth;
		float y2 = y1 - m_Colors.tabButtonHeight;
		
		{
			osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
				x1, y1, x2, y2, -0.1f,
				m_Colors.tabButtonBackgroundColorCurrent, m_Colors.tabButtonBackgroundColorCurrent, 
				m_Colors.tabButtonBackgroundColorCurrent, m_Colors.tabButtonBackgroundColorCurrent);
			geodeCurrentPage->addDrawable(geometry.get());
		}
	
		// Add a border to the button.
		geodeCurrentPage->addDrawable(buildRectangle(x1, y1, x2, y2, -0.2, m_Colors.tabBorderWidth,
			m_Colors.tabBorderColor1, m_Colors.tabBorderColor2,
			true, true, true, false));
			
		// Text of the window
		{
			osg::ref_ptr<osgText::Text> tab_text = new osgText::Text;
			tab_text->setText(page->getText());
			tab_text->setColor(m_Colors.labelTextColor);
			tab_text->setAlignment(osgText::Text::CENTER_CENTER);
			tab_text->setFont(getCaptionFont().c_str());
			tab_text->setCharacterSize(5);
			tab_text->setFontResolution(100, 100);
			tab_text->setAxisAlignment(osgText::Text::SCREEN);
			tab_text->setPosition(osg::Vec3(x1 + buttonWidth / 2, -0.2f, y1 - (m_Colors.tabButtonHeight/2)));
			geodeCurrentPage->addDrawable(tab_text.get());
		}
	}

	osg::ref_ptr<osg::Geode> geodeNotCurrentPage = new osg::Geode;
	{
		const double buttonWidth = 40.0f;
		const Size& size = tab->getSize();
		float x1 = 0 - (size.m_W/2) + (index * buttonWidth);
		float y1 = (size.m_H / 2) - 1;
		float x2 = x1 + buttonWidth;
		float y2 = y1 - (m_Colors.tabButtonHeight - 1);
		
		{
			osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
				x1, y1, x2, y2, -0.1f,
				m_Colors.tabButtonBackgroundColorNotCurrent, m_Colors.tabButtonBackgroundColorNotCurrent, 
				m_Colors.tabButtonBackgroundColorNotCurrent, m_Colors.tabButtonBackgroundColorNotCurrent);
			geodeNotCurrentPage->addDrawable(geometry.get());
		}
	
		// Add a border to the button.
		geodeNotCurrentPage->addDrawable(buildRectangle(x1, y1, x2, y2, -0.2, m_Colors.tabBorderWidth,
			m_Colors.tabBorderColor1, m_Colors.tabBorderColor2,
			true, true, true, true));
			
		// Text of the window
		{
			osg::ref_ptr<osgText::Text> tab_text = new osgText::Text;
			tab_text->setText(page->getText());
			tab_text->setColor(m_Colors.labelTextColor);
			tab_text->setAlignment(osgText::Text::CENTER_CENTER);
			tab_text->setFont(getCaptionFont().c_str());
			tab_text->setCharacterSize(5);
			tab_text->setFontResolution(100, 100);
			tab_text->setAxisAlignment(osgText::Text::SCREEN);
			tab_text->setPosition(osg::Vec3(x1 + buttonWidth / 2, -0.2f, y1 - (m_Colors.tabButtonHeight/2)));
			geodeNotCurrentPage->addDrawable(tab_text.get());
		}
	}

	group->addChild(geodeCurrentPage.get(), currentPage);
	group->addChild(geodeNotCurrentPage.get(), !currentPage);

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildTabPage(const TabPage* /*page*/) const {
	return NULL;
}

osg::Group* ControlGeometryBuilder::buildWindow(const Window* window) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	// Fetch all necesarry data that we need.
	const Size& size = window->getSize();
	const float borderWidth = getBorderWidth();
	const float captionHeight = getCaptionHeight();

	// Build up the background of the entire window.
	float x1 = 0 - (size.m_W/2);
	float y1 = size.m_H / 2 - captionHeight;
	float x2 = (size.m_W / 2);
	float y2 = 0 - size.m_H / 2;
	{
		const osg::Vec4 windowBgColor = m_Colors.windowBackgroundColor;
		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, 0.0f,
			windowBgColor, windowBgColor, windowBgColor, windowBgColor);
		geode->addDrawable(geometry.get());
	}

	// Create a shaded border round the client area of the window.
	geode->addDrawable(buildRectangle(x1, y1, x2, y2, -0.1, borderWidth,
		m_Colors.windowBorderColor1, m_Colors.windowBorderColor2));

	// Background of the window name
	float caption_x1 = 0 - (size.m_W/2) - 3;
	float caption_y1 = size.m_H / 2;
	float caption_x2 = (size.m_W / 2) + 3;
	float caption_y2 = caption_y1 - captionHeight;
	{
		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(caption_x1, caption_y1, caption_x2, caption_y2, -0.1f,
			m_Colors.windowCaptionBackgroundColor2, m_Colors.windowCaptionBackgroundColor2, 
			m_Colors.windowCaptionBackgroundColor1, m_Colors.windowCaptionBackgroundColor1);
		geode->addDrawable(geometry.get());
		
		// Draw a border around the caption.
		geode->addDrawable(buildRectangle(caption_x1, caption_y1, caption_x2, caption_y2, -0.2, borderWidth,
			m_Colors.windowCaptionBorderColor1, m_Colors.windowCaptionBorderColor2));
	}

	// Text of the window
	{
		osg::ref_ptr<osgText::Text> caption_text = new osgText::Text;
		caption_text->setText(window->getCaption());
		caption_text->setColor(m_Colors.windowCaptionTextColor);
		caption_text->setAlignment(osgText::Text::CENTER_CENTER);
		caption_text->setFont(getCaptionFont().c_str());
		caption_text->setCharacterSize(5);
		caption_text->setFontResolution(100, 100);
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

osg::Group* ControlGeometryBuilder::buildButton(const Button* button) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	// Fetch all necesarry data that we need.
	const Size& size = button->getSize();

	float x1 = 0 - (size.m_W/2);
	float y1 = size.m_H/2;
	float x2 = (size.m_W/2);
	float y2 = 0 - size.m_H/2;

	// Draw the background of the button.
	osg::ref_ptr<osg::Geometry> geometry = buildRectangle(x1, y1, x2, y2, -0.1f,
		m_Colors.buttonBackgroundColor1, m_Colors.buttonBackgroundColor2, 
		m_Colors.buttonBackgroundColor2, m_Colors.buttonBackgroundColor1);
	geode->addDrawable(geometry.get());

	// Add a border to the button.
	geode->addDrawable(buildRectangle(x1, y1, x2, y2, -0.2, m_Colors.buttonBorderWidth,
		m_Colors.buttonBorderColor1, m_Colors.buttonBorderColor2));

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

osg::Group* ControlGeometryBuilder::buildCheckBox(const CheckBox* checkBox) const {
	const Size& size = checkBox->getSize();
	
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	float x1 = 0 - (size.m_W / 2);
	float y1 = m_Colors.checkBoxHeight / 2;
	float x2 = x1 + m_Colors.checkBoxHeight;
	float y2 = 0 - (m_Colors.checkBoxHeight / 2);

	// Draw a border around the checkbox.
	geode->addDrawable(buildRectangle(x1, y1, x2, y2, -0.3, m_Colors.buttonBorderWidth,
		m_Colors.buttonBorderColor1, m_Colors.buttonBorderColor2));	
	geode->addDrawable(buildRectangle(x1, y1, x2, y2, -0.2f,
		m_Colors.notSelectedItemColor1, m_Colors.notSelectedItemColor2, 
		m_Colors.notSelectedItemColor2, m_Colors.notSelectedItemColor1));

	osg::ref_ptr<osgText::Text> button_text = new osgText::Text;
	button_text->setText(checkBox->getText());
	button_text->setColor(m_Colors.labelTextColor);
	button_text->setAlignment(osgText::Text::LEFT_CENTER);
	button_text->setFont("screeninfo.ttf");
	button_text->setCharacterSize(5);
	button_text->setFontResolution(30, 30);
	button_text->setAxisAlignment(osgText::Text::SCREEN);
	button_text->setPosition(osg::Vec3(x2 + 1.0f, -0.2f, 0));
	geode->addDrawable(button_text.get());

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());

	if(checkBox->getChecked()) {
		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(osg::Matrix::translate(x1 + (m_Colors.checkBoxHeight / 2.0f), -0.2f, 0));
		transform->addChild(buildStar());
		group->addChild(transform.get());
	}
	return group.release();
}

osg::Group* ControlGeometryBuilder::buildLabel(const Label* label) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    osg::ref_ptr<osgText::Text> button_text = new osgText::Text;
    button_text->setText(label->getText());
    button_text->setColor(m_Colors.labelTextColor);
    button_text->setAlignment(label->getAlignment());
    button_text->setFont("screeninfo.ttf");
    button_text->setCharacterSize(5);
    button_text->setFontResolution(30, 30);
	button_text->setAxisAlignment(osgText::Text::SCREEN);
	switch(label->getAlignment()) {
		case osgText::Text::LEFT_TOP:
		case osgText::Text::LEFT_CENTER:
			button_text->setPosition(osg::Vec3(0 - (label->getSize().m_W / 2), -0.2f, 0));
			break;
		case osgText::Text::LEFT_BOTTOM:
		case osgText::Text::CENTER_TOP:
		case osgText::Text::CENTER_CENTER:
			button_text->setPosition(osg::Vec3(0, -0.2f, 0));
			break;
		case osgText::Text::CENTER_BOTTOM:
		case osgText::Text::RIGHT_TOP:
		case osgText::Text::RIGHT_CENTER:
		case osgText::Text::RIGHT_BOTTOM:
		default:
			button_text->setPosition(osg::Vec3(0, -0.2f, 0));
			break;
	}
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

osg::Group* ControlGeometryBuilder::buildListBox(const ListBox* listBox) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	// Fetch all necesarry data that we need.
	const Size& size = listBox->getSize();

	float x1 = 0 - (size.m_W/2);
	float y1 = size.m_H / 2;
	float x2 = (size.m_W / 2);
	float y2 = 0 - (size.m_H/2);

	// Add a border to the button.
	geode->addDrawable(buildRectangle(x1, y1, x2, y2, -0.2, m_Colors.buttonBorderWidth,
		m_Colors.buttonBorderColor1, m_Colors.buttonBorderColor2));

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	return group.release();
}

osg::Switch* ControlGeometryBuilder::buildListBoxItem(const ListBox* listBox, const ListBoxItem* listBoxItem) const {
	const Size& size = listBoxItem->getSize();
	float x1 = 0 - (size.m_W/2);
	float y1 = size.m_H / 2;
	float x2 = (size.m_W / 2);
	float y2 = 0 - (size.m_H/2);

	osg::ref_ptr<osg::Geode> itemGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry = buildRectangle(x1, y1, x2, y2, -0.1f,
		m_Colors.notSelectedItemColor1, m_Colors.notSelectedItemColor2, 
		m_Colors.notSelectedItemColor2, m_Colors.notSelectedItemColor1);
	itemGeode->addDrawable(geometry.get());
	
	// This geode represents the geometry that is displayed when the item is selected.
	osg::ref_ptr<osg::Geode> selectedGeode = new osg::Geode;
	{
		// Draw the background of the button.
		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(x1, y1, x2, y2, -0.2f,
			m_Colors.selectedItemColor, m_Colors.selectedItemColor, 
			m_Colors.selectedItemColor, m_Colors.selectedItemColor);
		selectedGeode->addDrawable(geometry.get());
	}

	osg::ref_ptr<osg::Switch> group = new osg::Switch;
	group->addChild(itemGeode.get(), true);
	group->addChild(selectedGeode.get(), listBox->getSelectedItem() == listBoxItem);

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	return group.release();
}

Size ControlGeometryBuilder::getWindowClientAreaSize(const Window* window) const {
	Size windowSize = window->getSize();
	return Size(windowSize.m_W - getBorderWidth(), windowSize.m_H - getBorderWidth() - getCaptionHeight());
}

Point ControlGeometryBuilder::getWindowClientAreaLocation(const Window* /*window*/) const {
	return Point(0, 0 - (getCaptionHeight() / 2));
}

Size ControlGeometryBuilder::getTabPageClientAreaSize(const Tab* tab) const {
	Size tabSize = tab->getSize();
	return Size(tabSize.m_W, tabSize.m_H - m_Colors.tabButtonHeight);
}

Point ControlGeometryBuilder::getTabPageClientAreaLocation(const Tab* /*tab*/) const {
	return Point(0.0f, 0.0f - (m_Colors.tabButtonHeight / 2));
}

float ControlGeometryBuilder::getBorderWidth() const {
	return 1.0f;
}

float ControlGeometryBuilder::getCaptionHeight() const {
	return 7.0f;
}

std::string ControlGeometryBuilder::getCaptionFont() const {
	return "screeninfo.ttf";
}

} // namespace wf

CSP_NAMESPACE_END
