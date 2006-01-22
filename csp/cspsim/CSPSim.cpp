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
 * @file CSPSim.cpp
 *
 */


#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/AnimationSequence.h>
#include <csp/cspsim/Atmosphere.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/EventMapIndex.h>
#include <csp/cspsim/Exception.h>
#include <csp/cspsim/GameScreen.h>
#include <csp/cspsim/HID.h>
#include <csp/cspsim/InputEvent.h>
#include <csp/cspsim/LogoScreen.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Profile.h>
#include <csp/cspsim/SimpleSceneManager.h>
#include <csp/cspsim/SoundEngine.h>
#include <csp/cspsim/TerrainObject.h>
#include <csp/cspsim/Theater.h>
#include <csp/cspsim/VirtualScene.h>

#include <csp/cspsim/battlefield/LocalBattlefield.h>
#include <csp/cspsim/battlefield/SimObject.h>

#include <csp/cspsim/stores/StoresDatabase.h>

#include <csp/csplib/data/GeoPos.h>
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/data/Types.h>
#include <csp/csplib/net/ClientServer.h>
#include <csp/csplib/sound/Loader.h>
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Timing.h>
#include <csp/csplib/util/Trace.h>

#include <csp/modules/demeter/DemeterException.h>

#ifndef __APPLE__
#include <GL/gl.h>   // Header File For The OpenGL32 Library
#include <GL/glu.h>  // Header File For The GLu32 Library
#else // apple
#include <OpenGL/gl.h>   // Header File For The OpenGL32 Library
#include <OpenGL/glu.h>  // Header File For The GLu32 Library
#endif

#include <osg/Timer>
#include <osg/Notify>
#include <osgAL/SoundManager>
//--#include <Producer/RenderSurface>

#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>


CSP_NAMESPACE

// For network testing on a single box it's convenient to disable rendering on
// one of the sims.  Using a global here just because we're lazy.
bool g_DisableRender = false;



/**
 * static access to the CSPSim.  CSPSim should be used as a singleton,
 * although this is not enforced.
 */
CSPSim *CSPSim::theSim = 0;

CSPSim::CSPSim():
	m_DataManager(new DataManager),
	m_Atmosphere(new Atmosphere),
	//--m_RenderSurface(new Producer::RenderSurface),
	m_InputEvent(new InputEvent)
{
	if (theSim == 0) {
		theSim = this;
	}

	int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
	log().setCategories(cLogCategory_ALL);
	log().setPriority(level);
	std::string logfile = g_Config.getString("Debug", "LogFile", "CSPSim.log", true);
	log().logToFile(logfile);

	g_DisableRender = g_Config.getBool("Debug", "DisableRender", false, false);

	CSPLOG(DEBUG, APP) << "Constructing CSPSim object";

	m_Clean = true;

	m_Paused = true;
	m_Finished = false;

	m_CurrentScreen = NULL;
	m_PrevScreen = NULL;

	m_GameScreen = NULL;

	m_FrameTime = 0.05;
	m_FrameRate = 20.0;
	m_ElapsedTime = 0.0;
}


CSPSim::~CSPSim() {
	assert(m_Clean);
	if (theSim == this) {
		theSim = 0;
	}
}

