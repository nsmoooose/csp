#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#include <osg/Timer>

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>


#include "SDL.h"
#include "CON_console.h" // console is using SDL_OPENGLBLIT :-( switch to osgConsole?
#include "CON_consolecommands.h"
#include "DT_drawtext.h"

#include "DemeterException.h"

#include "BaseInput.h"
#include "Config.h"
#include "ConsoleCommands.h"
#include "CSPFlightSim.h"
#include "GameScreen.h"
#include "GlobalCommands.h"
#include "MenuScreen.h"
#include "LogoScreen.h"
#include "LogStream.h"
#include "SimTime.h"
#include "ObjectFactory.h"
#include "VirtualBattlefield.h"


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern VirtualBattlefield * g_pBattlefield;
extern SimTime * g_pSimTime;
extern ObjectFactory * g_pObjectFactory;

BaseObject * g_pPlayerObject;
BaseInput * g_pPlayerInput;

int g_ScreenWidth=800;
int g_ScreenHeight=600;

int ConsoleFont;

double g_LatticeXDist = 64000.0;
double g_LatticeYDist = 64000.0;

// urgent to clarify the following or to use a real viewer!
// it seems that this class looks more and more like an osg(SDL) viewer!
osg::Timer _timer;
osg::Timer_t _initialTick, _lastFrameTick, _frameTick;
double _timeStep, _frameTime, _timeLag = 0.0;

osg::Timer_t clockTick()
{
    return _timer.tick();
}

double clockSeconds() 
{ 
  return _timer.delta_s(_initialTick,clockTick()); 
}

// update time from the current frame update and the previous one.
// the timestep for updates is restricted to 1 second max.  greater
// delays will accumulate in _timeLag to be made up in subsequent 
// frames.  this prevents long delays from destabilizing the update
// computations.
osg::Timer_t updateFrameTick()
{
    _lastFrameTick = _frameTick;
    _frameTick = _timer.tick();
    _frameTime = _timer.delta_s(_lastFrameTick,_frameTick);
    _timeStep = _frameTime + _timeLag;
    if (_timeStep > 1.0) {
    	_timeLag = _timeStep - 1.0;
	_timeStep = 1.0;
    } 
    if (_timeStep < 0.001) {
    	// todo: microsleep
    	_timeStep = 0.001;
    	_timeLag = 0.0;
    } else {
    	_timeLag = 0.0;
    }
    return _frameTick;
}

double frameSeconds() 
{ 
	return _frameTime; //_timer.delta_s(_lastFrameTick,_frameTick); 
}

double frameRate() 
{ 
	if (_frameTime < 0.005) return 200.0;
	return 1.0/_frameTime; //frameSeconds(); 
}

CSPFlightSim::CSPFlightSim()
{
    m_bFinished = FALSE;
    m_CurrentScreen = NULL;
    m_PrevScreen = NULL;

    m_logoScreen = NULL;
    m_gameScreen = NULL;
    m_MainMenuScreen = NULL;

    m_bConsoleDisplay = false;
    m_pConsole = NULL;
    m_bFreezeSim = true;
    m_bShowStats = true;

    g_pPlayerObject = NULL;
	g_pPlayerInput = NULL;

	_initialTick = _timer.tick();
	_frameTick = _initialTick;
	_lastFrameTick = _initialTick;
}


CSPFlightSim::~CSPFlightSim()
{
  delete m_logoScreen;
  delete m_gameScreen;
}

void CSPFlightSim::Init()
{

    CSP_LOG( CSP_APP , CSP_INFO,  "Starting CSPFlightSim..." ); 

    // Initialize SDL
    InitSDL();

    SDL_WM_SetCaption("CSPFlightSim", "");

    // Put up Logo screen then do rest of initialization.
    m_logoScreen = new LogoScreen;
    m_logoScreen->OnInit();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_logoScreen->OnRender();

    SDL_GL_SwapBuffers();

    // Do rest of initialization.
    InitConsole();

#ifdef _WIN32
	std::string sep = ";";
#else
	std::string sep = ":";
#endif

	osgDB::setDataFilePathList("../Data" + sep + "../Data/Models" + sep + "../Data/Images" + sep + "../Data/Fonts");

	// we don't need this on Linux since libs are usually
	// installed in /usr/local/lib/osgPlugins or /usr/lib/osgPlugins.
	// OSG can find itself the plugins.
#ifdef _WIN32
	osgDB::setLibraryFilePathList("../Bin");
#endif

    g_pObjectFactory->initialize();

    g_pBattlefield->create();
    g_pBattlefield->buildScene();

    DoStartupScript();
	
    // Create screens
    m_gameScreen = new GameScreen;
    m_gameScreen->OnInit();
//    m_MainMenuScreen = new MenuScreen;
//    m_MainMenuScreen->OnInit();

    // Set the Main Menu then start the main loop.
//    ChangeScreen(m_MainMenuScreen);
    ChangeScreen(m_gameScreen);
	
	m_logoScreen->OnExit();

}

