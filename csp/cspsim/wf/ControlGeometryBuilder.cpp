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

#include <csp/csplib/util/FileUtility.h>

#include <csp/cspsim/Config.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Image.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/ListBoxItem.h>
#include <csp/cspsim/wf/Model.h>
#include <csp/cspsim/wf/Point.h>
#include <csp/cspsim/wf/ResourceLocator.h>
#include <csp/cspsim/wf/Size.h>
#include <csp/cspsim/wf/StyleBuilder.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TabPage.h>
#include <csp/cspsim/wf/Window.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgText/Text>

CSP_NAMESPACE

namespace wf {

ControlGeometryBuilder::ControlGeometryBuilder() {
}

ControlGeometryBuilder::~ControlGeometryBuilder() {
}

void ControlGeometryBuilder::getNextLayer(float& z) const {
	z += 0.1f;
}

osgText::Text* ControlGeometryBuilder::buildText(const std::string& text, const std::string& fontFamily, float fontSize, osg::Vec4& color) const {
    osg::ref_ptr<osgText::Text> textNode = new osgText::Text;
    textNode->setText(text);
    textNode->setColor(color);
    textNode->setFont(fontFamily.c_str());
    textNode->setCharacterSize(fontSize);
	textNode->setAxisAlignment(osgText::Text::XY_PLANE);
	textNode->setRotation(osg::Quat(PI/2, 0, 0, 0));
	return textNode.release();
}

osg::Geometry* ControlGeometryBuilder::buildRectangle(
	float x1, float y1, float x2, float y2, float z,
	const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3, const osg::Vec4& c4) const {
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x2, y1, z));//top right
	vertices->push_back(osg::Vec3(x2, y2, z));//bottom right
	vertices->push_back(osg::Vec3(x1, y2, z));//bottom left
	vertices->push_back(osg::Vec3(x1, y1, z));//top left
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
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
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
	if (left) {
		vertices->push_back(osg::Vec3(x1 + lineWidth, y1 + (top ? lineWidth : 0.0f), z));//top right
		vertices->push_back(osg::Vec3(x1 + lineWidth, y2 - (bottom ? lineWidth : 0.0f), z));//bottom right
		vertices->push_back(osg::Vec3(x1, y2, z));//bottom left
		vertices->push_back(osg::Vec3(x1, y1, z));//top left
		verticeCount+=4;
		colors->push_back(innerColor);
		colors->push_back(innerColor);
		colors->push_back(outerColor);
		colors->push_back(outerColor);
	}

	// Top rectangle
	if (top) {
		vertices->push_back(osg::Vec3(x2, y1, z));//top right
		vertices->push_back(osg::Vec3(x2 - (right ? lineWidth : 0.0f), y1 + lineWidth, z));//bottom right
		vertices->push_back(osg::Vec3(x1 + (left ? lineWidth : 0.0f), y1 + lineWidth, z));//bottom left
		vertices->push_back(osg::Vec3(x1, y1, z));//top left
		verticeCount+=4;
		colors->push_back(outerColor);
		colors->push_back(innerColor);
		colors->push_back(innerColor);
		colors->push_back(outerColor);
	}

	// Right rectangle
	if (right) {
		vertices->push_back(osg::Vec3(x2, y1, z));//top right
		vertices->push_back(osg::Vec3(x2, y2, z));//bottom right
		vertices->push_back(osg::Vec3(x2 - lineWidth, y2 - (bottom ? lineWidth : 0.0f), z));//bottom left
		vertices->push_back(osg::Vec3(x2 - lineWidth, y1 + (top ? lineWidth : 0.0f), z));//top left
		verticeCount+=4;
		colors->push_back(outerColor);
		colors->push_back(outerColor);
		colors->push_back(innerColor);
		colors->push_back(innerColor);
	}

	// Bottom rectangle
	if (bottom) {
		vertices->push_back(osg::Vec3(x2 - (right ? lineWidth : 0.0f), y2 - lineWidth, z));//top right
		vertices->push_back(osg::Vec3(x2, y2, z));//bottom right
		vertices->push_back(osg::Vec3(x1, y2, z));//bottom left
		vertices->push_back(osg::Vec3(x1 + (left ? lineWidth : 0.0f), y2 - lineWidth, z));//top left
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
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	geom->setStateSet(stateset);

	return geom.release();
}

