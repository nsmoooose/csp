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
 * @file Window.cpp
 *
 **/

#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManager.h>

#include <osg/Group>

namespace csp {

namespace wf {

Window::Window() :
	SingleControlContainer("Window"),
	m_StringResources(new StringResourceManager) {
	setCanFocus(true);
}

Window::~Window() {
}

void Window::performLayout() {
	SingleControlContainer::performLayout();

	ControlGeometryBuilder geometryBuilder;
	getNode()->addChild(geometryBuilder.buildWindow(this));
}

WindowManager* Window::getWindowManager() {
	return m_WindowManager.get();
}

void Window::setWindowManager(WindowManager* manager) {
	m_WindowManager = manager;
}

void Window::layoutChildControls() {
	Control* childControl = getControl();
	if(childControl != NULL) {
		ControlGeometryBuilder geometryBuilder;
		Rectangle clientRect = getClientRect();
		Ref<Style> style = childControl->getStyle();
		style->setWidth(Style::UnitValue(Style::Pixels, clientRect.width()));
		style->setHeight(Style::UnitValue(Style::Pixels, clientRect.height()));
		style->setLeft(Style::UnitValue(Style::Pixels, clientRect.x0));
		style->setTop(Style::UnitValue(Style::Pixels, clientRect.y0));
		Container* container = dynamic_cast<Container*>(childControl);
		if(container != NULL) {
			container->layoutChildControls();
		}
	}
}

void Window::close() {
	if(m_WindowManager.valid()) {
		m_WindowManager->close(this);
	}
}

Window* Window::getWindow(Control* control) {
	// We call the const version of this method.
	return const_cast<Window*>(getWindow(static_cast<const Control*>(control)));
}

const Window* Window::getWindow(const Control* control) {
	if(control == NULL) {
		return NULL;
	}
	const Window* window = dynamic_cast<const Window*>(control);
	if(window != NULL) {
		return window;
	}
	return getWindow(control->getParent());
}

void Window::setTheme(const std::string& theme) {
	m_Theme = theme;
}

std::string Window::getTheme() const {
	return m_Theme;
}

StringResourceManager* Window::getStringResourceManager() {
	return m_StringResources.get();
}

const StringResourceManager* Window::getStringResourceManager() const {
	return m_StringResources.get();
}

optional<Ref<Style> > Window::getNamedStyle(const std::string& name) const {
	optional<Ref<Style> > style;
	// Assign the style to the optional if it exists in the map.
	NamedStyleMap::const_iterator styleIterator = m_Styles.find(name);
	if(styleIterator != m_Styles.end()) {
		style.assign(styleIterator->second);
	}
	return style;
}

void Window::addNamedStyle(const std::string& name, Style* style) {
	m_Styles[name] = style;
}

void Window::centerWindow() {
	if(!m_WindowManager.valid()) {
		return;
	}

	Size screenSize = m_WindowManager->getScreenSize();
	Size windowSize = getSize();

	Point windowLocation =
		Point(screenSize.width / 2 - windowSize.width / 2,
		screenSize.height / 2 - windowSize.height / 2);

	Ref<Style> style = getStyle();
	style->setLeft(Style::UnitValue(Style::Pixels, windowLocation.x));
	style->setTop(Style::UnitValue(Style::Pixels, windowLocation.y));

	layoutChildControls();
	performLayout();
}

void Window::maximizeWindow() {
	if(!m_WindowManager.valid()) {
		return;
	}

	Size screenSize = m_WindowManager->getScreenSize();
	Ref<Style> style = getStyle();
	style->setWidth(Style::UnitValue(Style::Pixels, screenSize.width));
	style->setHeight(Style::UnitValue(Style::Pixels, screenSize.height));
	style->setLeft(Style::UnitValue(Style::Pixels, 0));
	style->setTop(Style::UnitValue(Style::Pixels, 0));

	layoutChildControls();
	performLayout();
}

void Window::createDefaultStyle() {
    // ==============================================================
    // Label
    Ref<Style> labelStyle = new Style();
	labelStyle->setColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	labelStyle->setFontFamily(std::string("prima_sans_bt.ttf"));
	labelStyle->setFontSize(15);
    addNamedStyle("Label", labelStyle.get());

    // ==============================================================
    // Listbox
    Ref<Style> listboxStyle = new Style();
	listboxStyle->setBorderWidth(1.0);
	listboxStyle->setBorderColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
    addNamedStyle("ListBox", listboxStyle.get());

    Ref<Style> listboxItemStyle = new Style();
	listboxItemStyle->setBorderWidth(1.0);
	listboxItemStyle->setBorderColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
    listboxItemStyle->setHeight(Style::UnitValue(Style::Pixels, 30));
    addNamedStyle("ListBoxItem", listboxItemStyle.get());

    Ref<Style> listboxItemSelectedStyle = new Style();
	listboxItemSelectedStyle->setBorderWidth(1.0);
	listboxItemSelectedStyle->setBorderColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	listboxItemSelectedStyle->setBackgroundColor(osg::Vec4(0.9, 0.9, 0.9, 1.0));
    listboxItemSelectedStyle->setHeight(Style::UnitValue(Style::Pixels, 30));
    addNamedStyle("ListBoxItem:selected", listboxItemSelectedStyle.get());

    // ==============================================================
    // Scrollbars
	Ref<Style> verticalStyle = new Style();
	verticalStyle->setWidth(Style::UnitValue(Style::Pixels, 22));
	verticalStyle->setBackgroundColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	verticalStyle->setBorderWidth(1.0);
	verticalStyle->setBorderColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	addNamedStyle("VerticalScrollBar", verticalStyle.get());

	Ref<Style> horizontalStyle = new Style();
	horizontalStyle->setHeight(Style::UnitValue(Style::Pixels, 22));
	horizontalStyle->setBackgroundColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	horizontalStyle->setBorderWidth(1.0);
	horizontalStyle->setBorderColor(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	addNamedStyle("HorizontalScrollBar", horizontalStyle.get());

	Ref<Style> scrollLeftStyle = new Style();
	scrollLeftStyle->setHeight(Style::UnitValue(Style::Pixels, 16));
	scrollLeftStyle->setWidth(Style::UnitValue(Style::Pixels, 16));
	scrollLeftStyle->setHorizontalAlign(std::string("left"));
	scrollLeftStyle->setVerticalAlign(std::string("middle"));
	scrollLeftStyle->setBackgroundColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	scrollLeftStyle->setBackgroundImage(std::string("go-left.png"));
	addNamedStyle("ScrollLeftButton", scrollLeftStyle.get());

	Ref<Style> scrollRightStyle = new Style();
	scrollRightStyle->setHeight(Style::UnitValue(Style::Pixels, 16));
	scrollRightStyle->setWidth(Style::UnitValue(Style::Pixels, 16));
	scrollRightStyle->setHorizontalAlign(std::string("right"));
	scrollRightStyle->setVerticalAlign(std::string("middle"));
	scrollRightStyle->setTop(Style::UnitValue(Style::Pixels, 0));
	scrollRightStyle->setBackgroundColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	scrollRightStyle->setBackgroundImage(std::string("go-right.png"));
	addNamedStyle("ScrollRightButton", scrollRightStyle.get());

	Ref<Style> scrollUpStyle = new Style();
	scrollUpStyle->setHeight(Style::UnitValue(Style::Pixels, 16));
	scrollUpStyle->setWidth(Style::UnitValue(Style::Pixels, 16));
	scrollUpStyle->setHorizontalAlign(std::string("center"));
	scrollUpStyle->setVerticalAlign(std::string("top"));
	scrollUpStyle->setBackgroundColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	scrollUpStyle->setBackgroundImage(std::string("go-up.png"));
	addNamedStyle("ScrollUpButton", scrollUpStyle.get());

	Ref<Style> scrollDownStyle = new Style();
	scrollDownStyle->setHeight(Style::UnitValue(Style::Pixels, 16));
	scrollDownStyle->setWidth(Style::UnitValue(Style::Pixels, 16));
	scrollDownStyle->setHorizontalAlign(std::string("center"));
	scrollDownStyle->setVerticalAlign(std::string("bottom"));
	scrollDownStyle->setBackgroundColor(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	scrollDownStyle->setBackgroundImage(std::string("go-down.png"));
	addNamedStyle("ScrollDownButton", scrollDownStyle.get());

    // ==============================================================
    // Window
    Ref<Style> windowStyle = new Style();
	windowStyle->setBackgroundColor(osg::Vec4(1, 1, 1, 10));
	windowStyle->setBorderWidth(1.0);
	windowStyle->setBorderColor(osg::Vec4(0, 0, 0, 1));
	addNamedStyle("Window", windowStyle.get());
}

} // namespace wf
} // namespace csp
