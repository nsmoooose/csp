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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>		// Header File For The OpenGL32 Library
#include <GL/glu.h>		// Header File For The GLu32 Library

#include <osg/Timer>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include "DemeterException.h"

#include "Config.h"      
#include "CSPSim.h"
#include "EventMapIndex.h"
#include "GameScreen.h"
#include "HID.h"
#include "MenuScreen.h"
#include "LogoScreen.h"
#include "LogStream.h"
#include "Platform.h"
#include "SimObject.h"
#include "VirtualBattlefield.h"
#include "ConsoleCommands.h"

#include <SimData/Types.h>
#include <SimData/Exception.h>
#include <SimData/DataArchive.h>

SDLWave  m_audioWave;


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


// very ugly.... please fix ;-)
double g_LatticeXDist = 64000.0;
double g_LatticeYDist = 64000.0;
int g_ScreenWidth = 0;
int g_ScreenHeight = 0;



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

	int level = g_Config.getInt("Debug", "LoggingLevel", 0, true);
	csplog().setLogLevels(CSP_ALL, (cspDebugPriority)level);
	csplog().set_output("CSPSim.log");
}


CSPSim::~CSPSim()
{
	if (theSim == this) {
		theSim = 0;
	}
	if (m_Battlefield) {
		delete m_Battlefield;
	}
	if (m_GameScreen) {
		delete m_GameScreen;
	}
	/*
	if (m_Console) {
		Py_DECREF(m_Console);
		m_Console = NULL;
	}
	*/
	csplog().set_output(cerr);
}

void CSPSim::setActiveObject(simdata::Pointer<DynamicObject> object) {

	CSP_LOG(CSP_APP, CSP_INFO, "CSPSim::setActiveObject - objectID: " << object->getObjectID() 
		  << ", ObjectType: " << object->getObjectType() 
		  << ", Position: " << object->getGlobalPosition());

	if (m_ActiveObject.valid()) {
		m_ActiveObject->setHuman(false);
	}
	m_ActiveObject = object;
	if (m_ActiveObject.valid()) {
		m_ActiveObject->setHuman();
		m_GameScreen->setActiveObject(m_ActiveObject);
		simdata::hasht classhash = m_ActiveObject->getPath();
		printf("getting map for %s\n", classhash.str().c_str());
		EventMapping *map = m_InterfaceMaps->getMap(classhash);
		printf("selecting map @ %p\n", map);
		m_Interface->setMapping(map);
		m_Interface->bindObject(m_ActiveObject.get());
		printf("map set, object bound.\n");
	}
	else {
		CSP_LOG(CSP_APP, CSP_ERROR, "CSPSim::setActiveObject - object was not valid");

	}
}

simdata::Pointer<DynamicObject const> const CSPSim::getActiveObject() const {
	return m_ActiveObject;
}

VirtualBattlefield * const CSPSim::getBattlefield() const
{
	return m_Battlefield;
}

void CSPSim::togglePause() {
	m_Paused = !m_Paused;
} 

