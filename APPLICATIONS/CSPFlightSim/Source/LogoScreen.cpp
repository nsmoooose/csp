#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#include "LogoScreen.h"


extern int g_ScreenWidth;
extern int g_ScreenHeight;


void LogoScreen::OnInit()
{
    m_image = SDL_LoadBMP("Images/CSPLogo.bmp");
    if (m_image == NULL)
    {
        printf("Unable to load bitmap Images/CSPLogo.bmp\n");
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

    glViewport(0,0,g_ScreenWidth, g_ScreenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, g_ScreenWidth, 0.0, g_ScreenHeight, -1.0, 1.0);

    // this centers the bitmap in case its a different size then the screen.
    glRasterPos2i( ((g_ScreenWidth - m_image->w) >> 1),
                     g_ScreenHeight - ((g_ScreenHeight - m_image->h) >> 1) );
    glPixelZoom(1.0,-1.0);

    glDrawPixels(m_image->w, m_image->h, GL_RGB, GL_UNSIGNED_BYTE, m_image->pixels);
}

