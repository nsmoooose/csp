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

#include "CSPSim.h"

#include "Animation.h"
#include "Atmosphere.h"
#include "Config.h"
//#include "ConsoleCommands.h"
#include "DynamicObject.h"
#include "EventMapIndex.h"
#include "Exception.h"
#include "GameScreen.h"
#include "HID.h"
#include "InputEvent.h"
#include "MenuScreen.h"
#include "LogoScreen.h"
#include "ObjectModel.h"
#include "Profile.h"
#include "Shell.h"
#include "SimpleSceneManager.h"
#include "TerrainObject.h"
#include "Theater.h"
#include "VirtualScene.h"

#include <SimCore/Battlefield/LocalBattlefield.h>
#include <SimCore/Battlefield/SimObject.h>
#include <SimCore/Util/Log.h>

#include <SimData/Types.h>
#include <SimData/ExceptionBase.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/FileUtility.h>
#include <SimData/GeoPos.h>
#include <SimData/Timing.h>
#include <SimData/Trace.h>

#include <SimNet/ClientServer.h>

#include <GL/gl.h>		// Header File For The OpenGL32 Library
#include <GL/glu.h>		// Header File For The GLu32 Library

#include <osg/Timer>
#include <osg/Notify>
//--#include <Producer/RenderSurface>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_joystick.h>


#include <Demeter/DemeterException.h>


////////////////////////////////////////////////
// FIXME

#include "AircraftObject.h"

//
////////////////////////////////////////////////


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


int g_ScreenWidth = 0;
int g_ScreenHeight = 0;


struct SDLWave {
	SDL_AudioSpec spec;
	Uint8   *sound;			/* Pointer to wave data */
	Uint32   soundlen;		/* Length of wave data */
	int      soundpos;		/* Current play position */
};

SDLWave  m_audioWave;

///////////////////////////////////////////////////////////////////////
// CSPSim


/**
 * static access to the CSPSim.  CSPSim should be used as a singleton,
 * although this is not enforced.
 */
CSPSim *CSPSim::theSim = 0;

CSPSim::CSPSim():
	m_DataManager(new simdata::DataManager),
	m_Atmosphere(new Atmosphere),
	//--m_RenderSurface(new Producer::RenderSurface),
	m_InputEvent(new InputEvent)
{
	if (theSim == 0) {
		theSim = this;
	}

	int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
	csplog().setLogCategory(CSP_ALL);
	csplog().setLogPriority(level);
        std::string logfile = g_Config.getString("Debug", "LogFile", "CSPSim.log", true);
        csplog().setOutput(logfile);

	CSP_LOG(APP, INFO, "Constructing CSPSim Object...");

	m_Clean = true;

	m_Paused = true;
	m_Finished = false;
	m_ConsoleOpen = false;

	//m_Console = NULL;

	m_CurrentScreen = NULL;
	m_PrevScreen = NULL;

	m_GameScreen = NULL;
	m_MainMenuScreen = NULL;

	m_FrameTime = 0.05;
	m_FrameRate = 20.0;
	m_ElapsedTime = 0.0;

	m_Shell = new PyShell();
}


CSPSim::~CSPSim()
{
	assert(m_Clean);
	if (theSim == this) {
		theSim = 0;
	}
}

void CSPSim::setActiveObject(simdata::Ref<DynamicObject> object) {

	/*
	CSP_LOG(APP, INFO, "CSPSim::setActiveObject - objectID: " << object->getObjectID()
	                   << ", ObjectType: " << object->getObjectType()
	                   << ", Position: " << object->getGlobalPosition());
	*/

	CSP_LOG(APP, INFO, "CSPSim::setActiveObject()");

	if (object == m_ActiveObject) return;
	if (m_ActiveObject.valid()) {
		// XXX XXX m_Battlefield->setHuman(m_ActiveObject->id(), false);
	}
	m_ActiveObject = object;
	if (m_GameScreen) {
		m_GameScreen->setActiveObject(m_ActiveObject);
	}
	if (m_ActiveObject.valid()) {
		// XXX XXX m_Battlefield->setHuman(m_ActiveObject->id(), true);
		simdata::hasht classhash = m_ActiveObject->getPath();
		CSP_LOG(APP, INFO, "getting map for " << classhash.str());
		simdata::Ref<EventMapping> map = m_InterfaceMaps->getMap(classhash);
		CSP_LOG(APP, INFO, "selecting map @ " << map.get());
		m_Interface->setMapping(map);
	}
	m_Interface->bindObject(m_ActiveObject.get());
}

