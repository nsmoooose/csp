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
 * @file GameScreen.h
 *
 **/

#ifndef __GAMESCREEN_H__
#define __GAMESCREEN_H__

#include "AirplaneObject.h"
#include "BaseScreen.h"
#include "SDL_keysym.h"
#include "SDL_joystick.h"
#include "TypesVector3.h"


/**
 * class GameScreen - Describe me!
 *
 * @author unknown
 */
class GameScreen : public BaseScreen
{
public:
    virtual void OnInit();
    virtual void OnExit();

    virtual void OnRender();
    virtual void OnUpdateObjects(double dt);


    //virtual bool OnKeyDown(char * key); the base class method is not redefined here
	virtual bool OnKeyDown(SDLKey key);
    virtual bool OnKeyUp(SDLKey key);
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseButtonDown(int num, int x, int y);
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val);
	virtual void OnJoystickHatMotion(int joynum, int hat, int val);
    virtual void OnJoystickButtonDown(int joynum, int butnum);
	

protected:
	static double const angleOffset;
    int m_iViewMode;
	float m_fangleRotX, m_fangleRotZ, m_fdisToPlane, m_fangleOffsetX, m_fangleOffsetZ;
	float m_fscaleFactor;
    float missile_delay;
	StandardVector3 m_fixedCamPos;

	bool m_bInternalView;
	bool m_bPreviousState;

	void NormalView();
	void TurnViewAboutX(double fangleMax = pi / 2);
    void TurnViewAboutZ(double fangleMax = pi);
	void ScaleView();
	StandardVector3 GetNewFixedCamPos(BaseObject * const p_pPlayerPlane) const;
    void SetCamera();
};

#endif // __GAMESCREEN_H__

