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

#ifndef __CSPSIM_H__
#define __CSPSIM_H__

#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>

#include <SimData/Types.h>

#include "DynamicObject.h"
#include "TerrainObject.h"


class VirtualBattlefield;
class VirtualHID;
class BaseScreen;
class GameScreen;
class EventMapIndex;


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
	virtual void Init();
	virtual void Run();
	virtual void Quit();
	virtual void Exit();
	virtual void Cleanup();

	void ChangeScreen(BaseScreen * newScreen);
	SDL_Surface * GetSDLScreen() {return m_SDLScreen;};

	simdata::SimDate & getCurrentTime() { return m_CurrentTime; }
	simdata::SimTime const & getFrameRate() const{ return m_FrameRate; }

	void setActiveObject(simdata::Pointer<DynamicObject> object);
	simdata::Pointer<DynamicObject> const getActiveObject() const;
	VirtualBattlefield * const getBattlefield() const;


	EventMapIndex *getInterfaceMaps() { return m_InterfaceMaps; }

	void togglePause();

	simdata::DataArchive * getDataArchive() { return m_DataArchive; }

protected:
	
	void InitSim();
	int InitSDL();

	void DoInput();
	void UpdateObjects(double dt);
	void DoStartupScript();

private:

	SDL_Surface *m_SDLScreen;
	SDL_Joystick *m_SDLJoystick;

	BaseScreen *m_CurrentScreen;
	BaseScreen *m_PrevScreen;

	BaseScreen *m_LogoScreen;
	GameScreen *m_GameScreen;
	BaseScreen *m_MainMenuScreen;

	int m_ScreenWidth;
	int m_ScreenHeight;

	bool m_bFreezeSim;
	bool m_bFinished;

	//SimPlayer *m_Player;
	
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
	VirtualHID *m_Interface;
	EventMapIndex *m_InterfaceMaps;
	simdata::Pointer<DynamicObject> m_ActiveObject;

	/**
	 * The virtual battlefield
	 */
	VirtualBattlefield *m_Battlefield;

	// TODO the terrain will eventually be encapsulated in a Theater class
	simdata::PathPointer<TerrainObject> m_ActiveTerrain;
	simdata::DataArchive *m_DataArchive;
};

#endif // __CSPSIM_H__