simdata::Ref<PyShell> CSPSim::getShell() const {
	return m_Shell;
}

simdata::Ref<DynamicObject> CSPSim::getActiveObject() const {
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

void CSPSim::init()
{
	try {
		CSP_LOG(APP, INFO, "Installing stack trace handler...");
		simdata::Trace::install();

		CSP_LOG(APP, INFO, "Starting CSPSim...");

		osg::setNotifyLevel(osg::WARN);

		// setup osg search path for external data files
		std::string image_path = getDataPath("ImagePath");
		std::string model_path = getDataPath("ModelPath");
		std::string font_path = getDataPath("FontPath");
		std::string search_path;
		simdata::ospath::addpath(search_path, image_path);
		simdata::ospath::addpath(search_path, model_path);
		simdata::ospath::addpath(search_path, font_path);
		ObjectModel::setDataFilePathList(search_path);

		// open the primary data archive
		std::string cache_path = getCachePath();
		std::string archive_file = simdata::ospath::join(cache_path, "sim.dar");
		try {
			simdata::DataArchive *sim = new simdata::DataArchive(archive_file.c_str(), 1);
			assert(sim);
			m_DataManager->addArchive(sim);
		}
		catch (simdata::Exception &e) {
			CSP_LOG(APP, ERROR, "Error opening data archive " << archive_file);
			CSP_LOG(APP, ERROR, e.getType() << ": " << e.getMessage());
			throw e;
			//::exit(0);
		}

		// initialize SDL
		if (initSDL()) {
			::exit(1);
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

		CSP_LOG(APP, DEBUG, "INIT:: interface maps");

		// load all interface maps and create a virtual hid for the active object
		m_InterfaceMaps = new EventMapIndex();
		m_InterfaceMaps->loadAllMaps();
		m_Interface = new VirtualHID();

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSP_LOG(APP, DEBUG, "INIT:: theater");

		std::string theater = g_Config.getPath("Testing", "Theater", "sim:theater.balkan", false);
		m_Theater = m_DataManager->getObject(theater.c_str());
		assert(m_Theater.valid());
		m_Terrain = m_Theater->getTerrain();
		assert(m_Terrain.valid());
		m_Terrain->activate();

		// configure the atmosphere for the theater location
		// this affects mean temperatures, and should not need
		// to be updated for motion within a given theater.
		double lat = m_Terrain->getCenter().latitude();
		double lon = m_Terrain->getCenter().longitude();
		m_Atmosphere->setPosition(lat, lon);
		
		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSP_LOG(APP, DEBUG, "INIT:: scene");

		m_Scene = new VirtualScene();
		m_Scene->buildScene();
		m_Scene->setTerrain(m_Terrain);

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSP_LOG(APP, DEBUG, "INIT:: battlefield");

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSP_LOG(APP, DEBUG, "INIT:: scene configuration");

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

		int visual_radius = g_Config.getInt("Testing", "VisualRadius",  40000, true);
		m_Battlefield = new LocalBattlefield(m_DataManager);
		m_Battlefield->setSceneManager(new SimpleSceneManager(m_Scene, visual_radius));
		if (m_Theater.valid()) {
			FeatureGroup::Ref::list groups = m_Theater->getAllFeatureGroups();
			CSP_LOG(BATTLEFIELD, DEBUG, "Adding " << groups.size() << " features to the battlefield");
			for (FeatureGroup::Ref::list::iterator iter = groups.begin(); iter != groups.end(); ++iter) {
				m_Battlefield->addStatic(*iter);
			}
		}

		// create the networking layer
		if (g_Config.getBool("Networking", "UseNetworking", false, true)) {
			simnet::netlog().setLogPriority(simdata::LOG_INFO);
			std::string local_address = g_Config.getString("Networking", "LocalIp", "127.0.0.1", true);
			int local_port = g_Config.getInt("Networking", "LocalPort", 14111, true);

			CSP_LOG(NETWORK, INFO, "Initializing network interface " << local_address << ":" << local_port);
			int incoming_bw = g_Config.getInt("Networking", "IncomingBandwidth", 36000, true);
			int outgoing_bw = g_Config.getInt("Networking", "OutgoingBandwidth", 36000, true);
			simnet::NetworkNode local_node(local_address, local_port);
			m_NetworkClient = new simnet::Client(local_node, incoming_bw, outgoing_bw);

			if (g_Config.hasKey("Networking", "ExternalIp")) {
				std::string external_address = g_Config.getString("Networking", "ExternalIp");
				simnet::NetworkNode external_node(external_address, local_port);
				CSP_LOG(NETWORK, INFO, "External interface is " << external_address << ":" << local_port);
				m_NetworkClient->setExternalNode(external_node);
			}

			std::string server_address = g_Config.getString("Networking", "ServerIp", "127.0.0.1", true);
			int server_port = g_Config.getInt("Networking", "ServerPort", 14110, true);
			CSP_LOG(NETWORK, INFO, "Connecting to server: " << server_address << ":" << server_port);
			simnet::NetworkNode server_node(server_address, server_port);
			if (!m_NetworkClient->connectToServer(server_node, 5.0 /*seconds*/)) {
				std::cerr << "Unable to connecting to server, running in local mode\n";
				CSP_LOG(NETWORK, ERROR, "Unable to connecting to server, running in local mode");
				m_NetworkClient = 0;
			} else {
				std::string name = g_Config.getString("Networking", "UserName", "anonymous", true);
				CSP_LOG(NETWORK, INFO, "Connecting to server battlefield as " << name);
				m_Battlefield->setNetworkClient(m_NetworkClient);
				m_Battlefield->connectToServer(name);
				simdata::Timer timer;
				timer.start();
				while (timer.elapsed() < 5.0 && !m_Battlefield->isConnectionActive()) {
					m_NetworkClient->processAndWait(0.01, 0.01, 0.1);
				}
				if (!m_Battlefield->isConnectionActive()) {
					// connection failed, go back to local mode
					m_Battlefield->setNetworkClient(0);
				}
			}
			simnet::netlog().setLogPriority(simdata::LOG_INFO);
			//simnet::netlog().setLogPriority(6);
		}

		logoScreen.onUpdate(0.0);
		logoScreen.onRender();
		SDL_GL_SwapBuffers();

		CSP_LOG(APP, DEBUG, "INIT:: gamescreen");

		// Following variables should be set before calling GameScreen.init()
		// because they are used in GameScreen initialization process

		// enable/disable pause at startup
		m_Paused = false;
		//m_Paused = true;

		// create and initialize screens
		m_GameScreen = new GameScreen;
		m_GameScreen->onInit();
		
#if 0
		// set the Main Menu then start the main loop
		m_MainMenuScreen = new MenuScreen;
		m_MainMenuScreen->onInit();
		changeScreen(m_MainMenuScreen);
#endif

		changeScreen(m_GameScreen);
		
	}
	catch(csp::Exception & pEx) {
		csp::FatalException(pEx, "initialization");
	}
	catch(DemeterException & pEx) {
		csp::DemeterFatalException(pEx, "initialization");
	}
	catch(simdata::Exception & pEx) {
		csp::SimDataFatalException(pEx, "initialization");
	}
	/*
	catch (...) {
		csp::OtherFatalException("initialization");
	}
	*/

}


void CSPSim::cleanup()
{
	CSP_LOG(APP, INFO, "CSPSim  cleanup...");

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
	/*
	if (m_Console) {
		Py_DECREF(m_Console);
		m_Console = NULL;
	}
	*/
	SDL_Quit();
	m_Clean = true;
}


void CSPSim::quit()
{
	CSP_LOG(APP, DEBUG, "CSPSim::quit...");
	m_Finished = true;
}


void CSPSim::changeScreen(BaseScreen * newScreen)
{
	CSP_LOG(APP, DEBUG, "CSPSim::changeScreen ...");
	m_PrevScreen = m_CurrentScreen;
	m_CurrentScreen = newScreen;
}


// Main Game loop
void CSPSim::run()
{
	float low_priority = 0.0;
	int idx = 0;

	CSP_LOG(APP, DEBUG, "CSPSim::run...");

	//m_bShowStats = true;

	std::string date_string = g_Config.getString("Testing", "Date", "2000-01-01 00:00:00.0", true);
	simdata::SimDate date;
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

	try
	{
		while (!m_Finished) {
			CSP_LOG(APP, DEBUG, "CSPSim::run... Starting loop iteration");

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
			if (low_priority > 0.33) {
				switch (idx++) {
					case 0:
						m_Atmosphere->update(low_priority);
						break;
					default:
						idx = 0;
				}
				low_priority = 0.0;
			}

			// Update Objects if sim is not frozen
			PROF0(_objects);
			if (!m_Paused && !m_ConsoleOpen) {
				//CSP_LOG(APP, ERROR, "update objects");
				updateObjects(dt);
				//CSP_LOG(APP, ERROR, "update objects done");
			}
			PROF1(_objects, 60);
			
			// Display (render) current Screen
			if (m_CurrentScreen) {
				PROF0(_screen_update);
				m_CurrentScreen->onUpdate(dt);
				PROF1(_screen_update, 60);
				PROF0(_screen_render);
				m_CurrentScreen->onRender();
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
		}
		//m_Battlefield->dumpObjectHistory();
	}
	catch(DemeterException * pEx) {
		std::string msg = pEx->GetErrorMessage();
		CSP_LOG(APP, ERROR, "Caught Demeter Exception: " << msg );
		cleanup();
		::exit(1);
	}
	catch(...) {
		CSP_LOG(APP, ERROR, "MAIN: Unexpected exception, GLErrorNUM: " << glGetError());
		cleanup();
		::exit(1);
	}

}


void CSPSim::initTime(simdata::SimDate const &date)
{
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
void CSPSim::updateTime()
{
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

void CSPSim::doInput(double dt)
{
	CSP_LOG(APP, DEBUG, "CSPSim::doInput()...");

	simdata::Ref<VirtualHID> screen_interface = m_CurrentScreen->getInterface();

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
		if (m_ConsoleOpen && event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				endConsole();
				handled = true;
			} else {
				/*
				handled = m_Console->onKey(event.key.keysym);
				*/
			}
		}
		if (!handled && m_CurrentScreen) {
			if (screen_interface.valid()) {
				handled = screen_interface->onEvent(event);
			}
		}
		if (!handled && m_Interface.valid()) {
		  CSP_LOG(APP, DEBUG, "CSPSim::doInput()-Calling m_Interface->onEvent()");	
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
void CSPSim::updateObjects(double dt)
{
	CSP_LOG(APP, DEBUG, "CSPSim::updateObjects...");

	if (m_Battlefield.valid()) {
		m_Battlefield->update(dt);
	}
	if (m_Scene.valid()) {
		m_Scene->onUpdate(dt);
	}
}


int CSPSim::initSDL()
{
	CSP_LOG(APP, DEBUG, "Initializing SDL...");

	int height = g_Config.getInt("Screen", "Height", 768, true);
	int width = g_Config.getInt("Screen", "Width", 1024, true);
	int fullscreen = g_Config.getInt("Screen", "FullScreen", 0, true);

	m_ScreenHeight = height;
	m_ScreenWidth = width;
	
	g_ScreenHeight = height;
	g_ScreenWidth = width;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) != 0) {
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << "\n";
		CSP_LOG(APP, ERROR, "ERROR! Unable to initialize SDL: " << SDL_GetError());
		return 1;
	}

	/* Make sure SDL_Quit gets called when the program exits. */
	atexit(SDL_Quit);

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	int bpp = info->vfmt->BitsPerPixel;

	CSP_LOG(APP, ERROR, "Initializing video at " << bpp << " BitsPerPixel.");

	Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF;

	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}

	m_SDLScreen = SDL_SetVideoMode(width, height, bpp, flags);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	if (m_SDLScreen == NULL) {
		std::cerr << "Unable to set video mode: " << SDL_GetError() << "\n";
		CSP_LOG(APP, ERROR, "ERROR! Unable to initialize SDL: " << SDL_GetError());
		return 1;
	}

	SDL_JoystickEventState(SDL_ENABLE);
	m_SDLJoystick = SDL_JoystickOpen(0);
	if (m_SDLJoystick == NULL) {
		CSP_LOG(APP, ERROR, "Failed to open joystick");
		CSP_LOG(APP, ERROR, SDL_GetError());
	}

	SDL_EnableUNICODE(1);

	// some simple sdl sound testing
	std::string sound_path = getDataPath("SoundPath");
	std::string test_sound = simdata::ospath::join(sound_path, "avionturbine5.wav");
	if (SDL_LoadWAV(test_sound.c_str(), &m_audioWave.spec, &m_audioWave.sound,
	                &m_audioWave.soundlen) == NULL) {
		CSP_LOG(APP, WARNING,  "Couldn't load '" << test_sound << "': " << SDL_GetError());
	} else {
		m_audioWave.spec.callback = fillerup;

		/* Initialize fillerup() variables */
		if (SDL_OpenAudio(&m_audioWave.spec, NULL) < 0)  {
			CSP_LOG(APP, ERROR, "Couldn't open audio: " << SDL_GetError());
			SDL_FreeWAV(m_audioWave.sound);
		}
	}
	bool mute = g_Config.getBool("Testing", "Mute", false, true);
	SDL_PauseAudio(mute);

	return 0;
}


void fillerup(void * /*unused*/, Uint8 *stream, int len)
{
	Uint8 *waveptr;
	int    waveleft;

	/* Set up the pointers */
	waveptr = m_audioWave.sound + m_audioWave.soundpos;
	waveleft = m_audioWave.soundlen - m_audioWave.soundpos;

	/* Go! */
	while ( waveleft <= len ) {
		SDL_MixAudio(stream, waveptr, waveleft, SDL_MIX_MAXVOLUME);
		stream += waveleft;
		len -= waveleft;
		waveptr = m_audioWave.sound;
		waveleft = m_audioWave.soundlen;
		m_audioWave.soundpos = 0;
	}
	SDL_MixAudio(stream, waveptr, len, SDL_MIX_MAXVOLUME);
	m_audioWave.soundpos += len;
}


void CSPSim::runConsole(PyConsole * /*console*/) {
	// XXX the console code needs a major rewrite since upgrading to osg 0.9.4
	/*
	m_Console = console;
	if (m_Console.valid()) {
		m_ConsoleOpen = true;
		m_Console->bind(m_Shell);
		m_Console->enable();
	}
	*/
}

void CSPSim::endConsole() {
	/*
	m_ConsoleOpen = false;
	m_Console->disable();
	m_Console = NULL;
	*/
}

simdata::Ref<Theater> CSPSim::getTheater() const {
	return m_Theater;
}

TerrainObject * CSPSim::getTerrain() {
	return m_Terrain.get();
}

TerrainObject const * CSPSim::getTerrain() const {
	return m_Terrain.get();
}

simdata::Ref<EventMapIndex> CSPSim::getInterfaceMaps() const {
	return m_InterfaceMaps;
}