void CSPSim::init()
{
	CSP_LOG(CSP_APP, CSP_INFO, "Starting CSPSim...");

	std::string data_path = g_Config.getPath("Paths", "DataPath", ".", true);
	std::string archive_file = ospath::join(data_path, "Sim.dar");
	
	// open the primary data archive
	try {
		m_DataArchive = new simdata::DataArchive(archive_file.c_str(), 1);
		assert(m_DataArchive);
	} 
	catch (simdata::Exception e) {
		CSP_LOG(CSP_APP, CSP_ERROR, "Error opening data archive " << archive_file);
		CSP_LOG(CSP_APP, CSP_ERROR, e.getType() << ": " << e.getMessage());
		::exit(0);
	}
	
	// initialize SDL
	initSDL();

	SDL_WM_SetCaption("CSPSim", "");

	// put up Logo screen then do rest of initialization
	LogoScreen logoScreen(m_ScreenWidth, m_ScreenHeight);
	logoScreen.OnInit();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	logoScreen.onRender();

	SDL_GL_SwapBuffers();

	// load all interface maps and create a virtual hid for the active object
	m_InterfaceMaps = new EventMapIndex();
	m_InterfaceMaps->loadAllMaps();
	m_Interface = new VirtualHID();

	// FIXME: these functions should be separated!
	// create the battlefield + scenegraph
	m_Battlefield = new VirtualBattlefield();
	m_Battlefield->create();
	m_Battlefield->buildScene();

	// eventually this will be set in an entirely different way...
	m_ActiveTerrain = m_DataArchive->getObject("terrain.balkan");
	m_ActiveTerrain->activate(m_Battlefield);

	// get view parameters from configuration file.  ultimately there should
	// be an in-game ui for this and probably a separate config file.
	bool wireframe = g_Config.getBool("View", "Wireframe", false, true);
	m_Battlefield->setWireframeMode(wireframe);
	int view_distance = g_Config.getInt("View", "ViewDistance", 35000, true);
	m_Battlefield->setViewDistance(view_distance);
	bool fog = g_Config.getBool("View", "Fog", true, true);
	m_Battlefield->setFogMode(fog);
	int fog_start = g_Config.getInt("View", "FogStart", 20000, true);
	m_Battlefield->setFogStart(fog_start);
	int fog_end = g_Config.getInt("View", "FogEnd", 35000, true);
	m_Battlefield->setFogEnd(fog_end);

	// create a couple test objects
	simdata::Pointer<AircraftObject> ao = m_DataArchive->getObject("vehicles.aircraft.m2k");
	assert(ao.valid());
	//ao->setGlobalPosition(483000, 499000, 2000);
	ao->setGlobalPosition(483000, 499000, 91.2);
	ao->setOrientation(0, 5.0, 0);
	ao->setVelocity(0, 120.0, 0);
	ao->setVelocity(0, 2.0, 0);
	ao->addToScene(m_Battlefield);
	m_Battlefield->addObject(ao);
	
	/*
	simdata::Pointer<DynamicObject> to = m_DataArchive->getObject("vehicles.aircraft.m2k");
	assert(to.valid());
	to->setGlobalPosition(483000, 501000, 0);
	to->addToScene(m_Battlefield);
	m_Battlefield->addObject(to);
	*/	

	// create screens
	m_GameScreen = new GameScreen;

	// setup screens
	m_GameScreen->SetBattlefield(m_Battlefield);
	m_GameScreen->OnInit();

	// start in the aircraft
	setActiveObject(ao);

#if 0
	// set the Main Menu then start the main loop
	m_MainMenuScreen = new MenuScreen;
	m_MainMenuScreen->OnInit();
	changeScreen(m_MainMenuScreen);
#endif


	changeScreen(m_GameScreen);
	
	logoScreen.OnExit();
}


void CSPSim::cleanup()
{
	CSP_LOG(CSP_APP, CSP_INFO, "CSPSim  cleanup...");

	assert(m_Battlefield);
	assert(m_ActiveTerrain.isNull() == false);
	assert(m_GameScreen);
	assert(m_InterfaceMaps);
	m_GameScreen->SetBattlefield(0);
	m_ActiveTerrain->deactivate();
	m_Battlefield->removeAllObjects();
	m_Battlefield->Cleanup();
	m_ActiveTerrain = simdata::PointerBase();
	delete m_Battlefield;
	m_Battlefield = NULL;
	delete m_GameScreen;
	m_GameScreen = NULL;
	delete m_InterfaceMaps;
	m_InterfaceMaps = NULL;
	if (m_SDLJoystick) {
		SDL_JoystickClose(m_SDLJoystick);
		m_SDLJoystick = NULL;
	}
	SDL_Quit();
}


void CSPSim::quit()
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::quit...");
	m_Finished = true;
}


void CSPSim::changeScreen(BaseScreen * newScreen)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::changeScreen ...");
	m_PrevScreen = m_CurrentScreen;
	m_CurrentScreen = newScreen;
}


