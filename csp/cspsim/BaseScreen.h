#pragma once
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
 * @file BaseScreen.h
 *
 **/

// On Mac OS X, force use of Cocoa as the OpenGL backend
#ifdef __APPLE__
#define __USE_OSX_CGL_IMPLEMENTATION__
#endif

#include <csp/cspsim/input/InputInterface.h>
#include <csp/cspsim/input/HID.h>

namespace csp {

/** BaseScreen represents a generic view of the simulation or GUI.  Each screen
 *  within the game / sim should subclass this class.  The active screen receives
 *  input events and renders the view via the onRender callback.
 */
class CSPSIM_EXPORT BaseScreen: public input::InputInterface, public Referenced
{
public:
	BaseScreen();
	virtual ~BaseScreen();

	virtual void onInit() = 0;
	virtual void onExit() = 0;

	virtual void onUpdate(double dt) = 0;

	virtual Ref<input::VirtualHID> getInterface();

protected:
	Ref<input::VirtualHID> m_Interface;
};

} // namespace csp
