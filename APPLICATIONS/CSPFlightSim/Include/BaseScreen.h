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

#include "SDL_keysym.h"

// BaseScreen represents a generic screen
// Each screen within the Game/Sim should
// Derive a class from class. Each Screen
// class must support a OnRender() function
// that knows how to draw the screen.


/**
 * class BaseScreen - Describe me!
 *
 * @author unknown
 */
class BaseScreen
{
public:
    virtual void OnInit() = 0;
    virtual void OnExit() = 0;

    virtual void OnUpdateObjects(float dt) = 0;
    virtual void OnRender() = 0;

    // Input Routines, are not required. If the are not overloaded then
    // a dummy routine will be called here.
    virtual bool OnKeyDown(char *key) { return false; }
	virtual bool OnKeyDown(SDLKey key) {return false; }
    virtual void OnKeyUp(char * key) { ; }
	virtual bool OnKeyUp(SDLKey key) { return false; }
    virtual void OnMouseMove(int x, int y) {; }
    virtual void OnMouseButtonDown(int num, int x, int y) {; }
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val) {; }
	virtual void OnJoystickHatMotion(int joynum, int hat, int val) {; }
    virtual void OnJoystickButtonDown(int joynum, int butnum) {; }

};

#endif // __BASESCREEN_H__

