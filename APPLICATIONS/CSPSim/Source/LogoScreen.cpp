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
 * @file LogoScreen.cpp
 *
 **/


# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning(disable : 4786)
# endif

#ifdef WIN32
#include <windows.h>
#endif

#include "LogoScreen.h"
#include "Config.h"

#include <SimData/FileUtility.h>

#include <iostream>
#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#include <SDL/SDL.h>


LogoScreen::LogoScreen(int width, int height) 
{
	m_width = width;
	m_height = height;
}

LogoScreen::~LogoScreen()
{
}

void LogoScreen::onInit()
{
	std::string image_path = getDataPath("ImagePath");
	std::string path = simdata::ospath::join(image_path, "CSPLogo.bmp");
	m_image = SDL_LoadBMP(path.c_str());
	if (m_image == NULL)
	{
		std::cout << "Unable to load bitmap " << path << std::endl;
	}
}

void LogoScreen::onExit()
{
	SDL_FreeSurface(m_image);
}

void LogoScreen::onRender()
{
    if (!m_image) 
        return;
    
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = m_image->w;
    src.h = m_image->h;

    dest.x = 0;
    dest.y = 0;
    dest.w = m_image->w;
    dest.h = m_image->h;

    glViewport(0, 0, m_width, m_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, m_width, 0.0, m_height, -1.0, 1.0);

    // this centers the bitmap in case its a different size then the screen.
    glRasterPos2i( ((m_width - m_image->w) >> 1),
                     m_height - ((m_height - m_image->h) >> 1) );
    glPixelZoom(1.0,-1.0);

    glDrawPixels(m_image->w, m_image->h, GL_RGB, GL_UNSIGNED_BYTE, m_image->pixels);
}

