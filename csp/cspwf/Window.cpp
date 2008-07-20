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

} // namespace wf

} // namespace csp
