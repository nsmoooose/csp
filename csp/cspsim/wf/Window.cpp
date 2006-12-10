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

#include <csp/cspsim/wf/Theme.h>
#include <csp/cspsim/wf/Window.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

Window::Window(Theme* theme) : 
	SingleControlContainer(theme),
	m_WindowManager(NULL),
	m_Caption("Caption") {
}

Window::Window(Theme* theme, std::string caption) : 
	SingleControlContainer(theme),
	m_WindowManager(NULL),
	m_Caption(caption) {
}

Window::~Window() {
}

void Window::onInit() {
}

const std::string &Window::getCaption() const { 
	return m_Caption; 
}

void Window::setCaption(const std::string &caption) { 
	m_Caption = caption; 
}

void Window::buildGeometry() {
	SingleControlContainer::buildGeometry();

	getNode()->addChild(getTheme()->buildWindow(this));
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
		Theme* theme = getTheme();
		childControl->setSize(theme->getWindowClientAreaSize(this));
		childControl->setLocation(theme->getWindowClientAreaLocation(this));
	}
}

void Window::close() {
	if(m_WindowManager != NULL) {
		m_WindowManager->close(this);
	}
}

} // namespace wf

CSP_NAMESPACE_END
