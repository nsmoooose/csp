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
 * @file MenuScreen.h
 *
 **/

#ifndef __MENUSCREEN_H__
#define __MENUSCREEN_H__

#include <osgText/Text>
#include <osg/Group>

#include "BaseScreen.h"

/**
 * class MenuScreen
 *
 * @author unknown
 */
class MenuScreen : public BaseScreen
{
public:
    virtual void OnInit();
    virtual void OnExit();

    virtual void OnRender();

    virtual bool OnKeyDown(char * key);
    virtual void OnKeyUp(char * key);
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseButtonDown(int num, int x, int y);
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val);
    virtual void OnJoystickButtonDown(int joynum, int butnum);

private:
        osgText::Text*    m_text;
        osg::Group* rootNode;


};

#endif // __MENUSCREEN_H__