// Main Game loop
void CSPSim::run()
{
	float low_priority = 0.0;
	int idx = 0;

	CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::run...");

	//m_bShowStats = true;
	//m_Paused = false;
	m_Paused = true;

	std::string date_string = g_Config.getString("Testing", "Date", "2000-01-01 00:00:00.0", true);
	simdata::SimDate date;
	try {
		date.parseXML(date_string.c_str());
	} 
	catch (...) {
		std::cerr << "Invalid starting date in INI file (Testing:Date).\n" << std::endl; 
		::exit(1);
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
			CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::run... Starting loop iteration");

			updateTime();
			float dt = m_FrameTime;

			// Do Input loop
			doInput();

			// Miscellaneous Updates
			low_priority += dt;
			if (low_priority > 1.0) {
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
			Py_BEGIN_ALLOW_THREADS;
			SDL_GL_SwapBuffers();
			Py_END_ALLOW_THREADS;

			// remove marked objects, this should be done at the end of the main loop.
			m_Battlefield->removeObjectsMarkedForDelete();
		}
		m_Battlefield->dumpObjectHistory();
	}
   
	catch(DemeterException * pEx) {
		CSP_LOG(CSP_APP, CSP_ERROR, "Caught Demeter Exception: " << pEx->GetErrorMessage());
		cleanup();
		::exit(0);
	}
	catch(...) {
		CSP_LOG(CSP_APP, CSP_ERROR, "MAIN: Unexpected exception, GLErrorNUM: " << glGetError());
		cleanup();
		::exit(0);
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
	if (m_FrameTime > 1.0) {
		m_TimeLag = m_FrameTime - 1.0;
		m_FrameTime = 1.0;
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


void CSPSim::doInput()
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::doInput()...");

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
			VirtualHID *i = m_CurrentScreen->getInterface();
			if (i) {
				handled = i->OnEvent(event);
			}
		}
		if (!handled && m_Interface) {
			handled = m_Interface->OnEvent(event);
		}
	}
}

/**
 * Update all objects, calling physics and AI routines.
 */
void CSPSim::updateObjects(double dt)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::updateObjects...");

    	if (m_Battlefield) {
		m_Battlefield->onUpdate(dt);
	}
}


int CSPSim::initSDL()
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "Initializing SDL...");

	int height = g_Config.getInt("Screen", "Height", 768, true);
	int width = g_Config.getInt("Screen", "Width", 1024, true);
	int fullscreen = g_Config.getInt("Screen", "FullScreen", 0, true);

	m_ScreenHeight = height;
	m_ScreenWidth = width;
	
	g_ScreenHeight = height;
	g_ScreenWidth = width;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		CSP_LOG(CSP_APP, CSP_ERROR, "ERROR! Unable to initialize SDL: " << SDL_GetError());
		return 1;
	}

	/* Make sure SDL_Quit gets called when the program exits. */
	atexit(SDL_Quit);

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	int bpp = info->vfmt->BitsPerPixel;

	CSP_LOG(CSP_APP, CSP_ERROR, "Initializing video at " << bpp << " BitsPerPixel.");

	Uint32 flags = SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF; 

	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	}

	m_SDLScreen = SDL_SetVideoMode(width, height, bpp, flags);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	if (m_SDLScreen == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		CSP_LOG(CSP_APP, CSP_ERROR, "ERROR! Unable to initialize SDL: " << SDL_GetError());
		return 1;
	}

	m_SDLJoystick = SDL_JoystickOpen(0);
	if (m_SDLJoystick == NULL) {
		CSP_LOG(CSP_APP, CSP_ERROR, "Failed to open joystick");
		CSP_LOG(CSP_APP, CSP_ERROR, SDL_GetError());
	}
    
	SDL_EnableUNICODE(1);

	std::string sound_path = g_Config.getPath("Paths", "SoundPath", ".", true);
	if ( SDL_LoadWAV(ospath::join(sound_path, "avionturbine5.wav").c_str(),
		&m_audioWave.spec, &m_audioWave.sound, &m_audioWave.soundlen) == NULL ) {
		CSP_LOG(CSP_APP, CSP_ERROR,  "Couldn't load " << sound_path << "/avionturbine5.wav: " << SDL_GetError());
		::exit(1);
	}
	m_audioWave.spec.callback = fillerup;

	/* Initialize fillerup() variables */
	if ( SDL_OpenAudio(&m_audioWave.spec, NULL) < 0 ) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		SDL_FreeWAV(m_audioWave.sound);
		::exit(2);
	}
	SDL_PauseAudio(0);

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

