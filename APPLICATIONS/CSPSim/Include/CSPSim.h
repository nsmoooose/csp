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
 * @file CSPSim.h
 *
 * Provides CSPSim, the primary simulation engine.
 * 
 */

#ifndef __CSPSIM_H__
#define __CSPSIM_H__


#include <SimData/Ref.h>
#include <SimData/Date.h>
#include <SimData/DataManager.h>

#include "Atmosphere.h"

#include <osg/ref_ptr>

#include <SDL/SDL_joystick.h>

class SDL_Surface;

class PyShell;
class DynamicObject;
class TerrainObject;
class VirtualBattlefield;
class VirtualScene;
class VirtualHID;
class BaseScreen;
class GameScreen;
class EventMapIndex;
class PyConsole;


void fillerup(void *unused, unsigned char *stream, int len);


/**
 * class CSPSim - The primary simulation engine for CSP.
 *
 * @author Wolverine
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class CSPSim
{
public:
	static CSPSim *theSim;
    
	CSPSim();
	virtual ~CSPSim();
	virtual void init();
	virtual void run();
	virtual void quit();
	virtual void cleanup();

	void changeScreen(BaseScreen * newScreen);
	SDL_Surface * getSDLScreen() {return m_SDLScreen;};

	simdata::SimDate & getCurrentTime() { return m_CurrentTime; }
	simdata::SimTime const & getFrameRate() const{ return m_FrameRate; }

	void setActiveObject(simdata::Ref<DynamicObject> object);
	simdata::Ref<DynamicObject> getActiveObject() const;
	VirtualBattlefield * getBattlefield();
	VirtualBattlefield const * getBattlefield() const;
	VirtualScene * getScene();
	VirtualScene const * getScene() const;

	EventMapIndex *getInterfaceMaps() { return m_InterfaceMaps; }

	void togglePause();
	void runConsole(PyConsole *console);
	void endConsole();
	bool isPaused() { return m_Paused; }

	simdata::DataManager & getDataManager() { return m_DataManager; }

	Atmosphere const * getAtmosphere() const { return &m_Atmosphere; }

	simdata::Ref<PyShell> getShell() const;

protected:
	
	void initSim();
	int initSDL();

	void doInput(double dt);
	void updateObjects(double dt);
	void doStartupScript();


private:

	SDL_Surface *m_SDLScreen;
	SDL_Joystick *m_SDLJoystick;

	BaseScreen *m_CurrentScreen;
	BaseScreen *m_PrevScreen;

	BaseScreen *m_LogoScreen;
	GameScreen *m_GameScreen;
	BaseScreen *m_MainMenuScreen;

	int m_ScreenWidth;
	int m_ScreenHeight;

	bool m_Paused;
	bool m_Finished;
	bool m_ConsoleOpen;
	bool m_Clean;

	/**
	 * The current simulation time/date
	 */
	simdata::SimDate m_CurrentTime;
	simdata::SimTime m_FrameTime;
	simdata::SimTime m_FrameRate;
	simdata::SimTime m_AverageFrameTime;
	simdata::SimTime m_ElapsedTime;
	simdata::SimTime m_TimeLag;

	void initTime(simdata::SimDate const &);
	void updateTime();

	/**
	 * The current input device interface
	 */
	VirtualHID *m_Interface;
	EventMapIndex *m_InterfaceMaps;
	simdata::Ref<DynamicObject> m_ActiveObject;

	/**
	 * The virtual battlefield
	 */
	simdata::Ref<VirtualBattlefield> m_Battlefield;
	simdata::Ref<VirtualScene> m_Scene;

	// TODO the terrain will eventually be encapsulated in a Theater class
	simdata::Ref<TerrainObject> m_ActiveTerrain;
	simdata::DataManager m_DataManager;
	Atmosphere m_Atmosphere;

	//PyObject* m_Console;
	osg::ref_ptr<PyConsole> m_Console;
	simdata::Ref<PyShell> m_Shell;

};

#endif // __CSPSIM_H__

