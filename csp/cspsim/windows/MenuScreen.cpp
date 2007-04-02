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

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/EventMapIndex.h>
#include <csp/cspsim/wf/WindowManager.h>
#include <csp/cspsim/windows/MenuScreen.h>
#include <csp/cspsim/windows/MainMenu.h>

CSP_NAMESPACE

namespace windows {

MenuScreen::MenuScreen() {
}

MenuScreen::~MenuScreen() {
}

void MenuScreen::onInit() {
	m_WindowManager = new wf::WindowManager;
	
	// Display the desktop window and the main menu.
	displayDesktopAndMainMenu();

	// We need some kindo of keyboard binding for this screen.
	m_Interface = new VirtualHID();
	m_Interface->bindObject(this);
}

void MenuScreen::onExit() {
}

void MenuScreen::onRender() {
	m_WindowManager->onRender();
}

void MenuScreen::onUpdate(double dt) {
	m_WindowManager->onUpdate(dt);
}

bool MenuScreen::onMouseMove(SDL_MouseMotionEvent const &event) {
	return m_WindowManager->onMouseMove(event.x, event.y, event.xrel, event.yrel);
}

bool MenuScreen::onMouseButton(SDL_MouseButtonEvent const &event) {
	if(event.state == SDL_RELEASED) {
		return m_WindowManager->onClick(event.x, event.y);
	}
	return false;
}

void MenuScreen::displayDesktopAndMainMenu() {
	m_WindowManager->closeByType<wf::Window>();

	// Serializer to be used for reading xml definitions of windows and controls.
	Ref<wf::Serialization> serializer = new wf::Serialization(getUIPath());

	// This is the desktop window that is used a background window
	// for all dialogs that is displayed.
	Ref<wf::Window> desktopWindow = new wf::Window;
	serializer->load(desktopWindow.get(), getUITheme(), "desktop.xml");
	m_WindowManager->show(desktopWindow.get());
	desktopWindow->maximizeWindow();
	
	// Display the main menu.
	Ref<MainMenu> mainMenu = new MainMenu;
	m_WindowManager->show(mainMenu.get());
}

}

CSP_NAMESPACE_END