void CSPSim::setActiveObject(Ref<DynamicObject> object) {
	if (object.valid()) {
		CSPLOG(INFO, APP) << "Setting active object to " << *object;
	} else {
		CSPLOG(INFO, APP) << "Deselecting active object";
	}

	if (object == m_ActiveObject) return;
	if (m_ActiveObject.valid()) {
		// The new battlefield code no longer supports this interface.  It could be
		// reimplemented for local objects, although we probably need a better long
		// term solution for vehicle allocation.  It's also a bit tricky since the
		// aggregation bubbles are determined by human-controlled vehicles.
		// XXX XXX m_Battlefield->setHuman(m_ActiveObject->id(), false);
	}
	m_ActiveObject = object;
	if (m_GameScreen) {
		m_GameScreen->setActiveObject(m_ActiveObject);
	}
	if (m_ActiveObject.valid()) {
		// The new battlefield code no longer supports this interface.  It could be
		// reimplemented for local objects, although we probably need a better long
		// term solution for vehicle allocation.  It's also a bit tricky since the
		// aggregation bubbles are determined by human-controlled vehicles.
		// XXX XXX m_Battlefield->setHuman(m_ActiveObject->id(), true);
		hasht classhash = m_ActiveObject->getPath();
		CSPLOG(DEBUG, APP) << "Getting input interface map for " << classhash.str();
		Ref<EventMapping> map = m_InterfaceMaps->getMap(classhash);
		CSPLOG(INFO, APP) << "Selecting map @ " << map.get();
		m_Interface->setMapping(map);
	}
	m_Interface->bindObject(m_ActiveObject.get());
}

Ref<DynamicObject> CSPSim::getActiveObject() const {
	return m_ActiveObject;
}

LocalBattlefield * CSPSim::getBattlefield() {
	return m_Battlefield.get();
}

LocalBattlefield const * CSPSim::getBattlefield() const {
	return m_Battlefield.get();
}

VirtualScene * CSPSim::getScene() {
	return m_Scene.get();
}

VirtualScene const * CSPSim::getScene() const {
	return m_Scene.get();
}

void CSPSim::togglePause() {
	m_Paused = !m_Paused;
}

//OpenThreads::Barrier bar;

