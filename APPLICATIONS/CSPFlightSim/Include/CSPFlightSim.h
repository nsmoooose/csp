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
 * @file CSPFlightSim.h
 *
 **/

#ifndef __CSPFLIGHTSIM_H__
#define __CSPFLIGHTSIM_H__


#include "BaseScreen.h"
#include "BaseObject.h"
#include "SDL.h"
#include "CON_console.h"

/**
 * class CSPFlightSim - Describe me!
 *
 * @author unknown
 */
class CSPFlightSim
{

public:
    CSPFlightSim();
    virtual ~CSPFlightSim();
    virtual void Init();
    virtual void Run();
    virtual void Exit();
    virtual void Cleanup();

    void ChangeScreen(BaseScreen * newScreen);
	SDL_Surface * GetSDLScreen() {return m_SDLScreen;};

protected:
	void InitSim();
    int InitSDL();
    int InitConsole();
    void ShowStats(float fps);
    void ShowPlaneInfo();
	void ShowPaused();
    
    void DoInput();
    void UpdateObjects(double dt);
    void DoStartupScript();


private:
    SDL_Surface * m_SDLScreen;
    bool m_bFinished;
    BaseScreen * m_CurrentScreen;
    BaseScreen * m_PrevScreen;

    BaseScreen * m_logoScreen;
    BaseScreen * m_gameScreen;
    BaseScreen * m_MainMenuScreen;

    bool m_bConsoleDisplay;
    ConsoleInformation * m_pConsole;
    bool m_bFreezeSim;
    bool m_bShowStats;


};




#endif