/*
int CSPSim::InitConsole()
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "CSPSim::InitConsole()...");

	SDL_Rect Con_rect;
	Con_rect.x = Con_rect.y = 0;
	Con_rect.w = Con_rect.h = 300;

	std::string font_path = g_Config.getPath("Paths", "FontPath", ".", true);
	std::string console_font = ospath::join(font_path, "ConsoleFont.bmp");

	m_ConsoleFont = DT_LoadFont(console_font.c_str(), 0);
	m_pConsole = CON_Init(console_font.c_str(), m_SDLScreen, 100, Con_rect);
	CON_Alpha(m_pConsole, 50);

	// Add some commands to the console
	CON_AddCommand(&KillProgram, "quit");
	CON_AddCommand(&AlphaChange, "alpha");
	CON_AddCommand(&Move, "move");
	CON_AddCommand(&Resize, "resize");
	CON_AddCommand(&ListCommands, "listcommands");

#ifdef _WIN32
	// I don't have this function in SDL_Console 1.3 on Linux
	CON_AddDefaultCommand(&DefaultCommand);
#endif

	CON_ListCommands(m_pConsole);

	return 0;
}


void CSPSim::ShowPaused()
{
	DT_DrawText("PAUSED", m_SDLScreen, m_ConsoleFont, m_SDLScreen->w - 36, 0);
}


void CSPSim::ShowPlaneInfo()
{
	if (m_ActiveObject.isNull()) return;

	int width = m_SDLScreen->w;
	simdata::Vector3 direction = m_ActiveObject->getDirection();
	simdata::Vector3 upVector = m_ActiveObject->getUpDirection();

	char buffer[256];
	snprintf(buffer, 255, "Direction: [%.3f, %.3f, %.3f]", direction.x, direction.y, direction.z);
	DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, width - 200, m_SDLScreen->h - 80);

	snprintf(buffer, 255, "Up: [%.3f, %.3f, %.3f]", upVector.x, upVector.y, upVector.z);
	DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, width - 200, m_SDLScreen->h - 60);

	int latX, latY;
	m_ActiveObject->getLatticePosition(latX, latY);
	snprintf(buffer, 255, "Lattice: [ %d, %d ]", latX, latY);
	DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, width - 200, m_SDLScreen->h - 40);
}


void CSPSim::ShowStats(float fps)
{
	char framerate[64];
	char buffer[256];
	static float maxFps = 0.0, minFps = 500.0;

	maxFps = max(maxFps, fps);
	minFps = min(minFps, fps);

	sprintf(framerate, "%.2f FPS min: %.2f max: %.2f", fps, minFps, maxFps);
	DT_DrawText(framerate, m_SDLScreen, m_ConsoleFont, 1, 0);

	sprintf(framerate, "%.2f FPS min: %.2f max: %.2f", fps, minFps, maxFps);
	simdata::SimDate artificial_time = m_CurrentTime;
	artificial_time.addTime(m_Battlefield->getSpin());
	std::string date = "Date: " + artificial_time.asString();
	DT_DrawText(date.c_str(), m_SDLScreen, m_ConsoleFont, 1, 20);

	sprintf(buffer, "Terrain Polygons: %d", m_Battlefield->getTerrainPolygonsRendered());
	DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, 1, m_SDLScreen->h - 160);


	if (!m_ActiveObject.isNull()) {
		simdata::Vector3 pos = m_ActiveObject->getLocalPosition();
		snprintf(buffer, 255, "LocalPosition: [%.3f, %.3f, %.3f]", pos.x, pos.y, pos.z);
		DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, 1, m_SDLScreen->h - 80);

		pos = m_ActiveObject->getGlobalPosition();
		snprintf(buffer, 255, "GlobalPosition: [%.3f, %.3f, %.3f]", pos.x, pos.y, pos.z);
		DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, 1, m_SDLScreen->h - 60);

		simdata::Vector3 vel = m_ActiveObject->getVelocity();
		snprintf(buffer, 255, "Velocity: [%.3f, %.3f, %.3f]", vel.x, vel.y, vel.z);
		DT_DrawText(buffer, m_SDLScreen, m_ConsoleFont, 1, m_SDLScreen->h - 40);

		std::vector<std::string> stats;
		std::vector<std::string>::iterator stat;
		m_ActiveObject->getStats(stats);
		int y = 40;
		for (stat = stats.begin(); stat != stats.end(); stat++, y+=20) {
			DT_DrawText(stat->c_str(), m_SDLScreen, m_ConsoleFont, 1, y);
			if (y > m_SDLScreen->h - 180) break;
		}
		// frame rate logging versus position
		// this is a temporary hack to look for problematic terrain tiles

		static float next_dump = 0.0;
		if (m_ElapsedTime > next_dump) {
			next_dump = m_ElapsedTime + 10.0;
			CSP_LOG(CSP_APP, CSP_WARN, m_FrameRate << endl << buffer);
		}

	}
}


*/
