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

Window::Window() : 
	m_WindowManager(NULL),
	m_BackgroundColor(1.0, 1.0, 1.0, 1.0), 
	m_Caption("Caption") {
}

Window::~Window() {
}

const std::string &Window::getCaption() const { 
	return m_Caption; 
}

void Window::setCaption(const std::string &caption) { 
	m_Caption = caption; 
}

const osg::Vec4 &Window::getBackgroundColor() const { 
	return m_BackgroundColor; 
}

void Window::setBackgroundColor(const osg::Vec4 &backgroundColor) { 
	m_BackgroundColor = backgroundColor; 
}

void Window::buildGeometry(WindowManager* manager) {
	Control::buildGeometry(manager);

	m_WindowManager = manager;
	
	const Theme& theme = manager->getTheme();
	getNode()->addChild(theme.buildWindow(*this));
}

void Window::close() {
	if(m_WindowManager != NULL) {
		m_WindowManager->close(this);
	}
}

} // namespace wf

CSP_NAMESPACE_END
