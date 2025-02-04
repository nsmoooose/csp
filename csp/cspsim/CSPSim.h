#pragma once
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

#include <csp/cspsim/Export.h>
#include <csp/cspsim/SDLViewer.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/data/Date.h>
#include <csp/cspwf/Signal.h>

#include <osg/ref_ptr>

namespace csp {

class BaseScreen;
class Client;
class DataManager;
class DynamicObject;
class GameScreen;
class LocalBattlefield;
class TerrainObject;
class Theater;
class VirtualScene;

namespace input {
	class EventMapIndex;
	class VirtualHID;
}

namespace config { class Configuration; }
namespace weather { class Atmosphere; }

namespace wf { class WindowManager; }

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
	/** Initializes all objects needed to run the simulator. */
	virtual void loadSimulation();
	/** Unloads all objects used during a simulation. */
	virtual void unloadSimulation();
	virtual void unloadSimulationNow();
	virtual void displayLogoScreen();
	virtual void displayMenuScreen();
	virtual void run();
	virtual void quit();
	virtual void cleanup();
	
	virtual osg::Group* getSceneData();
	virtual wf::WindowManager* getWindowManager();
	virtual wf::Signal* getConfigurationChangedSignal();
	virtual config::Configuration* getConfiguration();
	virtual void setConfiguration(config::Configuration* config); 

	void changeScreen(BaseScreen * newScreen);
	BaseScreen* getCurrentScreen();

	SimDate & getCurrentTime() { return m_CurrentTime; }
	void setCurrentTime(SimDate const &);
		
	SimTime const & getFrameRate() const{ return m_FrameRate; }
	SimTime const & getFrameTime() const{ return m_FrameTime; }
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
	Ref<input::EventMapIndex> getInterfaceMaps() const;
	Ref<input::VirtualHID> getActiveObjectInterface() const;

	void togglePause();
	bool isPaused() { return m_Paused; }

	DataManager & getDataManager() { return *m_DataManager; }

	weather::Atmosphere *getAtmosphere() const { return m_Atmosphere.get(); }
	
protected:

	void setWfResourceLocator();

private:
	osg::ref_ptr<SDLViewer> m_Viewer;
	Ref<wf::WindowManager> m_WindowManager;
	osg::ref_ptr<osg::Group> m_VirtualSceneGroup;
	Ref<wf::Signal> m_ConfigurationChanged;
	
	Ref<BaseScreen> m_CurrentScreen;
	Ref<config::Configuration> m_Configuration;

	bool m_Paused;
	bool m_Finished;
	bool m_Clean;
	bool m_UnloadSimulationRequested;

	/** The current simulation time/date */
	SimDate m_CurrentTime;
	/** The time of the current frame */
	SimTime m_FrameTime;
	/** The Frames Per Second rate */
	SimTime m_FrameRate;
	/** The average frame time */
	SimTime m_AverageFrameTime;
	/** The elapsed time */
	SimTime m_ElapsedTime;
	/** The time lag */
	SimTime m_TimeLag;

	void updateTime();

	/** The current input device interface */
	Ref<input::VirtualHID> m_Interface;
	/** The input interface maps */
	Ref<input::EventMapIndex> m_InterfaceMaps;
	/** The active object */
	Ref<DynamicObject> m_ActiveObject;

	/** 
	 * The virtual battlefield
	 */
	Ref<LocalBattlefield> m_Battlefield;
	Ref<VirtualScene> m_Scene;
	Ref<Client> m_NetworkClient;

	/**
	 * @TODO the terrain will eventually be encapsulated in a Theater class
	 */
	Ref<Theater> m_Theater;
	Ref<TerrainObject> m_Terrain;
	Ref<DataManager> m_DataManager;
	Ref<weather::Atmosphere> m_Atmosphere;
};

} // namespace csp
