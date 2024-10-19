// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file Display.cpp
 *
 **/

#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <csp/cspsim/config/Display.h>

namespace csp {
namespace config {

Display::Display(int width, int height, bool fullscreen, bool clouds) : m_Width(width), m_Height(height), m_Fullscreen(fullscreen), m_Clouds(clouds) {
}

Display::~Display() {
}

int Display::getWidth() {
	return m_Width;
}

void Display::setWidth(int width) {
	m_Width = width;
}

int Display::getHeight() {
	return m_Height;
}

void Display::setHeight(int height) {
	m_Height = height;
}

Display* Display::clone() {
	return new Display(m_Width, m_Height, m_Fullscreen, m_Clouds);
}

bool Display::getFullscreen() {
	return m_Fullscreen;
}

void Display::setFullscreen(bool fullscreen) {
	m_Fullscreen = fullscreen;
}

bool Display::getClouds() {
	return m_Clouds;
}

void Display::setClouds(bool clouds) {
	m_Clouds = clouds;
}

StringVector Display::enumerateDisplayModes() {
	StringVector displayModes;

	auto window = SDL_GL_GetCurrentWindow();
	if(!window) {
		return displayModes;
	}
	auto display_index = SDL_GetWindowDisplayIndex(window);
	auto mode_count = SDL_GetNumDisplayModes(display_index);
	for(int i = 0; i < mode_count; ++i) {
		SDL_DisplayMode mode;
		if(SDL_GetDisplayMode(display_index, i, &mode) != 0) {
			// Failed
			continue;
		}

		if(mode.w < 640 || mode.h < 480) {
			continue;
		}

		std::stringstream textStream;
		textStream << mode.w << 'x' << mode.h;
		displayModes.push_back(textStream.str());
	}
	return displayModes;

#if 0
	/* Get available fullscreen/hardware modes */
	Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
	SDL_Rect **modes=SDL_ListModes(NULL, flags);

	/* Check is there are any modes available */
	if(modes == (SDL_Rect **)0){
		return displayModes;
	}

	/* Check if or resolution is restricted */
	if(modes != (SDL_Rect **)-1){
		for(int i=0;modes[i];++i) {
			// Only display resolutions higher than 640x480
			if(modes[i]->w < 640 || modes[i]->h < 480) {
				continue;
			}
			std::stringstream textStream;
			textStream << modes[i]->w << 'x' << modes[i]->h;
			displayModes.push_back(textStream.str());
		}
	}
	return displayModes;
#endif
}

} // end namespace config
} // end namespace csp
