// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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
 * @file WindowManager.cpp
 *
 **/

#include <csp/cspwf/ControlCallback.h>
#include <csp/cspwf/Serialization.h>
#include <csp/cspwf/StyleBuilder.h>
#include <csp/cspwf/WindowManager.h>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Group>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/TexEnv>
#include <osgUtil/IntersectVisitor>

#include <SDL/SDL.h>

namespace csp {
namespace wf {

WindowManager::WindowManager(int width, int height) : 
	m_ScreenWidth(width), m_ScreenHeight(height) {

	m_Group = new osg::Group;
}

WindowManager::~WindowManager() {
}

bool WindowManager::onClick(int x, int y) {
	Control* control = getControlAtPosition(x, y);
	ClickEventArgs event(control, x, y);
	if(control != NULL) {
		control->onClick(event);
	}
	return event.handled;
}

bool WindowManager::onMouseMove(int x, int y) {
	// Save the new mouse position.
	m_MousePosition = Point(x, y);

	Control* newHoverControl = getControlAtPosition(x, y);
	if(newHoverControl != m_HoverControl.get()) {
		removeStateAndRebuildGeometry("hover", m_HoverControl.get());
		addStateAndRebuildGeometry("hover", newHoverControl);
	}
	m_HoverControl = newHoverControl;

	/* Fire signals if we have a control at the position. */
	Control* control = getControlAtPosition(x, y);
	MouseEventArgs event(control, x, y);
	if(control != NULL) {
		control->onMouseMove(event);
	}
	return event.handled;
}

bool WindowManager::onMouseDown(int x, int y, int button) {
	Control* control = getControlAtPosition(x, y);
	MouseButtonEventArgs event(control, button, x, y);
	if(control != NULL) {
		control->onMouseDown(event);
	}
	return event.handled;
}

bool WindowManager::onMouseUp(int x, int y, int button) {
	Control* control = getControlAtPosition(x, y);
	MouseButtonEventArgs event(control, button, x, y);
	if(control != NULL) {
		control->onMouseUp(event);
	}
	return event.handled;
}

void WindowManager::removeStateAndRebuildGeometry(const std::string& state, Control* control) {
	if(control == NULL) {
		return;
	}

	Ref<Style> currentStyle = StyleBuilder::buildStyle(control);
	control->removeState(state);
	Ref<Style> newStyle = StyleBuilder::buildStyle(control);
	if(!currentStyle->equals(newStyle.get())) {
		control->performLayout();
	}
}

void WindowManager::addStateAndRebuildGeometry(const std::string& state, Control* control) {
	if(control == NULL) {
		return;
	}

	Ref<Style> currentStyle = StyleBuilder::buildStyle(control);
	control->addState(state);
	Ref<Style> newStyle = StyleBuilder::buildStyle(control);
	if(!currentStyle->equals(newStyle.get())) {
		control->performLayout();
	}
}

Point WindowManager::getMousePosition() const {
	return m_MousePosition;
}

Window* WindowManager::getById(const std::string& id) {
	WindowVector::iterator window = m_Windows.begin();
	for(;window != m_Windows.end();++window) {
		if((*window)->getId() == id) {
			return window->get();
		}
	}
	return NULL;
}

void WindowManager::show(Window* window) {
	// We must initialize the window and all it's child controls.
	// We must assign the window to this instance of the window manager.
	// It makes it possible for windows to open new or close existing
	// windows.
	window->setWindowManager(this);

	performLayout(window);

	// Add the window into the tree of nodes in osg. This will make
	// the window visible for the user in the next render.
	getWindowNode()->addChild(window->getNode());

	// Also store a reference to the window.
	m_Windows.push_back(window);
}

void WindowManager::performLayout(Window* window) {
	// Force this container to align all child controls.
	window->layoutChildControls();

	// We do wish to handle alignment when the window is displayed for
	// the first time.
	Point windowLocation = window->getLocation();
	const Size windowSize = window->getSize();
	Size screenSize = getScreenSize();

	Ref<Style> windowStyle = StyleBuilder::buildStyle(window);
	if(windowStyle->getHorizontalAlign()) {
		if(*windowStyle->getHorizontalAlign() == "left") {
			windowLocation.x = 0;
		}
		else if(*windowStyle->getHorizontalAlign() == "center") {
			windowLocation.x = (screenSize.width / 2) - (windowSize.width / 2);
		}
		else if(*windowStyle->getHorizontalAlign() == "right") {
			windowLocation.x = screenSize.width - windowSize.width;
		}
	}
	if(windowStyle->getVerticalAlign()) {
		if(*windowStyle->getVerticalAlign() == "top") {
			windowLocation.y = 0;
		}
		else if(*windowStyle->getVerticalAlign() == "middle") {
			windowLocation.y = (screenSize.height / 2) - (windowSize.height / 2);
		}
		else if(*windowStyle->getVerticalAlign() == "bottom") {
			windowLocation.y = screenSize.height - windowSize.height;
		}
	}
	Ref<Style> style = window->getStyle();
	style->setLeft(Style::UnitValue(Style::Pixels, windowLocation.x));
	style->setTop(Style::UnitValue(Style::Pixels, windowLocation.y));

	// Build the actual geometry of all controls that is going to be
	// displayed.
	window->performLayout();
}

void WindowManager::close(Window* window) {
	Ref<Window> myWindow = window;

	// Lets remove the node that represents the window. This will
	// remove the window on the next rendering.
	osg::Group* windowGroup = window->getNode();
	getWindowNode()->removeChild(windowGroup);

	// We must also remove our reference to the Window object.
	WindowVector::iterator iteratedWindow = m_Windows.begin();
	for(;iteratedWindow != m_Windows.end();++iteratedWindow) {
		if(iteratedWindow->get() == window) {
			m_Windows.erase(iteratedWindow);
			break;
		}
	}

	// Detach the window from the window manager by assigning
	// a NULL window manager.
	myWindow->setWindowManager(NULL);
	myWindow->dispose();
}

void WindowManager::closeAll() {
	closeByType<Window>();
}


Size WindowManager::getScreenSize() const {
	Size size;
	float scale;
	calculateScreenSizeAndScale(size, scale);
	return size;
}

void WindowManager::calculateScreenSizeAndScale(Size& size, float& scale) const {
	Size screenSize(m_ScreenWidth, m_ScreenHeight);

	if(screenSize.width >= 1024 && screenSize.height >= 768) {
		size = screenSize;
		scale = 1.0;
		return;
	}

	// Well the user has a low screen resolution. We do wish to scale down
	// all user interface elements into smaler controls. We current don't have
	// support for rescaling of controls. So lets fake a higher screen
	// resolution and calculate a scale transformation.
	size = screenSize;
	double minimumRatio = 1024.0 / 768.0;
	double aspectRatio = screenSize.width / screenSize.height;
	if(aspectRatio > minimumRatio) {
		size.width *= (768.0 / screenSize.height);
		size.height *= (768.0 / screenSize.height);
		scale = screenSize.height / 768.0;
	}
	else {
		size.width *= (1024.0 / screenSize.width);
		size.height *= (1024.0 / screenSize.width);
		scale = screenSize.width / 1024.0;
	}
}

osg::Group* WindowManager::getWindowNode() {
	return m_Group.get();
}

} // namespace wf
} // namespace csp