void ControlGeometryBuilder::buildControl(osg::Geode* geode, float& z, const Style& style, const Control* control) const {
	// If the control isn't visible then we dont create any geometry at all.
	if (style.visible && *style.visible == false) {
		return;
	}

	const Size& size = control->getSize();

	float x1 = 0 - (size.width/2);
	float y1 = 0 - size.height/2;
	float x2 = (size.width/2);
	float y2 = size.height/2;

	// Draw background color of the control.
	osg::ref_ptr<osg::Geometry> backgroundGeometry;
	if (style.backgroundColor) {
		getNextLayer(z);
		// Draw the background of the control.
		// First priority is to use backgroundColorXxxYyyy if a value exists. Otherwise we use
		// just backgroundColor style.
		backgroundGeometry = buildRectangle(x1, y1, x2, y2, z,
			!style.backgroundColorTopRight ? *style.backgroundColor : *style.backgroundColorTopRight,
			!style.backgroundColorBottomRight ? *style.backgroundColor : *style.backgroundColorBottomRight,
			!style.backgroundColorBottomLeft ? *style.backgroundColor : *style.backgroundColorBottomLeft,
			!style.backgroundColorTopLeft ? *style.backgroundColor : *style.backgroundColorTopLeft);
		geode->addDrawable(backgroundGeometry.get());

	}

	// Test to see if we should have a background image to this control. In that case we
	// must load a texture and set it on background geometry.
	if (style.backgroundImage) {
		// If we have a background color set then we already have a background
		// geometry to use. In any other case we must add a rectangle that we use for
		// the texture we are going to load.
		if (!backgroundGeometry.valid()) {
			getNextLayer(z);
			backgroundGeometry = buildRectangle(x1, y1, x2, y2, z,
				osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f), 	osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
				osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f), 	osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
			geode->addDrawable(backgroundGeometry.get());
		}

		// The control needs to be attached to a window in order to load
		// textures. All textures belongs to the current theme.
		const Window* window = Window::getWindow(control);
		if (window != NULL) {
			// Add texture coordinates to our geometry.
			osg::ref_ptr<osg::Vec2Array> textureCoordinates = new osg::Vec2Array(4);
			textureCoordinates->at(3).set(0.0f, 1.0f);
			textureCoordinates->at(2).set(0.0f, 0.0f);
			textureCoordinates->at(1).set(1.0f, 0.0f);
			textureCoordinates->at(0).set(1.0f, 1.0f);
			backgroundGeometry->setTexCoordArray(0, textureCoordinates.get());

			osg::ref_ptr<osg::Image> image;

			// Try to locate the resource using a resource locator class.
			// The resource can be located in the theme directory or in
			// the datapath.
			Ref<ResourceLocator> resourceLocator = createDefaultResourceLocator();
			std::string filePath = *style.backgroundImage;
			if (resourceLocator->locateResource(filePath)) {
				image = osgDB::readImageFile(filePath);
			}

			// If we succeeded with loading of the texture then we
			// set it so it is displayed.
			if (image.valid()) {
				// Create a texture and assign the image to it.
				osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
				texture->setImage(image.get());

				osg::ref_ptr<osg::StateSet> stateset = backgroundGeometry->getOrCreateStateSet();
				stateset->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
				stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				backgroundGeometry->setStateSet(stateset.get());
			}
		}
	}

	// Increase the layer so the borders will be placed over the background.
	getNextLayer(z);

	// Draw top border.
	if ((style.borderWidth || style.borderTopWidth) && (style.borderColor || style.borderTopColor)) {
		float borderWidth = style.borderTopWidth ? *style.borderTopWidth : *style.borderWidth;
		osg::Vec4 borderColor = style.borderTopColor ? *style.borderTopColor : *style.borderColor;

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			false, true, false, false);
		geode->addDrawable(geometry.get());
	}

	// Draw bottom border.
	if ((style.borderWidth || style.borderBottomWidth) && (style.borderColor || style.borderBottomColor)) {
		float borderWidth = style.borderBottomWidth ? *style.borderBottomWidth : *style.borderWidth;
		osg::Vec4 borderColor = style.borderBottomColor ? *style.borderBottomColor : *style.borderColor;

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			false, false, false, true);
		geode->addDrawable(geometry.get());
	}

	// Draw left border.
	if ((style.borderWidth || style.borderLeftWidth) && (style.borderColor || style.borderLeftColor)) {
		float borderWidth = style.borderLeftWidth ? *style.borderLeftWidth : *style.borderWidth;
		osg::Vec4 borderColor = style.borderLeftColor ? *style.borderLeftColor : *style.borderColor;

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			true, false, false, false);
		geode->addDrawable(geometry.get());
	}

	// Draw right border.
	if ((style.borderWidth || style.borderRightWidth) && (style.borderColor || style.borderRightColor)) {
		float borderWidth = style.borderRightWidth ? *style.borderRightWidth : *style.borderWidth;
		osg::Vec4 borderColor = style.borderRightColor ? *style.borderRightColor : *style.borderColor;

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			false, false, true, false);
		geode->addDrawable(geometry.get());
	}
}

