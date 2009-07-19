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

#include <csp/cspsim/SDLViewer.h>
#include <csp/cspsim/SDLGraphicsWindow.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/BaseScreen.h>
#include <csp/cspsim/SDLEventHandler.h>
#include <csp/cspsim/VirtualScene.h>
#include <csp/cspsim/battlefield/LocalBattlefield.h>
#include <csp/cspsim/weather/Atmosphere.h>
#include <csp/csplib/util/Log.h>
#include <SDL/SDL.h>

namespace csp {

bool SDLViewer::setUpWindow(const char *caption, const ScreenSettings & screenSettings)
{
	setKeyEventSetsDone(0);
	if ( !setUpViewerAsSDLGraphicsWindow(caption, screenSettings) ) return false;
	//if ( !setUpViewerAsOSGGraphicsWindow(caption, screenSettings) ) return false;
	m_eventHandler = new SDLEventHandler();
	addEventHandler( m_eventHandler ); // Our base class has the responsibility to destroy m_eventHandler
	getCamera()->setClearMask(0);
	return true;
}

bool SDLViewer::setUpViewerAsSDLGraphicsWindow(const char *caption, const ScreenSettings & screenSettings)
{
    // inspired by osgViewer::Viewer::setUpViewerAsEmbeddedInWindow()
	setThreadingModel( SingleThreaded );
    SDLGraphicsWindow * gw = new SDLGraphicsWindow(caption, screenSettings);
    getCamera()->setViewport( new osg::Viewport(0, 0, screenSettings.width, screenSettings.height) );
	getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(screenSettings.width)/static_cast<double>(screenSettings.height), 1.0f, 10000.0f);
	getCamera()->setGraphicsContext( gw );
	return gw->valid();
}

bool SDLViewer::setUpViewerAsOSGGraphicsWindow(const char *caption, const ScreenSettings & screenSettings)
{
	if ( screenSettings.fullScreen )
	{
		setUpViewOnSingleScreen();
	}
	else
	{
		setUpViewInWindow(50, 50, screenSettings.width, screenSettings.height);
	}
	return true;
}

void SDLViewer::eventTraversal()
{
	// Traverse osg events
	osgViewer::Viewer::eventTraversal();

	// Traverse SDL Joystick events
	pollSdlEvents();

	// Run input scripts
	m_eventHandler->runInputScripts();
}

void SDLViewer::updateTraversal()
{
	// Update CSP
	updateAtmosphere();
	updateObjects();
	updateCurrentScreen();

	// Traverse osg updaters
	osgViewer::Viewer::updateTraversal();
}

void SDLViewer::pollSdlEvents()
{
	CSPLOG(DEBUG, APP) << "Checking for SDL input events";

	SDL_Event event;
	short doPoll = 10;
	while ( doPoll-- && SDL_PollEvent(&event) )
	{
		if ( event.type == SDL_QUIT )
		{
			CSPSim::theSim->quit();
			return;
		}

		m_eventHandler->handle(event);
	}
}

void SDLViewer::updateAtmosphere()
{
	static float low_priority = 0.0;
	low_priority += CSPSim::theSim->getFrameTime();

	if ( low_priority > 0.66 )
	{
		const weather::Atmosphere * atmosphere = CSPSim::theSim->getAtmosphere();
		const_cast<weather::Atmosphere *>(atmosphere)->update(low_priority);
		low_priority = 0.0;
	}
}

void SDLViewer::updateObjects()
{
	if ( CSPSim::theSim->isPaused() ) return;

	LocalBattlefield * battlefield = CSPSim::theSim->getBattlefield();
	if ( battlefield ) battlefield->update( CSPSim::theSim->getFrameTime() );

	VirtualScene * scene = CSPSim::theSim->getScene();
	if ( scene ) scene->onUpdate( CSPSim::theSim->getFrameTime() );
}

void SDLViewer::updateCurrentScreen()
{
	Ref<BaseScreen> currentScreen = CSPSim::theSim->getCurrentScreen();
	if ( !currentScreen ) return;

	currentScreen->onUpdate( CSPSim::theSim->getFrameTime() );
}

} // namespace csp
