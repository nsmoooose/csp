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
 * @file CSPSim.cpp
 *
 */
#include <Python.h>

#include "CSPSim.h"
#include "Config.h"      
#include "DynamicObject.h"
#include "EventMapIndex.h"
#include "Exception.h"
#include "GameScreen.h"
#include "HID.h"
#include "MenuScreen.h"
#include "LogoScreen.h"
#include "Log.h"
#include "Shell.h"
#include "SimObject.h"
#include "StaticObject.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "TerrainObject.h"
#include "Theater.h"
#include "ConsoleCommands.h"
   

#include <SimData/Types.h>
#include <SimData/Exception.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/Exception.h>
#include <SimData/FileUtility.h>

#include <GL/gl.h>		// Header File For The OpenGL32 Library
#include <GL/glu.h>		// Header File For The GLu32 Library

#include <osg/Timer>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include "DemeterException.h"


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

CSPSim::CSPSim()
{ 
	if (theSim == 0) {
		theSim = this;
	}

	m_Clean = true;

	m_Paused = true;
	m_Finished = false;
	m_ConsoleOpen = false;

	m_Console = NULL;

	m_CurrentScreen = NULL;
	m_PrevScreen = NULL;

	m_GameScreen = NULL;
	m_MainMenuScreen = NULL;

	m_Interface = NULL;
	m_Battlefield = NULL;
	m_Scene = NULL;

	m_FrameTime = 0.05;
	m_FrameRate = 20.0;
	m_ElapsedTime = 0.0;

	m_Shell = new PyShell();

	int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
	csplog().setLogLevels(CSP_ALL, level);
	csplog().setOutput("CSPSim.log");
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

	if (m_ActiveObject.valid()) {
		m_Battlefield->setHuman(m_ActiveObject, false);
		//m_ActiveObject->setHuman(false);
	}
	m_ActiveObject = object;
	if (m_GameScreen) {
		m_GameScreen->setActiveObject(m_ActiveObject);
	}
	if (m_ActiveObject.valid()) {
		//m_ActiveObject->setHuman(true);
		m_Battlefield->setHuman(m_ActiveObject, true);
		simdata::hasht classhash = m_ActiveObject->getPath();
		printf("getting map for %s\n", classhash.str().c_str());
		EventMapping *map = m_InterfaceMaps->getMap(classhash);
		printf("selecting map @ %p\n", map);
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

VirtualBattlefield * CSPSim::getBattlefield() {
	return m_Battlefield.get();
}

VirtualBattlefield const * CSPSim::getBattlefield() const {
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


void CSPSim::init()
{
	try {
		CSP_LOG(APP, INFO, "Starting CSPSim...");

		std::string data_path = g_Config.getPath("Paths", "DataPath", ".", true);
		std::string archive_file = simdata::ospath::join(data_path, "sim.dar");
		
		// open the primary data archive
		try {
			simdata::DataArchive *sim = new simdata::DataArchive(archive_file.c_str(), 1);
			assert(sim);
			m_DataManager.addArchive(sim);
		} 

#ifndef CSP_NDEBUG
		catch (simdata::Exception &e) {
			CSP_LOG(APP, ERROR, "Error opening data archive " << archive_file);
			CSP_LOG(APP, ERROR, e.getType() << ": " << e.getMessage());
			throw;
			//::exit(0);
		}
#else 
		catch (...) {
		}
#endif
		// initialize SDL
		initSDL();

		SDL_WM_SetCaption("CSPSim", "");

		// put up Logo screen then do rest of initialization
		LogoScreen logoScreen(m_ScreenWidth, m_ScreenHeight);
		logoScreen.onInit();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		logoScreen.onRender();

		SDL_GL_SwapBuffers();

		m_Clean = false;

		// load all interface maps and create a virtual hid for the active object
		m_InterfaceMaps = new EventMapIndex();
		m_InterfaceMaps->loadAllMaps();
		m_Interface = new VirtualHID();

		std::string theater = g_Config.getPath("Testing", "Theater", "sim:theater.balkan", false);
		m_Theater = m_DataManager.getObject(theater.c_str());
		assert(m_Theater.valid());
		m_Terrain = m_Theater->getTerrain();
		assert(m_Terrain.valid());
		m_Terrain->activate();
		
		// eventually this will be set in an entirely different way...
		//m_ActiveTerrain = m_DataManager.getObject("sim:terrain.balkan");
		//m_ActiveTerrain->activate();

		m_Scene = new VirtualScene();
		m_Scene->buildScene();
		m_Scene->setTerrain(m_Terrain);

		m_Battlefield = new VirtualBattlefield();
		m_Battlefield->create();
		m_Battlefield->setTerrain(m_Terrain);
		m_Battlefield->setScene(m_Scene);
		m_Battlefield->setTheater(m_Theater);

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

		// create a couple test objects

		//simdata::Ref<ObjectModel> test = m_DataManager.getObject("sim:theater.runway_model");
		//m_Scene->addEffectUpdater(test->getModel().get());

		std::string vehicle = g_Config.getPath("Testing", "Vehicle", "sim:vehicles.aircraft.m2k", false);
		simdata::Ref<AircraftObject> ao = m_DataManager.getObject(vehicle.c_str());
		assert(ao.valid());
		//ao->setGlobalPosition(483025, 499000, 88.5);
		ao->setGlobalPosition(483025, 499000, 2000.0);
		ao->setAttitude(0.0, 0.0, 1.92);
		ao->setVelocity(0, 1.0, 0);
		m_Battlefield->addUnit(ao);

#if 0
		static simdata::Ref<StaticObject> so = m_DataManager.getObject("sim:objects.runway");
		assert(so.valid());
		so->setGlobalPosition(483000, 499000, 100.0);
		so->addToScene(m_Battlefield);
#endif
 
		// Following variables should be set before calling GameScreen.init()
		// because they are used in GameScreen initialization process

		// enable/disable pause at startup
		m_Paused = false;
		//m_Paused = true;

		// start in the aircraft
		setActiveObject(ao);

		// create screens
		m_GameScreen = new GameScreen;
		// setup screens
		m_GameScreen->onInit();

#if 0
		// set the Main Menu then start the main loop
		m_MainMenuScreen = new MenuScreen;
		m_MainMenuScreen->onInit();
		changeScreen(m_MainMenuScreen);
#endif

		changeScreen(m_GameScreen);
		logoScreen.onExit();
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
	catch (...) {
		csp::OtherFatalException("initialization");
	}

}


void CSPSim::cleanup()
{
	CSP_LOG(APP, INFO, "CSPSim  cleanup...");

	assert(m_Battlefield.valid());
	assert(m_Scene.valid());
	assert(m_Terrain.valid());
	assert(m_GameScreen);
	assert(m_InterfaceMaps);
//	m_ActiveObject = NULL; 
	setActiveObject(NULL);
	delete m_GameScreen;
	m_GameScreen = NULL;
	delete m_InterfaceMaps;
	m_InterfaceMaps = NULL;
	m_Terrain->deactivate();
	m_Battlefield->removeAllUnits();
	m_Battlefield->cleanup();
	m_Terrain = NULL;
	m_Theater = NULL;
	m_Battlefield = NULL;
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
	
	// XXX move this elsewhere
	double lat = simdata::DegreesToRadians(g_Config.getFloat("Testing", "Latitude", 0, true));
	double lon = simdata::DegreesToRadians(g_Config.getFloat("Testing", "Longitude", 0, true));
	m_Atmosphere.setPosition(lat, lon);
	m_Atmosphere.setDate(date);
	m_Atmosphere.reset();

	try 
	{
		while (!m_Finished) {
			CSP_LOG(APP, DEBUG, "CSPSim::run... Starting loop iteration");

			updateTime();
			float dt = m_FrameTime;

			// Do Input loop
			doInput(dt);

			// Miscellaneous Updates
			low_priority += dt;
			if (low_priority > 0.33) {
				switch (idx++) {
					case 0:
						m_Atmosphere.update(low_priority);
						break;
					default:
						idx = 0;
				}
				low_priority = 0.0;
			}

			// Update Objects if sim is not frozen
			if (!m_Paused && !m_ConsoleOpen) {
				updateObjects(dt);
			}
			
			// Display (render) current Screen
			if (m_CurrentScreen) {
				m_CurrentScreen->onUpdate(dt);
				m_CurrentScreen->onRender();
			}
            
			// Swap OpenGL buffers
#ifndef __CSPSIM_EXE__
			Py_BEGIN_ALLOW_THREADS;
			SDL_GL_SwapBuffers();
			Py_END_ALLOW_THREADS;
#else
			SDL_GL_SwapBuffers();
#endif
			// remove marked objects, this should be done at the end of the main loop.
			m_Battlefield->removeUnitsMarkedForDelete();
		}
		//m_Battlefield->dumpObjectHistory();
	}
#ifndef CSP_NDEBUG
	catch(DemeterException * pEx) {
		CSP_LOG(APP, ERROR, "Caught Demeter Exception: " << pEx->GetErrorMessage());
		cleanup();
		::exit(1);
	}
#endif
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

	VirtualHID *screen_interface = m_CurrentScreen->getInterface();

	SDL_Event event;
	int doPoll = 10;
	while (doPoll-- && SDL_PollEvent(&event)) {
		bool handled = false;
		if (event.type == SDL_QUIT) {
			m_Finished = true;
			return;
		}
		if (m_ConsoleOpen && event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				endConsole();
				handled = true;
			} else {
				handled = m_Console->onKey(event.key.keysym);
			}
		}
		if (!handled && m_CurrentScreen) {
			if (screen_interface) {
				handled = screen_interface->onEvent(event);
			}
		}
		if (!handled && m_Interface) {
			handled = m_Interface->onEvent(event);
		}
	}
	// run input scripts
	if (screen_interface) screen_interface->onUpdate(dt);
	if (m_Interface) m_Interface->onUpdate(dt);
}

/**
 * Update all objects, calling physics and AI routines.
 */
void CSPSim::updateObjects(double dt)
{
	CSP_LOG(APP, DEBUG, "CSPSim::updateObjects...");

    	if (m_Battlefield.valid()) {
		m_Battlefield->onUpdate(dt);
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
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
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
		printf("Unable to set video mode: %s\n", SDL_GetError());
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


	std::string sound_path = g_Config.getPath("Paths", "SoundPath", ".", true);
	if ( SDL_LoadWAV(simdata::ospath::join(sound_path, "avionturbine5.wav").c_str(),
		&m_audioWave.spec, &m_audioWave.sound, &m_audioWave.soundlen) == NULL ) {
		CSP_LOG(APP, ERROR,  "Couldn't load " << sound_path << "/avionturbine5.wav: " << SDL_GetError());
		::exit(1);
	}
	m_audioWave.spec.callback = fillerup;

	/* Initialize fillerup() variables */
	if ( SDL_OpenAudio(&m_audioWave.spec, NULL) < 0 ) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		SDL_FreeWAV(m_audioWave.sound);
		::exit(2);
	}
	bool mute = g_Config.getBool("Testing", "Mute", false, true);
	SDL_PauseAudio(mute);

	return 0;
}


void fillerup(void *unused, Uint8 *stream, int len)
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


void CSPSim::runConsole(PyConsole *console) {
	// XXX the console code needs a major rewrite since upgrading to osg 0.9.4
	return;  // XXX temporarily disabled.
	m_Console = console;
	if (m_Console.valid()) {
		m_ConsoleOpen = true;
		m_Console->bind(m_Shell);
		m_Console->enable();
	}
}

void CSPSim::endConsole() {
	m_ConsoleOpen = false;
	m_Console->disable();
	m_Console = NULL;
}