void CSPFlightSim::Cleanup()
{
    g_pBattlefield->removeAllObjects();
	g_pBattlefield->Cleanup();
    g_pObjectFactory->Cleanup();
}


void CSPFlightSim::Exit()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::Exit..." );

    SDL_Quit();

    // other cleanup

//    exit(0);

}

void CSPFlightSim::ChangeScreen(BaseScreen * newScreen)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::ChangeScreen ..." );
    m_PrevScreen = m_CurrentScreen;
    m_CurrentScreen = newScreen;
}

// Main Game loop
void CSPFlightSim::Run()
{	
    bool first = true;
    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::Run..." );
    m_bFreezeSim = false;
    
    // reset to frame time so we don't jump in one frame through
    // all the time it took to initialize.  just a hack for now.
	_initialTick = _timer.tick();
	_frameTick = _initialTick;
	_lastFrameTick = _initialTick;
	
    try
    {
		while(!m_bFinished)
		{
			updateFrameTick();
			float dt = _timeStep; //frameSeconds();

			g_pSimTime->updateSimTime(dt);    
			float simtime = g_pSimTime->getSimTime();
			
			// Do Input loop
			DoInput();
			
			// Update Objects if sim is not frozen
			if (!m_bFreezeSim)
				UpdateObjects(dt);
			
			// Display (render) current Screen
			if (m_CurrentScreen)
				m_CurrentScreen->OnRender();
			
			// Do Console
			if(m_bConsoleDisplay) 
				CON_DrawConsole(m_pConsole);
			
			if ( m_bFreezeSim )
				ShowPaused();
			
			// Do fps and other stuff.
			if (m_bShowStats && !first)
			{
				ShowStats(frameRate());
				ShowPlaneInfo();
			}

			// Swap OpenGL buffers
			SDL_GL_SwapBuffers();
			
			// remove marked objects, this should be done at the end of the main loop.
			g_pBattlefield->removeObjectsMarkedForDelete();

			first = false;
		}
		
		g_pBattlefield->dumpObjectHistory();
	}
	catch(DemeterException* pEx)
	{
		CSP_LOG( CSP_APP, CSP_ERROR, "Caught Demeter Exception: " << pEx->GetErrorMessage() );
		Cleanup();
		SDL_Quit();
		exit(0);
	}
	catch(...)
	{
		CSP_LOG( CSP_APP, CSP_ERROR, "MAIN: Unexpected exception, GLErrorNUM: " << glGetError() );
		Cleanup();
		SDL_Quit();
		exit(0);
	}
	
}

