// Combat Simulator Project - FlightSim Demo
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

#ifndef __BASESCREEN_H__
#define __BASESCREEN_H__

//#include "SDL_keysym.h"
#include "InputInterface.h"


// BaseScreen represents a generic screen
// Each screen within the Game/Sim should
// Derive a class from class. Each Screen
// class must support a onRender() function
// that knows how to draw the screen.


/**
 * class BaseScreen - Describe me!
 *
 * @author unknown
 */
class BaseScreen: public InputInterface
{
public:
	BaseScreen();
	virtual ~BaseScreen();

	virtual void onInit() = 0;
	virtual void onExit() = 0;

	virtual void onUpdate(double dt) = 0;
	virtual void onRender() = 0;

	virtual simdata::Ref<VirtualHID> getInterface();

protected:
	simdata::Ref<VirtualHID> m_Interface;
};

#endif // __BASESCREEN_H__