osg::Geode* ControlGeometryBuilder::buildStar(float heightAndWidth, float z, osg::Vec4& color1, osg::Vec4& color2) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	const double halfHeight = heightAndWidth / 2;
	const double thirdHeight = halfHeight / 6;

	// Top arrow. Left triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, halfHeight), // Top
			Point(0.0f - thirdHeight, thirdHeight), // Left
			Point(0.0f, 0.0f), // bottom
			z, color1, color1, color2));

	// Left arrow. Top triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f - halfHeight, 0.0f), // Left
			Point(0.0f - thirdHeight, thirdHeight), // top
			z, color2, color1, color1));

	// Left arrow. Bottom triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f - halfHeight, 0.0f), // Left
			Point(0.0f - thirdHeight, 0.0f - thirdHeight), // bottom
			z, color2, color1, color1));

	// Bottom arrow. Left triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f, 0.0f - halfHeight), // Left
			Point(0.0f - thirdHeight, 0.0f - thirdHeight), // bottom
			z, color2, color1, color1));

	// Bottom arrow. Right triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f, 0.0f - halfHeight), // Left
			Point(thirdHeight, 0.0f - thirdHeight), // bottom
			z, color2, color1, color1));

	// Right arrow. Bottom triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(halfHeight, 0.0f), // Left
			Point(thirdHeight, 0.0f - thirdHeight), // bottom
			z, color2, color1, color1));

	// Right arrow. Top triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(halfHeight, 0.0f), // Left
			Point(thirdHeight, thirdHeight), // top
			z, color2, color1, color1));

	// Top arrow. Right triangle.
	geode->addDrawable(
		buildTriangle(
			Point(0.0f, 0.0f), // Right
			Point(0.0f, halfHeight), // Left
			Point(thirdHeight, thirdHeight), // bottom
			z, color2, color1, color1));

	return geode.release();
}

osg::Geometry* ControlGeometryBuilder::buildTriangle(
	const Point& p1, const Point& p2, const Point& p3, float z,
	const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3) const {

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	vertices->push_back(osg::Vec3(p1.x, p1.y, z));
	vertices->push_back(osg::Vec3(p2.x, p2.y, z));
	vertices->push_back(osg::Vec3(p3.x, p3.y, z));
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
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	geom->setStateSet(stateset);

	return geom.release();
}

