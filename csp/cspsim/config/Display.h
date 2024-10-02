#pragma once
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
 * @file Display.h
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Export.h>

namespace csp {
namespace config {

typedef std::vector<std::string> StringVector;

/** Use to retreive and change display settings. Use CSPSim singleton in order
 * to make changes permanent. See Configuration class for more information. */
class CSPSIM_EXPORT Display: public Referenced {
public:
	Display(int width, int height, bool fullscreen, bool clouds);
	virtual ~Display();

	virtual int getWidth();
	virtual void setWidth(int width);

	virtual int getHeight();
	virtual void setHeight(int height);

	virtual bool getFullscreen();
	virtual void setFullscreen(bool fullscreen);

	virtual bool getClouds();
	virtual void setClouds(bool clouds);

	/** Enumerates all display modes and returns them as a vector in the following
	 *	format. NxN. Ex:
	 *	1280x1027
	 *	1024x768 */
	virtual StringVector enumerateDisplayModes();

	virtual Display* clone();
private:
	int m_Width;
	int m_Height;
	bool m_Fullscreen;
	bool m_Clouds;
};

} // end namespace config
} // end namespace csp