void CSPSim::init() {
	try {
		CSPLOG(INFO, APP) << "Installing stack trace handler";
		AutoTrace::install();

		CSPLOG(INFO, APP) << "Beginning initialization";
		osg::setNotifyLevel(osg::WARN);

		// setup osg search path for external data files
		std::string image_path = getDataPath("ImagePath");
		std::string model_path = getDataPath("ModelPath");
		std::string font_path = getDataPath("FontPath");
		std::string search_path;
		ospath::addpath(search_path, image_path);
		ospath::addpath(search_path, model_path);
		ospath::addpath(search_path, font_path);
		ObjectModel::setDataFilePathList(search_path);

		// open the primary data archive
		std::string cache_path = getCachePath();
		std::string archive_file = ospath::join(cache_path, "sim.dar");
		try {
			DataArchive *sim = new DataArchive(archive_file.c_str(), 1);
			assert(sim);
			m_DataManager->addArchive(sim);
		}
		catch (Exception &e) {
			CSPLOG(ERROR, APP) << "Error opening data archive " << archive_file;
			CSPLOG(ERROR, APP) << e.getType() << ": " << e.getMessage();
			throw e;
			//::exit(0);
		}

		if (initSDL()) {
			::exit(1);  // error already logged
		}

		//--m_RenderSurface->setWindowRectangle(-1, -1, m_ScreenWidth, m_ScreenHeight);
		//--m_RenderSurface->setWindowName("CSPSim");
		//--m_RenderSurface->fullScreen(0);
		//--m_RenderSurface->realize();
		SDL_WM_SetCaption("CSPSim", "");

		LogoScreen logoScreen(m_ScreenWidth, m_ScreenHeight);
		logoScreen.onInit();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// should run in its own thread
		logoScreen.onRender();
		SDL_GL_SwapBuffers();
		//--m_RenderSurface->swapBuffers();
		logoScreen.run();

		m_Clean = false;

		// load all interface maps and create a virtual hid for the active object
		CSPLOG(DEBUG, APP) << "Initializing input event maps";
		m_InterfaceMaps = new EventMapIndex();
		m_InterfaceMaps->loadAllMaps();
		m_Interface = new VirtualHID();

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();


		CSPLOG(DEBUG, APP) << "Initializing theater";
		std::string theater = g_Config.getPath("Testing", "Theater", "sim:theater.balkan", false);
		m_Theater = m_DataManager->getObject(theater.c_str());
		assert(m_Theater.valid());
		//CSP_VERIFY(m_Theater->initialize(*m_DataManager));
		m_Terrain = m_Theater->getTerrain();
		assert(m_Terrain.valid());
		m_Terrain->setScreenSizeHint(m_ScreenWidth, m_ScreenHeight);
		m_Terrain->activate();

		// configure the atmosphere for the theater location
		// this affects mean temperatures, and should not need
		// to be updated for motion within a given theater.
		double lat = m_Terrain->getCenter().latitude();
		double lon = m_Terrain->getCenter().longitude();
		m_Atmosphere->setPosition(lat, lon);
		
		// TODO may need a reset method when we swap theaters
		StoresDatabase::getInstance().load(*m_DataManager, "sim:stores");

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSPLOG(DEBUG, APP) << "Initializing sound system";
		SoundEngine::getInstance().initialize();
		SoundEngine::getInstance().mute();
		SoundFileLoader::init();

		CSPLOG(DEBUG, APP) << "Initializing scene graph";
		m_Scene = new VirtualScene(m_ScreenWidth, m_ScreenHeight);
		m_Scene->buildScene();
		m_Scene->setTerrain(m_Terrain);

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		// get view parameters from configuration file.  ultimately there should
		// be an in-game ui for this and probably a separate config file.
		bool wireframe = g_Config.getBool("View", "Wireframe", false, true);
		m_Scene->setWireframeMode(wireframe);
		int view_distance = g_Config.getInt("View", "ViewDistance", 35000, true);
		m_Scene->setViewDistance(view_distance);
		bool fog = g_Config.getBool("View", "Fog", true, true);
		m_Scene->setFogMode(fog);
		int fog_start = g_Config.getInt("View", "FogStart", 20000, true);
		m_Scene->setFogStart(fog_start);
		int fog_end = g_Config.getInt("View", "FogEnd", 35000, true);
		m_Scene->setFogEnd(fog_end);

		CSPLOG(DEBUG, APP) << "Initializing battlefield";
		int visual_radius = g_Config.getInt("Testing", "VisualRadius",  40000, true);
		m_Battlefield = new LocalBattlefield(m_DataManager);
		m_Battlefield->setSceneManager(new SimpleSceneManager(m_Scene, visual_radius));
		if (m_Theater.valid()) {
			FeatureGroup::Ref::list groups = m_Theater->getAllFeatureGroups();
			CSPLOG(DEBUG, BATTLEFIELD) << "Adding " << groups.size() << " features to the battlefield";
			for (FeatureGroup::Ref::list::iterator iter = groups.begin(); iter != groups.end(); ++iter) {
				m_Battlefield->addStatic(*iter);
			}
		}

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		// create the networking layer
		if (g_Config.getBool("Networking", "UseNetworking", false, true)) {
			CSPLOG(DEBUG, APP) << "Initializing network layer";
			std::string netlogfile = g_Config.getString("Debug", "NetLogFile", "SimNet.log", true);
			netlog().logToFile(netlogfile);
			netlog().setPriority(cLogPriority_INFO);
			std::string default_ip = NetworkNode().getIpString();
			std::string local_address = g_Config.getString("Networking", "LocalIp", default_ip, true);
			int local_port = g_Config.getInt("Networking", "LocalPort", 3161, true);
			NetworkNode local_node(local_address, local_port);
			CSPLOG(INFO, NETWORK) << "Initializing network interface " << local_address << ":" << local_port;

			int incoming_bw = g_Config.getInt("Networking", "IncomingBandwidth", 36000, true);
			int outgoing_bw = g_Config.getInt("Networking", "OutgoingBandwidth", 36000, true);
			m_NetworkClient = new Client(local_node, incoming_bw, outgoing_bw);

			std::string server_address = g_Config.getString("Networking", "ServerIp", default_ip, true);
			int server_port = g_Config.getInt("Networking", "ServerPort", 3160, true);
			CSPLOG(INFO, NETWORK) << "Connecting to server: " << server_address << ":" << server_port;
			NetworkNode server_node(server_address, server_port);
			if (!m_NetworkClient->connectToServer(server_node, 5.0 /*seconds*/)) {
				std::cerr << "Unable to connecting to server, running in local mode\n";
				CSPLOG(ERROR, NETWORK) << "Unable to connecting to server, running in local mode";
				m_NetworkClient = 0;
			} else {
				std::string name = g_Config.getString("Networking", "UserName", "anonymous", true);
				CSPLOG(INFO, NETWORK) << "Connecting to server battlefield as " << name;
				m_Battlefield->setNetworkClient(m_NetworkClient);
				m_Battlefield->connectToServer(name);
				Timer timer;
				timer.start();
				while (timer.elapsed() < 5.0 && !m_Battlefield->isConnectionActive()) {
					m_NetworkClient->processAndWait(0.01, 0.01, 0.1);
				}
				if (!m_Battlefield->isConnectionActive()) {
					// connection failed, go back to local mode
					m_Battlefield->setNetworkClient(0);
				}
			}
			netlog().setPriority(cLogPriority_INFO);
		}

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSPLOG(DEBUG, APP) << "Initializing gamescreen";

		// Following variables should be set before calling GameScreen.init()
		// because they are used in GameScreen initialization process

		m_Paused = false;  // enable/disable pause at startup

		// create and initialize screens
		m_GameScreen = new GameScreen;
		m_GameScreen->onInit();

		changeScreen(m_GameScreen);
	}
	catch (Exception &e) {
		FatalException(e, "initialization");
	}
	catch (DemeterException &e) {
		DemeterFatalException(e, "initialization");
	}
	/*
	catch (...) {
		csp::OtherFatalException("initialization");
	}
	*/

}


