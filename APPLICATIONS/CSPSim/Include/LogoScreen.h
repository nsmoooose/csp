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
 * @file LogoScreen.h
 *
 **/

#ifndef __LOGOSCREEN_H__
#define __LOGOSCREEN_H__


#include <SDL/SDL.h>
#include "BaseScreen.h"


/**
 * class LogoScreen
 *
 * @author unknown
 */
class LogoScreen : public BaseScreen
{
public:
	LogoScreen(int width, int height);
	virtual ~LogoScreen();
 
	virtual void OnInit();
	virtual void OnExit();

	virtual void OnRender();
	virtual void OnUpdateObjects(double dt)  {;  }


private:
	SDL_Surface * m_image;
	int m_width, m_height;
};


#endif // __LOGOSCREEN_H__

