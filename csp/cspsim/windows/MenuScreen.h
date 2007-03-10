// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file MenuScreen.h
 *
 **/

#ifndef __CSPSIM_WINDOWS_MENUSCREEN_H__
#define __CSPSIM_WINDOWS_MENUSCREEN_H__

#include <csp/cspsim/BaseScreen.h>
#include <csp/cspsim/wf/WindowManager.h>

CSP_NAMESPACE

namespace wf {
	class WindowManager;
}

namespace windows {

class MenuScreen : public BaseScreen { 
public:
	MenuScreen();
	virtual ~MenuScreen();
 
	virtual void onInit();
	virtual void onExit();

	virtual void onRender();
	virtual void onUpdate(double dt);
	
	virtual void displayDesktopAndMainMenu();

	DECLARE_INPUT_INTERFACE(MenuScreen, BaseScreen)
		BIND_CLICK("LEFT_CLICK", on_LeftClick);
	END_INPUT_INTERFACE  // protected:

	void on_LeftClick(MapEvent::ClickEvent const &event);

private:
	Ref<wf::WindowManager> m_WindowManager;
};

}

CSP_NAMESPACE_END

#endif // __CSPSIM_WINDOWS_MENUSCREEN_H__