osg::Group* ControlGeometryBuilder::buildTab(const Tab* tab) const {
	osg::ref_ptr<osg::Group> group = new osg::Group;

	// Build up the background of the entire tab.
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;



	/*

		En tab kontrol består av ett flertal kontroller.

		Hela kontrollen med allt innehåll Tab

		Raden med flikar som kan vara horizontell eller vertikal. TabPageContainer

		Själva sidan som varje flik motsvarar. TabPage. Detta är en container klass och går inte att byta ut.

		Själva knappen som man klickar på för att komma till en sida. TabPageButton -> SingleControlContainer. Denna går att byta ut vid behov.


		Problem?
		Hur skall man kunna styla allt detta? Man kan sätta olika styles på Tab kontrollen? Eller på enskilda
	*/

	float z = 0;

	Style style = StyleBuilder::buildStyle(tab);
	buildControl(geode.get(), z, style, tab);


	group->addChild(geode.get());

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildTabButton(const Tab* /*tab*/, const TabPage* /*page*/) const {
	osg::ref_ptr<osg::Group> group = new osg::Group;
	return group.release();
}

osg::Group* ControlGeometryBuilder::buildTabPage(const TabPage* /*page*/) const {
	osg::ref_ptr<osg::Group> group = new osg::Group;
	return group.release();
}

osg::Group* ControlGeometryBuilder::buildWindow(const Window* window) const {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	float z = 0;

	Style style = StyleBuilder::buildStyle(window);
	buildControl(geode.get(), z, style, window);

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
	// Build all generic geometry like background, border, colors etc.
	Style style = StyleBuilder::buildStyle(button);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	// Add all default control behaviours like borders, background color, image etc.
	buildControl(geode.get(), z, style, button);

	Ref<const Window> window = Window::getWindow(button);
	if(style.fontFamily && style.fontSize && style.color && window.valid()) {
		getNextLayer(z);
		
		Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
		std::string parsedText = stringResources->parseAndReplace(button->getText());
		
		osg::ref_ptr<osgText::Text> textNode = buildText(parsedText, *style.fontFamily, *style.fontSize, *style.color);
		textNode->setAlignment(osgText::Text::CENTER_CENTER);
		textNode->setPosition(osg::Vec3(0, 0, z));
		geode->addDrawable(textNode.get());
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

osg::Group* ControlGeometryBuilder::buildCheckBox(const CheckBox* checkBox) const {
	Style style = StyleBuilder::buildStyle(checkBox);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	const Size& size = checkBox->getSize();

	float x1 = 0 - (size.width/2);
	float y1 = 0 - size.height/2;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style, checkBox);

	// Draw a border around the checkbox. In order to do this the color style
	// must have been set. We draw the border with this color.
	if (style.color) {
		getNextLayer(z);
		geode->addDrawable(buildRectangle(x1, y1, x1 + size.height, y1 + size.height, z, 2.0f, *style.color, *style.color, true, true, true, true));

		// Draw an invisible object that can pick up mouse clicks.
		getNextLayer(z);
		osg::Vec4 invisibleColor(0.0f, 0.0f, 0.0f, 0.0f);
		geode->addDrawable(buildRectangle(x1, y1, x1 + size.height, y1 + size.height, z, invisibleColor, invisibleColor, invisibleColor, invisibleColor));
	}

	// Draw the text of the checkbox if all needed styles exists.
	if (style.fontSize && style.fontFamily && style.color) {
		std::string checkBoxText = checkBox->getText();
		std::string parsedText = checkBoxText;
		Ref<const Window> window = Window::getWindow(checkBox);
		if (window.valid()) {
			Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
			parsedText = stringResources->parseAndReplace(checkBoxText);
		}

		getNextLayer(z);
		osg::ref_ptr<osgText::Text> button_text = new osgText::Text;
		button_text->setText(parsedText);
		button_text->setColor(*style.color);
		button_text->setAlignment(osgText::Text::LEFT_CENTER);
		button_text->setFont(style.fontFamily->c_str());
		button_text->setCharacterSize(*style.fontSize);
		button_text->setFontResolution(30, 30);
		button_text->setAxisAlignment(osgText::Text::XY_PLANE);
		button_text->setRotation(osg::Quat(PI/2, 0, 0, 0));
		button_text->setPosition(osg::Vec3(x1 + size.height + 5, 0, z));
		geode->addDrawable(button_text.get());
	}

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateSet->setAttributeAndModes(blendFunction.get());

	if (checkBox->getChecked() && style.color) {
		getNextLayer(z);
		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
		transform->setMatrix(osg::Matrix::translate(x1 + (size.height / 2.0f), -0.2f, 0));
		osg::Vec4 color2 = *style.color;
		color2._v[3] = 0.0f;
		transform->addChild(buildStar(size.height, z, color2, *style.color));
		group->addChild(transform.get());
	}
	return group.release();
}

osg::Group* ControlGeometryBuilder::buildLabel(const Label* label) const {
	Style style = StyleBuilder::buildStyle(label);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style, label);

	// Check for mandatory elements of font information.
	if (style.fontSize && style.fontFamily && style.color) {
		std::string labelText = label->getText();
		std::string parsedText = labelText;
		Ref<const Window> window = Window::getWindow(label);
		if (window.valid()) {
			Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
			parsedText = stringResources->parseAndReplace(labelText);
		}

		getNextLayer(z);
		
	    osg::ref_ptr<osgText::Text> button_text = buildText(parsedText, *style.fontFamily, *style.fontSize, *style.color);
	    button_text->setMaximumWidth(label->getSize().width);
	    button_text->setAlignment(label->getAlignment());
		switch(label->getAlignment()) {
			case osgText::Text::LEFT_TOP:
			case osgText::Text::LEFT_CENTER:
				button_text->setPosition(osg::Vec3(0 - (label->getSize().width / 2), 0, z));
				break;
			case osgText::Text::LEFT_BOTTOM:
			case osgText::Text::CENTER_TOP:
			case osgText::Text::CENTER_CENTER:
				button_text->setPosition(osg::Vec3(0, 0, z));
				break;
			case osgText::Text::CENTER_BOTTOM:
			case osgText::Text::RIGHT_TOP:
			case osgText::Text::RIGHT_CENTER:
			case osgText::Text::RIGHT_BOTTOM:
			default:
				button_text->setPosition(osg::Vec3(0, 0, z));
				break;
		}
	    geode->addDrawable(button_text.get());
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

osg::Group* ControlGeometryBuilder::buildImage(const Image* image) const {
	Style style = StyleBuilder::buildStyle(image);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style, image);

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
	Style style = StyleBuilder::buildStyle(listBox);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style, listBox);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildListBoxItem(const ListBox* /*listBox*/, const ListBoxItem* listBoxItem) const {
	Style style = StyleBuilder::buildStyle(listBoxItem);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style, listBoxItem);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildModel(const Model* model) const {
	Style style = StyleBuilder::buildStyle(model);

	// Test if the control is visible or not.
	if (style.visible && *style.visible == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style, model);
	
	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

	// Load the model from the file system and add
	// it to the control if it was found.
	Ref<ResourceLocator> resourceLocator = createDefaultResourceLocator();
	std::string modelPath = model->getModelFilePath();
	if (resourceLocator->locateResource(modelPath)) {
		osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(modelPath);
		if (loadedModel.valid()) {
			double scale = model->getScale();
			if(scale == 1.0) {
				group->addChild(loadedModel.get());
			}
			else {
				osg::ref_ptr<osg::MatrixTransform> transformGroup = new osg::MatrixTransform;
				transformGroup->setMatrix(osg::Matrix::scale(osg::Vec3(scale, scale, scale)));
				transformGroup->addChild(loadedModel.get());
				group->addChild(transformGroup.get());
			}
		}
	}

    osg::StateSet *stateSet = group->getOrCreateStateSet();
    stateSet->setRenderBinDetails(100, "RenderBin");
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	return group.release();
}

} // namespace wf

CSP_NAMESPACE_END