void CSPFlightSim::DoInput()
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::DoInput()..." );

    
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        m_bFinished = TRUE;
                        break;
                    }

                case SDL_MOUSEMOTION:
                    {
                        if (m_CurrentScreen)
                            m_CurrentScreen->OnMouseMove(event.motion.x, event.motion.y);
                        break;
                    }

                case SDL_MOUSEBUTTONDOWN:
                    {
                        if (m_CurrentScreen)
                            m_CurrentScreen->OnMouseButtonDown(event.button.button,
                                                            event.button.x,
                                                            event.button.y);
                        break;
                    }
                case SDL_KEYDOWN:
						{
                        bool handled = false;
                        char * key = SDL_GetKeyName(event.key.keysym.sym);

						if (g_pPlayerInput)
						{
							handled = g_pPlayerInput->OnKeyDown(event.key.keysym.sym);
						}
                        
                        // see if the current screen handles this key
                        if (!handled && m_CurrentScreen && !m_bConsoleDisplay)
						{
							handled = m_CurrentScreen->OnKeyDown(event.key.keysym.sym);
						}
                        // other let the system handle the key.
                        if (!handled)
                        {
                            switch(event.key.keysym.sym)
                            {
                                case SDLK_ESCAPE:
                                {
                                    m_bFinished = TRUE;
                                    break;
                                }
                                case SDLK_F9:
                                {
                                    m_bConsoleDisplay = !m_bConsoleDisplay;
                                    if (m_bConsoleDisplay)
                                    {
                                        m_bFreezeSim = true;
                                        CON_Topmost(m_pConsole);
                                        SDL_EnableUNICODE(1);
                                    }
                                    else
                                    {
                                        m_bFreezeSim = false;
                                        CON_Topmost(NULL);
                                        SDL_EnableUNICODE(0);
                                    }
                                    break;
                                }
                                case SDLK_F10:
                                {
                                    m_bShowStats = !m_bShowStats;
                                    break;
                                }
								case SDLK_p:
									{
										// don't do pause while in console mode.
										if (!m_bConsoleDisplay)
										{
											if (!m_bFreezeSim)
												m_bFreezeSim = true;
											else 
												m_bFreezeSim = false;
										}
									}
					            default:
                                {
    					            // Send the event to the console
						            if(m_bConsoleDisplay)
							            CON_Events(&event);
	    					        break;
                                }
                            }
						}
                        break;
                   }

                case SDL_KEYUP:
                    {
						bool handled = false;
						if (g_pPlayerInput)
						{
							handled = g_pPlayerInput->OnKeyUp(event.key.keysym.sym);
						}
                      
                        if (!handled && m_CurrentScreen)
						{
                            handled = m_CurrentScreen->OnKeyUp(event.key.keysym.sym);
						}
                        break;
                    }
                
                case SDL_JOYAXISMOTION:
                    {

						if (g_pPlayerInput)
						{
							g_pPlayerInput->OnJoystickAxisMotion(event.jaxis.which, event.jaxis.axis,
                                                                  event.jaxis.value);
							break;
						}

                        if (m_CurrentScreen)
                            m_CurrentScreen->OnJoystickAxisMotion(event.jaxis.which, event.jaxis.axis,
                                                                  event.jaxis.value);
                         break;
     
                    }
				case SDL_JOYHATMOTION:
					if ( m_CurrentScreen )
						m_CurrentScreen->OnJoystickHatMotion(event.jhat.which, event.jhat.hat,
						                                     event.jhat.value);
                    break;
                case SDL_JOYBUTTONDOWN:
                    {
                        if (m_CurrentScreen)
                            m_CurrentScreen->OnJoystickButtonDown(event.jbutton.which, event.jbutton.button);
                        break;
                    }
     

            }
        }


}

// Update all objects including the players.
// This will call the objects physics and
// AI routines.
void CSPFlightSim::UpdateObjects(double dt)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::UpdateObjects..." );

    if (m_CurrentScreen)
             m_CurrentScreen->OnUpdateObjects(dt);
 
}


int CSPFlightSim::InitSDL()
{
    CSP_LOG( CSP_APP , CSP_DEBUG,  "Initializing SDL..." ); 


	int height = Config.GetInteger("ScreenHeight");
    int width = Config.GetInteger("ScreenWidth");
    int fullscreen = Config.GetInteger("FullScreen");

    if (height)
        g_ScreenHeight=height;

    if (width)
        g_ScreenWidth=width;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        CSP_LOG( CSP_APP, CSP_ERROR , "ERROR! Unable to initialize SDL: " << SDL_GetError() );
        return 1;
    }

    /* Make sure SDL_Quit gets called when the program exits. */
    atexit(SDL_Quit);

    const SDL_VideoInfo* info = SDL_GetVideoInfo( );
    int bpp = info->vfmt->BitsPerPixel;

    CSP_LOG(CSP_APP, CSP_ERROR, "Initializing video at " << bpp << " BitsPerPixel." );

    Uint32 flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_OPENGLBLIT ;
	//Uint32 flags = SDL_OPENGL | SDL_HWSURFACE;

    if (fullscreen)
        flags |= SDL_FULLSCREEN;


    m_SDLScreen = SDL_SetVideoMode(width, height, bpp, flags );
    if (m_SDLScreen == NULL)
    {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        CSP_LOG( CSP_APP, CSP_ERROR , "ERROR! Unable to initialize SDL: " << SDL_GetError() );
        return 1;
    }

    struct _SDL_Joystick * joystick = SDL_JoystickOpen(0);
    if (joystick == NULL)
    {
        CSP_LOG(CSP_APP, CSP_ERROR, "Failed to open joystick");
    	CSP_LOG(CSP_APP, CSP_ERROR, SDL_GetError());
    }

	SDL_EnableUNICODE(1);

    return 0;
}