void CSPSim::cleanup() {
	CSPLOG(INFO, APP) << "Cleaning up resources";

	assert(m_Battlefield.valid());
	assert(m_Scene.valid());
	assert(m_Terrain.valid());
	assert(m_GameScreen);
	setActiveObject(NULL);
	delete m_GameScreen;
	m_GameScreen = NULL;
	m_InterfaceMaps = NULL;
	m_Terrain->deactivate();
	m_Battlefield = NULL;
	m_Terrain = NULL;
	m_Theater = NULL;
	m_Scene = NULL;
 	if (m_SDLJoystick) {
		SDL_JoystickClose(m_SDLJoystick);
		m_SDLJoystick = NULL;
	}

	// release cached objects.  this must be done before the sound engine is shut
	// down to prevent errors when deleting cached sound samples.
	m_DataManager = 0;

	SoundEngine::getInstance().shutdown();
	SDL_Quit();
	m_Clean = true;
}


void CSPSim::quit() {
	CSPLOG(INFO, APP) << "Quit requested";
	m_Finished = true;
	SoundEngine::getInstance().mute();
}


void CSPSim::changeScreen(BaseScreen * newScreen) {
	CSPLOG(DEBUG, APP) << "Changing screen";
	m_PrevScreen = m_CurrentScreen;
	m_CurrentScreen = newScreen;
}


