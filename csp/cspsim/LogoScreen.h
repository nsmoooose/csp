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


/**
 * @file LogoScreen.h
 *
 **/

#ifndef __CSPSIM_LOGOSCREEN_H__
#define __CSPSIM_LOGOSCREEN_H__


#include <csp/cspsim/BaseScreen.h>

#include <osg/ref_ptr>

namespace osg {
	class Camera;
	class Texture2D;
}


namespace csp {

/** A screen for displaying a series of static images.
 *
 *  TODO This class is currently specialized to display a fixed set of images
 *  during startup, but should be generalized.
 */
class LogoScreen : public BaseScreen {
public:
	LogoScreen();
	virtual ~LogoScreen();
 
	virtual void onInit();
	virtual void onExit();

	virtual void onUpdate(double dt);

private:
	osg::ref_ptr<osg::Camera> m_Camera;
	osg::ref_ptr<osg::Texture2D> m_Texture;
};

} // namespace csp

#endif // __CSPSIM_LOGOSCREEN_H__

