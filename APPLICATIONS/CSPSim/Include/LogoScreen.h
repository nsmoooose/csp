// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002-2004 The Combat Simulator Project
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


#include "BaseScreen.h"

#include <osg/ref_ptr>
#include <osgProducer/Viewer>

namespace osg {
	class Texture2D;
};

namespace osgUtil {
	class SceneView;
};



//struct SDL_Surface;

/**
 * class LogoScreen
 *
 * @author unknown
 */
class LogoScreen : public BaseScreen { //, public osgProducer::Viewer, public virtual OpenThreads::Thread {
public:
	LogoScreen(int width, int height);
	virtual ~LogoScreen();
 
	virtual void onInit();
	virtual void onExit();

	virtual void onRender();
	virtual void onUpdate(double dt);

	void run();
	void stop();

private:
	osg::ref_ptr<osgUtil::SceneView> m_LogoView; 
	osg::ref_ptr<osg::Texture2D> m_Texture;
	osgProducer::Viewer m_Viewer;
	//SDL_Surface * m_image;
	int m_width, m_height;
};


#endif // __LOGOSCREEN_H__

