// Combat Simulator Project
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

/**
 * @namespace csp
 * @brief Primary namespace for all internal code (csplib and cspsim).
 */


#ifndef __CSPSIM_H__
#define __CSPSIM_H__

#include <csp/cspsim/Config.h>
#include <csp/cspsim/config/Configuration.h>
#include <csp/cspsim/Export.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/data/Date.h>
#include <csp/cspsim/wf/Signal.h>

#include <osg/ref_ptr>

struct SDL_Surface;
typedef struct _SDL_Joystick SDL_Joystick;

//--namespace Producer { class RenderSurface; }

CSP_NAMESPACE

class Atmosphere;
class BaseScreen;
class Client;
class DataManager;
class DynamicObject;
class EventMapIndex;
class GameScreen;
class InputEvent;
class LocalBattlefield;
class TerrainObject;
class Theater;
class VirtualHID;
class VirtualScene;

/** The primary simulation engine for CSP.  Also acts as a singleton to provide
 *  direct access to shared simulation state.  Do not abuse this access point;
 *  minimizing dependence on the CSPSim instance is a Good Thing.
 */
class CSPSIM_EXPORT CSPSim {
public:
	static CSPSim *theSim;

	CSPSim();
	virtual ~CSPSim();
	virtual void init();
	// Initializes all objects needed to run the simulator.	
	virtual void loadSimulation();
	// Unloads all objects used during a simulation.
	virtual void unloadSimulation();
	virtual void unloadSimulationNow();
	virtual void displayLogoScreen();
	virtual void displayMenuScreen();
	virtual void run();
	virtual void quit();
	virtual void cleanup();
	
	virtual wf::Signal* getConfigurationChangedSignal();
	virtual config::Configuration* getConfiguration();
	virtual void setConfiguration(config::Configuration* config); 

	void changeScreen(BaseScreen * newScreen);
	BaseScreen* getCurrentScreen();
	SDL_Surface * getSDLScreen() {return m_SDLScreen;};

	SimDate & getCurrentTime() { return m_CurrentTime; }
	SimTime const & getFrameRate() const{ return m_FrameRate; }
	SimTime const & getElapsedTime() const { return m_ElapsedTime; }
	
	void setActiveObject(Ref<DynamicObject> object);
	Ref<DynamicObject> getActiveObject() const;
	LocalBattlefield * getBattlefield();
	LocalBattlefield const * getBattlefield() const;
	TerrainObject * getTerrain();
	TerrainObject const * getTerrain() const;
	VirtualScene * getScene();
	VirtualScene const * getScene() const;
	Ref<Theater> getTheater() const;
	Ref<EventMapIndex> getInterfaceMaps() const;

	void togglePause();
	bool isPaused() { return m_Paused; }

	DataManager & getDataManager() { return *m_DataManager; }

	Atmosphere const * getAtmosphere() const { return m_Atmosphere.get(); }
	
protected:

	void initSim();
	int initSDL();

	void doInput(double dt, BaseScreen* currentScreen);
	void updateObjects(double dt);
	void doStartupScript();

private:
	Ref<wf::Signal> m_ConfigurationChanged;
	
	SDL_Surface *m_SDLScreen;
	SDL_Joystick* m_SDLJoystick;

	Ref<BaseScreen> m_CurrentScreen;
	Ref<config::Configuration> m_Configuration;

	ScreenSettings screenSettings;

	bool m_Paused;
	bool m_Finished;
	bool m_Clean;
	bool m_UnloadSimulationRequested;

	// The current simulation time/date
	SimDate m_CurrentTime;
	SimTime m_FrameTime;
	SimTime m_FrameRate;
	SimTime m_AverageFrameTime;
	SimTime m_ElapsedTime;
	SimTime m_TimeLag;

	void initTime(SimDate const &);
	void updateTime();

	// The current input device interface
	Ref<VirtualHID> m_Interface;
	Ref<EventMapIndex> m_InterfaceMaps;
	Ref<DynamicObject> m_ActiveObject;

	// The virtual battlefield
	Ref<LocalBattlefield> m_Battlefield;
	Ref<VirtualScene> m_Scene;
	Ref<Client> m_NetworkClient;

	// TODO the terrain will eventually be encapsulated in a Theater class
	Ref<Theater> m_Theater;
	Ref<TerrainObject> m_Terrain;
	Ref<DataManager> m_DataManager;
	ScopedPointer<Atmosphere> m_Atmosphere;

	//--osg::ref_ptr<Producer::RenderSurface> m_RenderSurface;

	ScopedPointer<InputEvent> m_InputEvent;
};


CSP_NAMESPACE_END

#endif // __CSPSIM_H__

