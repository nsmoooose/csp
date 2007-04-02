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

#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Window.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

Window::Window() : 
	m_WindowManager(NULL) {
}

Window::~Window() {
}

std::string Window::getName() const {
	return "Window";
}

void Window::buildGeometry() {
	SingleControlContainer::buildGeometry();

	ControlGeometryBuilder geometryBuilder;
	getNode()->addChild(geometryBuilder.buildWindow(this));
}

WindowManager* Window::getWindowManager() {
	return m_WindowManager;
}

void Window::setWindowManager(WindowManager* manager) {
	m_WindowManager = manager;
}

void Window::layoutChildControls() {
	Control* childControl = getControl();
	if(childControl != NULL) {
		ControlGeometryBuilder geometryBuilder;
		Rectangle clientRect = getClientRect();
		childControl->setSize(Size(clientRect.width(), clientRect.height()));
		childControl->setLocation(Point(clientRect.x0, clientRect.y0));
		Container* container = dynamic_cast<Container*>(childControl);
		if(container != NULL) {
			container->layoutChildControls();
		}
	}
}

void Window::close() {
	if(m_WindowManager != NULL) {
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

optional<Style> Window::getNamedStyle(const std::string& name) const {
	optional<Style> style;
	// Assign the style to the optional if it exists in the map.
	NamedStyleMap::const_iterator styleIterator = m_Styles.find(name);
	if(styleIterator != m_Styles.end()) {
		style.assign(styleIterator->second);
	}
	return style;
}

void Window::addNamedStyle(const std::string& name, const Style& style) {
	m_Styles[name] = style;
}

void Window::centerWindow() {
	if(m_WindowManager == NULL) {
		return;
	}
	
	Size screenSize = m_WindowManager->getScreenSize();
	Size windowSize = getSize();
		
	Point windowLocation = 
		Point(screenSize.width / 2 - windowSize.width / 2, 
		screenSize.height / 2 - windowSize.height / 2);
		
	setLocation(windowLocation);

	layoutChildControls();
	buildGeometry();
}

void Window::maximizeWindow() {
	if(m_WindowManager == NULL) {
		return;
	}

	Size screenSize = m_WindowManager->getScreenSize();
	setSize(screenSize);
	setLocation(Point(0, 0));

	layoutChildControls();
	buildGeometry();
}

} // namespace wf

CSP_NAMESPACE_END
