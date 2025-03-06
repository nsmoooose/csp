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

#include <csp/cspsim/SDLGraphicsWindow.h>
#include <csp/cspsim/Config.h>
#include <csp/csplib/util/Log.h>
#include <SDL/SDL.h>
#include <iostream>

namespace csp {

SDLGraphicsWindow::SDLGraphicsWindow(const char *caption, const ::csp::ScreenSettings & screenSettings)
	: osgViewer::GraphicsWindowEmbedded(0, 0, screenSettings.width, screenSettings.height)
	, m_valid(false)
{
	CSPLOG(Prio_DEBUG, Cat_APP) << "Initializing SDL";

	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0 )
	{
		std::cerr << "Unable to initialize SDL (" << SDL_GetError() << ")" << std::endl;
		CSPLOG(Prio_ERROR, Cat_APP) << "Unable to initialize SDL (" << SDL_GetError() << ")";
		return;
	}

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	int bpp = info->vfmt->BitsPerPixel;

	CSPLOG(Prio_INFO, Cat_APP) << "Initializing video at " << bpp << " bits per pixel.";

	Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF;

	if ( screenSettings.fullScreen )
	{
		flags |= SDL_FULLSCREEN;
	}

	if ( !SDL_SetVideoMode(screenSettings.width, screenSettings.height, bpp, flags) )
	{
		std::cerr << "Unable to set video mode (" << SDL_GetError() << ")" << std::endl;
		CSPLOG(Prio_ERROR, Cat_APP) << "Unable to set video mode (" << SDL_GetError() << ")";
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_JoystickEventState(SDL_ENABLE);

	SDL_EnableUNICODE(1);

	SDL_WM_SetCaption(caption, "");

	m_valid = true;
}

SDLGraphicsWindow::~SDLGraphicsWindow()
{
	SDL_Quit();
}

void SDLGraphicsWindow::swapBuffersImplementation()
{
	SDL_GL_SwapBuffers();
}

} // namespace csp
