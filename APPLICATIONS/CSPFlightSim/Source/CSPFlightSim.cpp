#ifdef WIN32
#include <windows.h>
#endif

#include "GL\GL.h"

#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#ifdef WIN32
#include <GL/glaux.h>		// Header File For The Glaux Library
#endif

#include "stdinc.h"


#include "SDL.h"
#include "CON_console.h"
#include "CON_consolecommands.h"
#include "DT_drawtext.h"

#include "DemeterException.h"


#include "CSPFlightSim.h"
#include "GameScreen.h"
#include "MenuScreen.h"
#include "LogoScreen.h"
#include "SimTime.h"
#include "ObjectFactory.h"
#include "GlobalCommands.h"
#include "ConsoleCommands.h"

#include "VirtualBattlefield.h"
#include "VirtualBattlefieldScene.h"
#include "CockpitDrawable.h"
#include "BaseInput.h"

#include <osg/Geode>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern VirtualBattlefield * g_pBattlefield;
//extern VirtualBattlefieldScene * g_pBattleScene;
extern SimTime * g_pSimTime;

extern ObjectFactory * g_pObjectFactory;

BaseObject * g_pPlayerObject;
BaseInput * g_pPlayerInput;

int g_ScreenWidth=800;
int g_ScreenHeight=600;

int ConsoleFont;

double g_LatticeXDist = 64000.0;
double g_LatticeYDist = 64000.0;


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
//    SDL_Delay(1000);

    InitConsole();

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
    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::ChangeScreen..." );
    m_PrevScreen = m_CurrentScreen;
    m_CurrentScreen = newScreen;
}

// Main Game loop
void CSPFlightSim::Run()
{
	static float averagedt = 0.0;
    static unsigned long stepNumber = 0;

    CSP_LOG(CSP_APP, CSP_DEBUG, "CSPFlightSim::Run..." );
    m_bFreezeSim = false;

    prevSceneTime = curSceneTime = SDL_GetTicks();
    try
    {

    while(!m_bFinished)
    {
        prevSceneTime = curSceneTime;
        curSceneTime = SDL_GetTicks();
        int diffTime = curSceneTime - prevSceneTime;
        float dt = (float)diffTime / 1000.0;

        if ( dt > 0.10) 
			dt = 0.10;
		else if (dt < 0.001)
			dt = 0.001;

	    //averagedt += dt;
        //++stepNumber;
        //dt = averagedt / stepNumber;

        g_pSimTime->updateSimTime(dt);    
        float simtime = g_pSimTime->getSimTime();

       // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        if (m_bShowStats)
        {
            float fps = 1000.0 / (curSceneTime - prevSceneTime);
            ShowStats(fps);
			ShowPlaneInfo();
        }

       // Swap OpenGL buffers
       SDL_GL_SwapBuffers();

       // remove marked objects, this should be done at the end of the main loop.
       g_pBattlefield->removeObjectsMarkedForDelete();


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

                        // see if the current screen handles this key
                        if (m_CurrentScreen && !m_bConsoleDisplay)
							 handled = m_CurrentScreen->OnKeyDown(event.key.keysym.sym);

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
    					            /* Send the event to the console */
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
                        char * key = SDL_GetKeyName(event.key.keysym.sym);
                        if (m_CurrentScreen)
                            m_CurrentScreen->OnKeyUp(event.key.keysym.sym);
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
void CSPFlightSim::UpdateObjects(float dt)
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

    int flags = SDL_OPENGL;

    flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_OPENGLBLIT ;
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
    }

	SDL_EnableUNICODE(1);
	//SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY / 16,SDL_DEFAULT_REPEAT_INTERVAL);

    return 0;
}

int CSPFlightSim::InitConsole()
{
    CSP_LOG( CSP_APP , CSP_DEBUG,  "CSPFlightSim::InitConsole()..." ); 

    
    SDL_Rect Con_rect;
	Con_rect.x = Con_rect.y = 0;
	Con_rect.w = Con_rect.h = 300;
    
	ConsoleFont = DT_LoadFont("Fonts\\ConsoleFont.bmp", 0);

    m_pConsole = CON_Init("Fonts\\ConsoleFont.bmp", m_SDLScreen, 100, Con_rect);
    CON_Alpha( m_pConsole, 50 );

	/* Add some commands to the console */
	CON_AddCommand(&KillProgram, "quit");
	CON_AddCommand(&AlphaChange, "alpha");
	CON_AddCommand(&Move, "move");
	CON_AddCommand(&Resize, "resize");
	CON_AddCommand(&ListCommands, "listcommands");
    CON_AddDefaultCommand(&DefaultCommand);

	CON_ListCommands(m_pConsole);


    return 0;
}


void CSPFlightSim::DoStartupScript()
{
    char startScriptFile[256];
    Config.GetString(startScriptFile, "StartupScript");
    string startCommand;
    startCommand = string("RUN ") + startScriptFile;
    ProcessCommandString(startCommand);
}


void CSPFlightSim::ShowPaused()
{
	DT_DrawText("PAUSED", m_SDLScreen, ConsoleFont, m_SDLScreen->w - 36, 0);
}

void CSPFlightSim::ShowPlaneInfo()
{
	int width = m_SDLScreen->w;
	char buffer[128];
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
    char framerate[30];
    char buffer[128];

    sprintf(framerate, "%.2f FPS", fps);
	DT_DrawText(framerate, m_SDLScreen, ConsoleFont, 1, 0);

	sprintf(buffer, "Terrain Polygons: %d", 
		g_pBattlefield->getTerrainPolygonsRendered() );
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 160);

	// do airplane specific output.
	AirplaneObject * pAirplane = dynamic_cast<AirplaneObject*>(g_pPlayerObject);
	if (pAirplane)
	{
		sprintf(buffer, "Trottle: %.3f", pAirplane->getThrottle());
		DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 140);

		sprintf(buffer, "Elevator: %.3f, Aileron %.3f, Rudder %.3f", pAirplane->getElevator(), pAirplane->getAileron(),
		    pAirplane->getRudder());
        DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 120);

	    float speed = pAirplane->getSpeed();
    	float alpha = pAirplane->getAngleOfAttack();
		sprintf(buffer, "Alpha: %.3f", RadiansToDegrees(alpha) );
		DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 100);
	}

    StandardVector3 pos = g_pPlayerObject->getLocalPosition();
    sprintf(buffer, "LocalPosition: [%.3f, %.3f, %.3f]", pos.x,
               		pos.y, pos.z);
    DT_DrawText(buffer, m_SDLScreen, ConsoleFont, 1, m_SDLScreen->h - 80);
    

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