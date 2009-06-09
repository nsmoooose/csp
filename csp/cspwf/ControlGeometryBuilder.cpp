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

#include <csp/cspwf/Button.h>
#include <csp/cspwf/Check.h>
#include <csp/cspwf/CheckBox.h>
#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/Image.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/ListBox.h>
#include <csp/cspwf/ListBoxItem.h>
#include <csp/cspwf/Model.h>
#include <csp/cspwf/Point.h>
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/Size.h>
#include <csp/cspwf/StyleBuilder.h>
#include <csp/cspwf/Tab.h>
#include <csp/cspwf/Window.h>

#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgText/Text>

static const char* WF_FILENOTFOUND = "Failed to locate resource: ";

namespace csp {
namespace wf {

ControlGeometryBuilder::ControlGeometryBuilder() {
}

ControlGeometryBuilder::~ControlGeometryBuilder() {
}

void ControlGeometryBuilder::getNextLayer(float& z) const {
	z += 0.1f;
}

osgText::Text* ControlGeometryBuilder::buildText(const std::string& text, const std::string& fontFamily, float fontSize, osg::Vec4& color) const {
	Ref<ResourceLocator> resourceLocator = getDefaultResourceLocator();
	std::string fontFilePath = fontFamily;
	if(resourceLocator->locateResource(fontFilePath)) {
		osg::ref_ptr<osgText::Text> textNode = new osgText::Text;
		textNode->setText(text, osgText::String::ENCODING_UTF8);
		textNode->setColor(color);
		textNode->setFont(fontFilePath.c_str());
		textNode->setCharacterSize(fontSize);
		textNode->setAxisAlignment(osgText::Text::XY_PLANE);
		textNode->setRotation(osg::Quat(PI/2, 0, 0, 0));
		return textNode.release();
	}
	else {
		CSPLOG(ERROR, APP) << WF_FILENOTFOUND << fontFilePath;
	}
	return NULL;
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

	return geom.release();
}

void ControlGeometryBuilder::buildControl(osg::Geode* geode, float& z, const Style* style, const Control* control) const {
	// If the control isn't visible then we dont create any geometry at all.
	if (style->getVisible() && *style->getVisible() == false) {
		return;
	}

	const Size& size = control->getSize();

	float x1 = 0 - (size.width/2);
	float y1 = 0 - size.height/2;
	float x2 = (size.width/2);
	float y2 = size.height/2;

	// Draw background color of the control.
	osg::ref_ptr<osg::Geometry> backgroundGeometry;
	if (style->getBackgroundColor()) {
		getNextLayer(z);
		// Draw the background of the control.
		// First priority is to use backgroundColorXxxYyyy if a value exists. Otherwise we use
		// just backgroundColor style.
		backgroundGeometry = buildRectangle(x1, y1, x2, y2, z,
			!style->getBackgroundColorTopRight() ? *style->getBackgroundColor() : *style->getBackgroundColorTopRight(),
			!style->getBackgroundColorBottomRight() ? *style->getBackgroundColor() : *style->getBackgroundColorBottomRight(),
			!style->getBackgroundColorBottomLeft() ? *style->getBackgroundColor() : *style->getBackgroundColorBottomLeft(),
			!style->getBackgroundColorTopLeft() ? *style->getBackgroundColor() : *style->getBackgroundColorTopLeft());
		geode->addDrawable(backgroundGeometry.get());
	}

	// Test to see if we should have a background image to this control. In that case we
	// must load a texture and set it on background geometry.
	if (style->getBackgroundImage()) {
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
		Ref<ResourceLocator> resourceLocator = getDefaultResourceLocator();
		std::string filePath = *style->getBackgroundImage();
		if (resourceLocator->locateResource(filePath)) {
			image = osgDB::readImageFile(filePath);
			if(!image.valid()) {
				CSPLOG(ERROR, APP) << "Failed to read file: " << filePath;
			}
		}
		else {
			CSPLOG(ERROR, APP) << WF_FILENOTFOUND << filePath;
		}

		// If we succeeded with loading of the texture then we
		// set it so it is displayed.
		if (image.valid()) {
			// Create a texture and assign the image to it.
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			texture->setImage(image.get());

			osg::ref_ptr<osg::StateSet> stateset = backgroundGeometry->getOrCreateStateSet();
			stateset->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
			backgroundGeometry->setStateSet(stateset.get());
		}
	}

	// Increase the layer so the borders will be placed over the background.
	getNextLayer(z);

	// Draw top border.
	if ((style->getBorderWidth() || style->getBorderTopWidth()) && (style->getBorderColor() || style->getBorderTopColor())) {
		float borderWidth = style->getBorderTopWidth() ? *style->getBorderTopWidth() : *style->getBorderWidth();
		osg::Vec4 borderColor = style->getBorderTopColor() ? *style->getBorderTopColor() : *style->getBorderColor();

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			false, true, false, false);
		geode->addDrawable(geometry.get());
	}