// Main Game loop
void CSPSim::run() {
	float low_priority = 0.0;
	int idx = 0;

	CSPLOG(DEBUG, APP) << "Initializing simulation time";

	std::string date_string = g_Config.getString("Testing", "Date", "2000-01-01 00:00:00.0", true);
	SimDate date;
	try {
		date.parseXML(date_string.c_str());
	}
	catch (...) {
		std::cerr << "Invalid starting date in INI file (Testing:Date).\n" << std::endl;
		::exit(1);
	}

	// XXX the first call of these routines typically takes a few seconds.
	// calling them here avoids a time jump at the start of the simloop.
	if (m_CurrentScreen) {
		m_CurrentScreen->onUpdate(0.01);
		m_CurrentScreen->onRender();
	}

	initTime(date);
	
	m_Atmosphere->setDate(date);
	m_Atmosphere->reset();

	Timer time_object_update;
	Timer time_render;
	bool lopri = false;

	SoundEngine::getInstance().unmute();

	CSPLOG(INFO, APP) << "Entering main simulation loop";
	try {
		while (!m_Finished) {
			CSPLOG(DEBUG, APP) << "Starting simulation loop iteration";

			if (m_NetworkClient.valid()) {
				m_NetworkClient->processIncoming(0.01);
			}

			PROF0(_simloop);

			updateTime();
			float dt = m_FrameTime;

			// Do Input loop
			PROF0(_input);
			doInput(dt);
			PROF1(_input, 60);

			// Miscellaneous Updates
			low_priority += dt;
			lopri = false;
			if (low_priority > 0.33) {
				switch (idx++) {
					case 0:
						lopri = true;
						m_Atmosphere->update(low_priority);
						break;
					default:
						idx = 0;
				}
				low_priority = 0.0;
			}

			// Update Objects if sim is not frozen
			PROF0(_objects);
			if (!m_Paused) {
				//CSPLOG(ERROR, APP) << "update objects";
				time_object_update.start();
				updateObjects(dt);
				time_object_update.stop();
				//CSPLOG(ERROR, APP) << "update objects done";
			}
			PROF1(_objects, 60);
			
			// Display (render) current Screen
			if (m_CurrentScreen) {
				PROF0(_screen_update);
				m_CurrentScreen->onUpdate(dt);
				PROF1(_screen_update, 60);
				PROF0(_screen_render);
				time_render.start();
				if (!g_DisableRender) {
					m_CurrentScreen->onRender();
				}
				time_render.stop();
				PROF1(_screen_render, 60);
			}

			if (m_NetworkClient.valid()) {
				m_NetworkClient->processOutgoing(0.01);
			}

			// Swap OpenGL buffers
#ifndef __CSPSIM_EXE__
			Py_BEGIN_ALLOW_THREADS;
			//--m_RenderSurface->swapBuffers();
			SDL_GL_SwapBuffers();
			Py_END_ALLOW_THREADS;
#else
			//--m_RenderSurface->swapBuffers();
			SDL_GL_SwapBuffers();
#endif
			PROF1(_simloop, 30);

			if (time_render.elapsed() + time_object_update.elapsed() > 0.05) {
				//std::cout << "long frame: update=" << time_object_update.elapsed() << " render=" << time_render.elapsed() << " lo=" << lopri << "\n";
			}
		}
		//m_Battlefield->dumpObjectHistory();
	}
	catch (DemeterException *e) {
		std::string msg = e->GetErrorMessage();
		CSPLOG(ERROR, APP) << "Caught demeter exception: " << msg ;
		cleanup();
		::exit(1);
	}
	catch (Exception &e) {
		FatalException(e, "mainloop");
	}
	catch (...) {
		CSPLOG(ERROR, APP) << "Caught unexpected (and unknown) exception";
		CSPLOG(ERROR, APP) << "glError is " << glGetError();
		cleanup();
		::exit(1);
	}
}


void CSPSim::initTime(SimDate const &date) {
	m_CurrentTime = date;
	m_CurrentTime.update();
	m_ElapsedTime = 0.0;
	m_TimeLag = 0.00001;
	m_AverageFrameTime = 1.0;
}


// update time from the current frame update and the previous one.
// the timestep for updates is restricted to 1 second max.  greater
// delays will accumulate in _timeLag to be made up in subsequent
// frames.  this prevents long delays from destabilizing the update
// computations.
void CSPSim::updateTime() {
	m_FrameTime = m_CurrentTime.update();
	m_ElapsedTime += m_FrameTime;
	m_FrameTime += m_TimeLag;
	assert(m_FrameTime > 0.0);
	if (m_FrameTime > 0.2) {
		m_TimeLag = m_FrameTime - 0.2;
		m_FrameTime = 0.2;
	} else {
		m_TimeLag = 0.0;
	}
	if (m_FrameTime < 0.01) {
		// todo: microsleep
		m_FrameTime = 0.01;
	}
	// smooth out the fluctuations a bit
	m_AverageFrameTime = 0.95 * m_AverageFrameTime + 0.5 * m_FrameTime;
	if (m_FrameTime < 0.005) {
		m_FrameRate = 200.0;
	} else {
		m_FrameRate = 1.0 / m_FrameTime;
	}
}

