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
#include <iostream>

namespace csp {

SDLGraphicsWindow::SDLGraphicsWindow(const char *caption, const ::csp::ScreenSettings & screenSettings)
	: osgViewer::GraphicsWindowEmbedded(0, 0, screenSettings.width, screenSettings.height)
	, m_valid(false)
{
	CSPLOG(DEBUG, APP) << "Initializing SDL";

	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0 )
	{
		std::cerr << "Unable to initialize SDL (" << SDL_GetError() << ")" << std::endl;
		CSPLOG(ERROR, APP) << "Unable to initialize SDL (" << SDL_GetError() << ")";
		return;
	}

	int windowFlags = SDL_WINDOW_OPENGL;
	if ( screenSettings.fullScreen ) {
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	m_window = SDL_CreateWindow(caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenSettings.width, screenSettings.height, windowFlags);
	if (! m_window ) {
		std::cerr << "Unable to create window (" << SDL_GetError() << ")" <<::std::endl;
		return;
	}

	m_context = SDL_GL_CreateContext(m_window);
	if(!m_context) {
		std::cerr << "Unable to create GL context (" << SDL_GetError() << ")" <<::std::endl;
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_JoystickEventState(SDL_ENABLE);

	m_valid = true;
}

SDLGraphicsWindow::~SDLGraphicsWindow()
{
	SDL_Quit();
}

void SDLGraphicsWindow::swapBuffersImplementation()
{
	SDL_GL_SwapWindow(m_window);
}

} // namespace csp
