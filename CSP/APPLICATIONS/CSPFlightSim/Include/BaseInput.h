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
 * @file BaseInput.h
 *
 **/

#ifndef __BASEINPUT_H__
#define __BASEINPUT_H__

#include "SDL_keysym.h"
#include "BaseObject.h"


/**
 * class BaseInput - Describe me!
 *
 * @author unknown
 */
class BaseInput
{

public:

	virtual bool OnKeyDown(SDLKey key) = 0;
    virtual bool OnKeyUp(SDLKey key) = 0;
    virtual void OnMouseMove(int x, int y) = 0;
    virtual void OnMouseButtonDown(int num, int x, int y) = 0;
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val) = 0;
	virtual void OnJoystickHatMotion(int joynum, int hat, int val) = 0;
    virtual void OnJoystickButtonDown(int joynum, int butnum) = 0;
    virtual void OnUpdate() = 0;

	virtual void SetObject(BaseObject * ) = 0;

};

#endif // __BASEINPUT_H__

