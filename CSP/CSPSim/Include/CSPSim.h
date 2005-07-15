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

/**
 * @mainpage CSPSim - Combat Simulator Project
 *
 * @section overview Overview
 *
 * The Combat Simulator Project is an open-source community united by a
 * common interest in combat simulator development.  We are developing
 * standards, technologies, code libraries, tools and artwork that can be
 * used to build a wide variety of real-time combat simulators.  Our
 * ultimate goal is to realize concurrent and networked cross-platform
 * simulation of large-scale multi-vehicle combat scenarios.
 *
 * CSPSim is a simulator testbed used to develop the infrastructure of
 * CSP.  It is currently a functional flight simulator, using OpenGL
 * rendering under both Microsoft Windows and GNU/Linux environments.
 * For more details about current development efforts, please visit
 * the <a href='csp.sourceforge.net/forum'>CSP Forums</a>.
 *
 */

#ifndef __CSPSIM_H__
#define __CSPSIM_H__

#include <SimData/Ref.h>
#include <SimData/ScopedPointer.h>
#include <SimData/Date.h>

#include <osg/ref_ptr>

struct SDL_Surface;
typedef struct _SDL_Joystick SDL_Joystick;

class Atmosphere;
//--namespace Producer { class RenderSurface; }
namespace simdata { class DataManager; }
namespace simnet { class Client; }
class InputEvent;
class PyShell;
class DynamicObject;
class TerrainObject;
class Theater;
class LocalBattlefield;
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
	simdata::SimTime const & getElapsedTime() const { return m_ElapsedTime; }
	
	void setActiveObject(simdata::Ref<DynamicObject> object);
	simdata::Ref<DynamicObject> getActiveObject() const;
	LocalBattlefield * getBattlefield();
	LocalBattlefield const * getBattlefield() const;
	TerrainObject * getTerrain();
	TerrainObject const * getTerrain() const;
	VirtualScene * getScene();
	VirtualScene const * getScene() const;
	simdata::Ref<Theater> getTheater() const;
	simdata::Ref<EventMapIndex> getInterfaceMaps() const;

/*WNET
	NetworkMessenger * getNetworkMessenger();
*/

	void togglePause();
	void runConsole(PyConsole *console);
	void endConsole();
	bool isPaused() { return m_Paused; }

	simdata::DataManager & getDataManager() { return *m_DataManager; }

	Atmosphere const * getAtmosphere() const { return m_Atmosphere.get(); }

	simdata::Ref<PyShell> getShell() const;


protected:

	void initSim();
	int initSDL();

	void doInput(double dt);
	void updateObjects(double dt);
	void doStartupScript();


private:

	SDL_Surface *m_SDLScreen;
	SDL_Joystick* m_SDLJoystick;

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
	simdata::Ref<VirtualHID> m_Interface;
	simdata::Ref<EventMapIndex> m_InterfaceMaps;
	simdata::Ref<DynamicObject> m_ActiveObject;

	/**
	 * The virtual battlefield
	 */
	simdata::Ref<LocalBattlefield> m_Battlefield;
	simdata::Ref<VirtualScene> m_Scene;
	simdata::Ref<simnet::Client> m_NetworkClient;

	// TODO the terrain will eventually be encapsulated in a Theater class
	simdata::Ref<Theater> m_Theater;
	simdata::Ref<TerrainObject> m_Terrain;
	simdata::Ref<simdata::DataManager> m_DataManager;
	simdata::ScopedPointer<Atmosphere> m_Atmosphere;

	//PyObject* m_Console;
	//osg::ref_ptr<PyConsole> m_Console;
	simdata::Ref<PyShell> m_Shell;
	
	//--osg::ref_ptr<Producer::RenderSurface> m_RenderSurface;

	simdata::ScopedPointer<InputEvent> m_InputEvent;
};

#endif // __CSPSIM_H__