	// Draw bottom border.
	if ((style->getBorderWidth() || style->getBorderBottomWidth()) && (style->getBorderColor() || style->getBorderBottomColor())) {
		float borderWidth = style->getBorderBottomWidth() ? *style->getBorderBottomWidth() : *style->getBorderWidth();
		osg::Vec4 borderColor = style->getBorderBottomColor() ? *style->getBorderBottomColor() : *style->getBorderColor();

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			false, false, false, true);
		geode->addDrawable(geometry.get());
	}

	// Draw left border.
	if ((style->getBorderWidth() || style->getBorderLeftWidth()) && (style->getBorderColor() || style->getBorderLeftColor())) {
		float borderWidth = style->getBorderLeftWidth() ? *style->getBorderLeftWidth() : *style->getBorderWidth();
		osg::Vec4 borderColor = style->getBorderLeftColor() ? *style->getBorderLeftColor() : *style->getBorderColor();

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			true, false, false, false);
		geode->addDrawable(geometry.get());
	}

	// Draw right border.
	if ((style->getBorderWidth() || style->getBorderRightWidth()) && (style->getBorderColor() || style->getBorderRightColor())) {
		float borderWidth = style->getBorderRightWidth() ? *style->getBorderRightWidth() : *style->getBorderWidth();
		osg::Vec4 borderColor = style->getBorderRightColor() ? *style->getBorderRightColor() : *style->getBorderColor();

		osg::ref_ptr<osg::Geometry> geometry = buildRectangle(
			x1, y1, x2, y2, z, borderWidth,
			borderColor, borderColor,
			false, false, true, false);
		geode->addDrawable(geometry.get());
	}
}

osg::Group* ControlGeometryBuilder::buildTab(const Tab* tab) const {
	return buildGenericControl(tab);
}

