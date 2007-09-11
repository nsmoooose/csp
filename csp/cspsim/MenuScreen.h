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

#ifndef __CSPSIM_MENUSCREEN_H__
#define __CSPSIM_MENUSCREEN_H__

#include <csp/cspsim/BaseScreen.h>
#include <csp/cspsim/wf/WindowManager.h>

namespace osg {
	class State;
}

CSP_NAMESPACE

namespace wf {
	class WindowManager;
}

class CSPSIM_EXPORT MenuScreen : public BaseScreen { 
public:
	MenuScreen(osg::State* state);
	virtual ~MenuScreen();
 
	virtual void onInit();
	virtual void onExit();

	virtual void onRender();
	virtual void onUpdate(double dt);
	
	virtual bool onMouseMove(SDL_MouseMotionEvent const &);
	virtual bool onMouseButton(SDL_MouseButtonEvent const &);
	
	virtual wf::WindowManager* getWindowManager();
	virtual wf::Serialization* getSerializer();

private:
	osg::ref_ptr<osg::State> m_State;
	Ref<wf::WindowManager> m_WindowManager;
	Ref<wf::Serialization> m_Serializer;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_MENUSCREEN_H__
