#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#include "LogoScreen.h"
#include "Platform.h"



LogoScreen::LogoScreen(int width, int height) 
{
	m_width = width;
	m_height = height;
}

LogoScreen::~LogoScreen()
{
}

void LogoScreen::OnInit()
{
    std::string path = ospath::filter("../Data/Images/CSPLogo.bmp");
    m_image = SDL_LoadBMP(path.c_str());
    if (m_image == NULL)
    {
		std::cout << "Unable to load bitmap " << path << std::endl;
    }


}

void LogoScreen::OnExit()
{
	SDL_FreeSurface(m_image);
}

void LogoScreen::OnRender()
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