void CSPSim::doInput(double dt) {
	CSPLOG(DEBUG, APP) << "Checking for input events";

	Ref<VirtualHID> screen_interface = m_CurrentScreen->getInterface();

	SDL_Event event;
	short doPoll = 10;
	while (doPoll-- && (*m_InputEvent)(event)) {
	//while (doPoll-- && SDL_PollEvent(&event)) {
		bool handled = false;
		HID::translate(event);
		if (event.type == SDL_QUIT) {
			m_Finished = true;
			return;
		}
		if (!handled && m_CurrentScreen) {
			if (screen_interface.valid()) {
				handled = screen_interface->onEvent(event);
			}
		}
		if (!handled && m_Interface.valid()) {
			CSPLOG(DEBUG, APP) << "Passing event to the active object";
			handled = m_Interface->onEvent(event);
		}
	}

	// run input scripts
	if (screen_interface.valid()) {
		screen_interface->onUpdate(dt);
	}
	if (m_Interface.valid()) {
		m_Interface->onUpdate(dt);
	}
}

/**
 * Update all objects, calling physics and AI routines.
 */
void CSPSim::updateObjects(double dt) {
	CSPLOG(DEBUG, APP) << "Updating all objects";

	if (m_Battlefield.valid()) {
		m_Battlefield->update(dt);
	}
	if (m_Scene.valid()) {
		m_Scene->onUpdate(dt);
	}
}


int CSPSim::initSDL() {
	CSPLOG(DEBUG, APP) << "Initializing SDL";

	int height = g_Config.getInt("Screen", "Height", 768, true);
	int width = g_Config.getInt("Screen", "Width", 1024, true);
	int fullscreen = g_Config.getInt("Screen", "FullScreen", 0, true);

	m_ScreenHeight = height;
	m_ScreenWidth = width;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
		std::cerr << "Unable to initialize SDL (" << SDL_GetError() << ")\n";
		CSPLOG(ERROR, APP) << "Unable to initialize SDL (" << SDL_GetError() << ")";
		return 1;
	}

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	int bpp = info->vfmt->BitsPerPixel;

	CSPLOG(INFO, APP) << "Initializing video at " << bpp << " bits per pixel.";

	Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF;

	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}

	m_SDLScreen = SDL_SetVideoMode(width, height, bpp, flags);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (m_SDLScreen == NULL) {
		std::cerr << "Unable to set video mode (" << SDL_GetError() << ")\n";
		CSPLOG(ERROR, APP) << "Unable to set video mode (" << SDL_GetError() << ")";
		return 1;
	}

	SDL_JoystickEventState(SDL_ENABLE);
	m_SDLJoystick = SDL_JoystickOpen(0);
	if (m_SDLJoystick == NULL) {
		CSPLOG(ERROR, APP) << "Failed to open joystick (" <<  SDL_GetError() << ")";
	}

	SDL_EnableUNICODE(1);

	// make sure SDL_Quit gets called when the program exits.
	atexit(SDL_Quit);

	return 0;
}

Ref<Theater> CSPSim::getTheater() const {
	return m_Theater;
}

TerrainObject * CSPSim::getTerrain() {
	return m_Terrain.get();
}

TerrainObject const * CSPSim::getTerrain() const {
	return m_Terrain.get();
}

Ref<EventMapIndex> CSPSim::getInterfaceMaps() const {
	return m_InterfaceMaps;
}

CSP_NAMESPACE_END