osg::Group* ControlGeometryBuilder::buildTabHeader(const TabHeader* header) const {
	Ref<Style> style = StyleBuilder::buildStyle(header);

	// Test if the control is visible or not.
	if (style->getVisible() && *style->getVisible() == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style.get(), header);

	// Check for mandatory elements of font information.
	if (style->getFontSize() && style->getFontFamily() && style->getColor()) {
		std::string labelText = header->getText();
		std::string parsedText = labelText;
		Ref<const Window> window = Window::getWindow(header);
		if (window.valid()) {
			Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
			parsedText = stringResources->parseAndReplace(labelText);
		}

		getNextLayer(z);

	    osg::ref_ptr<osgText::Text> button_text = buildText(parsedText, *style->getFontFamily(), *style->getFontSize(), *style->getColor());
		if(button_text.valid()) {
			button_text->setMaximumWidth(header->getSize().width);
			button_text->setAlignment(osgText::Text::CENTER_CENTER);
			button_text->setPosition(osg::Vec3(0, 0, z));
			geode->addDrawable(button_text.get());
		}
	}

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildTabPage(const TabPage* page) const {
	return buildGenericControl(page);
}

osg::Group* ControlGeometryBuilder::buildWindow(const Window* window) const {
	return buildGenericControl(window);
}

osg::Group* ControlGeometryBuilder::buildButton(const Button* button) const {
	// Build all generic geometry like background, border, colors etc.
	Ref<Style> style = StyleBuilder::buildStyle(button);

	// Test if the control is visible or not.
	if (style->getVisible() && *style->getVisible() == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	// Add all default control behaviours like borders, background color, image etc.
	buildControl(geode.get(), z, style.get(), button);

	Ref<const Window> window = Window::getWindow(button);
	if(style->getFontFamily() && style->getFontSize() && style->getColor() && window.valid()) {
		getNextLayer(z);

		Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
		std::string parsedText = stringResources->parseAndReplace(button->getText());

		osg::ref_ptr<osgText::Text> textNode = buildText(parsedText, *style->getFontFamily(), *style->getFontSize(), *style->getColor());
		if(textNode.valid()) {
			textNode->setAlignment(osgText::Text::CENTER_CENTER);
			textNode->setPosition(osg::Vec3(0, 0, z));
			geode->addDrawable(textNode.get());
		}
	}

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildCheck(const Check* check) const {
	return buildGenericControl(check);
}

osg::Group* ControlGeometryBuilder::buildCheckBox(const CheckBox* checkBox) const {
	Ref<Style> style = StyleBuilder::buildStyle(checkBox);

	// Test if the control is visible or not.
	if (style->getVisible() && *style->getVisible() == false) {
		return NULL;
	}

	const Size& size = checkBox->getSize();

	float x1 = 0 - (size.width/2);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style.get(), checkBox);

	// Draw the text of the checkbox if all needed styles exists.
	if (style->getFontSize() && style->getFontFamily() && style->getColor()) {
		std::string checkBoxText = checkBox->getText();
		std::string parsedText = checkBoxText;
		Ref<const Window> window = Window::getWindow(checkBox);
		if (window.valid()) {
			Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
			parsedText = stringResources->parseAndReplace(checkBoxText);
		}

		getNextLayer(z);
		osg::ref_ptr<osgText::Text> button_text = new osgText::Text;
		button_text->setText(parsedText, osgText::String::ENCODING_UTF8);
		button_text->setColor(*style->getColor());
		button_text->setAlignment(osgText::Text::LEFT_CENTER);
		button_text->setFont(style->getFontFamily()->c_str());
		button_text->setCharacterSize(*style->getFontSize());
		button_text->setFontResolution(30, 30);
		button_text->setAxisAlignment(osgText::Text::XY_PLANE);
		button_text->setRotation(osg::Quat(PI/2, 0, 0, 0));
		button_text->setPosition(osg::Vec3(x1 + size.height + 5, 0, z));
		geode->addDrawable(button_text.get());
	}

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());
	return group.release();
}

osg::Group* ControlGeometryBuilder::buildLabel(const Label* label) const {
	Ref<Style> style = StyleBuilder::buildStyle(label);

	// Test if the control is visible or not.
	if (style->getVisible() && *style->getVisible() == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style.get(), label);

	// Check for mandatory elements of font information.
	if (style->getFontSize() && style->getFontFamily() && style->getColor()) {
		std::string labelText = label->getText();
		std::string parsedText = labelText;
		Ref<const Window> window = Window::getWindow(label);
		if (window.valid()) {
			Ref<const StringResourceManager> stringResources = window->getStringResourceManager();
			parsedText = stringResources->parseAndReplace(labelText);
		}

		getNextLayer(z);
		
		// We need to set the adjustment of the text.
		float xAdjustment = 0.0f;
		float yAdjustment = 0.0f;
		
		// We store alignment settings within the style object as text strings.
		// This is not the most perfect solution. But it works nice with
		// the xml serialization we have. 
		// Here are some uggly code to translate these text based settings to the
		// enumeration that osg is using.
		std::string alignment = "center";
		if(style->getTextHorizontalAlign()) {
			if(*style->getTextHorizontalAlign() == "left") {
				xAdjustment = 0 - (label->getSize().width / 2);
				alignment = "left";
			}
			else if(*style->getTextHorizontalAlign() == "center") {
				xAdjustment = 0;
				alignment = "center";
			}
			else if(*style->getTextHorizontalAlign() == "right") {
				xAdjustment = (label->getSize().width / 2);
				alignment = "right";
			}
		}
		
		if(style->getTextVerticalAlign()) {
			if(*style->getTextVerticalAlign() == "top") {
				yAdjustment = 0 - (label->getSize().height / 2);
				alignment += "top";
			}
			else if(*style->getTextVerticalAlign() == "middle") {
				yAdjustment = 0;
				alignment += "middle";
			}
			else if(*style->getTextVerticalAlign() == "bottom") {
				yAdjustment = (label->getSize().height / 2);
				alignment += "bottom";
			}
		}
		else {
			alignment += "middle";
		}
		
		osgText::Text::AlignmentType alignmentEnum = osgText::Text::CENTER_CENTER;
		if(alignment == "lefttop") {
			alignmentEnum = osgText::Text::LEFT_TOP;
		}
		else if(alignment == "leftmiddle") {
			alignmentEnum = osgText::Text::LEFT_CENTER;
		}
		else if(alignment == "leftbottom") {
			alignmentEnum = osgText::Text::LEFT_BOTTOM;
		}
		else if(alignment == "centertop") {
			alignmentEnum = osgText::Text::CENTER_TOP;
		}
		else if(alignment == "centermiddle") {
			alignmentEnum = osgText::Text::CENTER_CENTER;
		}
		else if(alignment == "centerbottom") {
			alignmentEnum = osgText::Text::CENTER_BOTTOM;
		}
		else if(alignment == "righttop") {
			alignmentEnum = osgText::Text::RIGHT_TOP;
		}
		else if(alignment == "rightmiddle") {
			alignmentEnum = osgText::Text::RIGHT_CENTER;
		}
		else if(alignment == "rightbottom") {
			alignmentEnum = osgText::Text::RIGHT_BOTTOM;
		}

	    osg::ref_ptr<osgText::Text> button_text = buildText(parsedText, *style->getFontFamily(), *style->getFontSize(), *style->getColor());
		if(button_text.valid()) {
			button_text->setMaximumWidth(label->getSize().width);
			button_text->setAlignment(alignmentEnum);
			button_text->setPosition(osg::Vec3(xAdjustment, yAdjustment, z));
			geode->addDrawable(button_text.get());
		}
	}

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildGenericControl(const Control* control) const {
	Ref<Style> style = StyleBuilder::buildStyle(control);

	// Test if the control is visible or not.
	if (style->getVisible() && *style->getVisible() == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style.get(), control);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

	return group.release();
}

osg::Group* ControlGeometryBuilder::buildImage(const Image* image) const {
	return buildGenericControl(image);
}

osg::Group* ControlGeometryBuilder::buildListBox(const ListBox* listBox) const {
	return buildGenericControl(listBox);
}

osg::Group* ControlGeometryBuilder::buildListBoxItem(const ListBoxItem* listBoxItem) const {
	return buildGenericControl(listBoxItem);
}

osg::Group* ControlGeometryBuilder::buildModel(const Model* model) const {
	Ref<Style> style = StyleBuilder::buildStyle(model);

	// Test if the control is visible or not.
	if (style->getVisible() && *style->getVisible() == false) {
		return NULL;
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	float z = 0;

	buildControl(geode.get(), z, style.get(), model);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode.get());

	// Load the model from the file system and add
	// it to the control if it was found.
	Ref<ResourceLocator> resourceLocator = getDefaultResourceLocator();
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
		else {
			CSPLOG(ERROR, APP) << WF_FILENOTFOUND << modelPath;
		}
	}

	return group.release();
}

Size ControlGeometryBuilder::getSizeOfText(const std::string& text, const std::string& fontFamily, float fontSize) const {
	osg::Vec4 color(0.0f, 0.0f, 0.0f, 1.0f);
	osg::ref_ptr<osgText::Text> textObject = buildText(text, fontFamily, fontSize, color);
	if(textObject.valid()) {
		osg::BoundingBox boundingBox = textObject->getBound();
		return Size(boundingBox.xMax() - boundingBox.xMin(), boundingBox.yMax() - boundingBox.yMin());
	}
	else {
		return Size(0, 0);
	}
}

Size ControlGeometryBuilder::calculateSize(const Control* control, const Style* style) const {
	if(!style->getWidth() || !style->getHeight()) {
		return Size(0,0);
	}

	float height = style->getHeight()->value;
	float width = style->getWidth()->value;

	const Container* parent = control->getParent();
	if(parent == NULL && (style->getWidth()->unit == Style::Percentage || style->getHeight()->unit == Style::Percentage) ) {
		return Size(0,0);
	}
	
	// Check if we need to change the variable to the actual amount of pixels.
	if(style->getWidth()->unit == Style::Percentage) {
		width = parent->getSize().width * (width / 100);
	}
	if(style->getHeight()->unit == Style::Percentage) {
		height = parent->getSize().height * (height / 100);
	}
	return Size(width, height);
}

Point ControlGeometryBuilder::calculateLocation(const Control* control, const Style* style) const {
	if(!style->getTop() || !style->getLeft()) {
		return Point(0,0);
	}
	
	float top = style->getTop()->value;
	float left = style->getLeft()->value;

	const Container* parent = control->getParent();	
	if(parent == NULL && (style->getTop()->unit == Style::Percentage || style->getLeft()->unit == Style::Percentage) ) {
		return Point(0,0);
	}
	
	if(style->getTop()->unit == Style::Percentage) {
		top = parent->getClientRect().height() * (top / 100);
	}
	if(style->getLeft()->unit == Style::Percentage) {
		left = parent->getClientRect().width() * (left / 100);
	} 
	return Point(left,top);
}

} // namespace wf

} // namespace csp