int CSPFlightSim::InitConsole()
{
    CSP_LOG( CSP_APP , CSP_DEBUG,  "CSPFlightSim::InitConsole()..." ); 

    
    SDL_Rect Con_rect;
	Con_rect.x = Con_rect.y = 0;
	Con_rect.w = Con_rect.h = 300;
    
	ConsoleFont = DT_LoadFont("../Data/Fonts/ConsoleFont.bmp", 0);

    m_pConsole = CON_Init("../Data/Fonts/ConsoleFont.bmp", m_SDLScreen, 100, Con_rect);
    CON_Alpha( m_pConsole, 50 );

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


void CSPFlightSim::DoStartupScript()
{
    char startScriptFile[256];
    Config.GetString(startScriptFile, "StartupScript");
    string startCommand;
    startCommand = string("RUN ") + startScriptFile;
    printf("%s\n", startCommand.c_str());
    ProcessCommandString(startCommand);
    printf("done\n");
}


void CSPFlightSim::ShowPaused()
{
	DT_DrawText("PAUSED", m_SDLScreen, ConsoleFont, m_SDLScreen->w - 36, 0);
}

void CSPFlightSim::ShowPlaneInfo()
{
	int width = m_SDLScreen->w;
	char buffer[256];
	StandardVector3 direction = g_pPlayerObject->getDirection();
	StandardVector3 upVector = g_pPlayerObject->getUpDirection();

	sprintf(buffer, "Direction: [%.3f, %.3f, %.3f]", direction.x,
               		direction.y, direction.z);
	DT_DrawText(buffer, m_SDLScreen, ConsoleFont, width-200, m_SDLScreen->h - 80);


	sprintf(buffer, "Up: [%.3f, %.3f, %.3f]", upVector.x,
               		upVector.y, upVector.z);
	DT_DrawText(buffer, m_SDLScreen, ConsoleFont, width-200, m_SDLScreen->h - 60);

	int latX, latY;
	g_pPlayerObject->getLatticePosition(latX, latY);
	sprintf(buffer, "Lattice: [ %d, %d ]", latX, latY);
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, width-200, m_SDLScreen->h - 40);

}

void CSPFlightSim::ShowStats(float fps)
{
    char framerate[64];
    char buffer[256];
    static float maxFps = 0.0, minFps = 500.0;

	maxFps = max(maxFps, fps);
    minFps = min(minFps, fps);
  
    sprintf(framerate, "%.2f FPS min: %.2f max: %.2f", fps, minFps, maxFps);
	DT_DrawText(framerate, m_SDLScreen, ConsoleFont, 1, 0);

	sprintf(buffer, "Terrain Polygons: %d", 
		g_pBattlefield->getTerrainPolygonsRendered() );
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 160);


//	sprintf(buffer, "Trottle: %.3f", g_pPlayerPlane->getThrottle());
  //DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 140);

	AirplaneObject * pplayerAirplane = dynamic_cast<AirplaneObject*>(g_pPlayerObject);

	sprintf(buffer, "Elevator: %.3f, Aileron %.3f, Rudder %.3f", pplayerAirplane->getElevator(), 
		    pplayerAirplane->getAileron(), pplayerAirplane->getRudder());
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 120);

	//float speed = g_pPlayerPlane->getSpeed();
//	float alpha = g_pPlayerObject->getAngleOfAttack();
//	sprintf(buffer, "Alpha: %.3f", RadiansToDegrees(alpha) );
//	DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 100);

    StandardVector3 pos = g_pPlayerObject->getLocalPosition();
    sprintf(buffer, "LocalPosition: [%.3f, %.3f, %.3f]", pos.x,
               		pos.y, pos.z);
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 80);
    
   /* frame rate logging versus position -- temporary hack */
   static float next_dump = 0.0;
   float simtime = g_pSimTime->getSimTime();
   if (simtime > next_dump) {
   	next_dump = simtime + 10.0;
        CSP_LOG( CSP_APP , CSP_WARN,  framerate << endl << buffer ); 
   }

    pos = g_pPlayerObject->getGlobalPosition();
    sprintf(buffer, "GlobalPosition: [%.3f, %.3f, %.3f]", pos.x,
               		pos.y, pos.z);
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 60);

	StandardVector3 vel = g_pPlayerObject->getVelocity();
    sprintf(buffer, "Velocity: [%.3f, %.3f, %.3f]", vel.x,
               		vel.y, vel.z);
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 40);

    sprintf(buffer, "Heading: %.3f, Pitch: %.3f, Roll: %.3f",
                g_pPlayerObject->getHeading(), g_pPlayerObject->getPitch(), 
                g_pPlayerObject->getRoll() );

    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 20);


}
