// Combat Simulator Project
// Copyright (C) 2002-2004 The Combat Simulator Project
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
 * @file MenuScreen.cpp
 *
 **/

#include <osg/State>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/input/EventMapIndex.h>
#include <csp/cspsim/MenuScreen.h>
#include <csp/cspwf/WindowManager.h>

namespace csp {

MenuScreen::MenuScreen(osg::State* state) : m_State(state) {
}

MenuScreen::~MenuScreen() {
}

void MenuScreen::onInit() {
	m_Serializer = new wf::Serialization();

	// We need some kindo of keyboard binding for this screen.
	m_Interface = new input::VirtualHID();
	m_Interface->bindObject(this);
}

void MenuScreen::onExit() {
}

void MenuScreen::onRender() {
}

void MenuScreen::onUpdate(double /* dt */) {
}

bool MenuScreen::onMouseMove(SDL_MouseMotionEvent const &event) {
	return CSPSim::theSim->getWindowManager()->onMouseMove(event.x, event.y, event.xrel, event.yrel);
}

bool MenuScreen::onMouseButton(SDL_MouseButtonEvent const &event) {
	if(event.state == SDL_RELEASED) {
		return CSPSim::theSim->getWindowManager()->onClick(event.x, event.y);
	}
	return false;
}

wf::Serialization* MenuScreen::getSerializer() {
	return m_Serializer.get();
}

} // namespace csp

