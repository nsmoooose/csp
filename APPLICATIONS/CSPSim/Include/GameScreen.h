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

#include <SimData/Math.h>
#include <SimData/Vector3.h>

#include "BaseScreen.h"
#include "DynamicObject.h"
#include "HID.h"


class VirtualBattlefield;


/*
class GameScreenInterface : public VirtualHID
{
public:
	GameScreenInterface();
protected:
};
*/


/**
 * class GameScreen - Describe me!
 *
 * @author unknown
 */
class GameScreen : public BaseScreen
{
public:
	GameScreen();
	virtual ~GameScreen();

	virtual void OnInit();
	virtual void OnExit();

	virtual void OnRender();
	virtual void OnUpdateObjects(double dt);

	virtual void setActiveObject(simdata::PointerBase &);

	ACTION_INTERFACE(GameScreen, on_Quit);
	ACTION_INTERFACE(GameScreen, on_Pause);
	ACTION_INTERFACE(GameScreen, on_ChangeVehicle);
	ACTION_INTERFACE(GameScreen, on_View0);
	ACTION_INTERFACE(GameScreen, on_View1);
	ACTION_INTERFACE(GameScreen, on_View2);
	ACTION_INTERFACE(GameScreen, on_View3);
	ACTION_INTERFACE(GameScreen, on_View4);
	ACTION_INTERFACE(GameScreen, on_View5);
	ACTION_INTERFACE(GameScreen, on_View6);
	ACTION_INTERFACE(GameScreen, on_View7);
	ACTION_INTERFACE(GameScreen, on_View8);
	ACTION_INTERFACE(GameScreen, on_View9);
	ACTION_INTERFACE(GameScreen, on_ViewPanLeft);
	ACTION_INTERFACE(GameScreen, on_ViewPanRight);
	ACTION_INTERFACE(GameScreen, on_ViewPanLeftStop);
	ACTION_INTERFACE(GameScreen, on_ViewPanRightStop);
	ACTION_INTERFACE(GameScreen, on_ViewPanUp);
	ACTION_INTERFACE(GameScreen, on_ViewPanDown);
	ACTION_INTERFACE(GameScreen, on_ViewPanUpStop);
	ACTION_INTERFACE(GameScreen, on_ViewPanDownStop);
	ACTION_INTERFACE(GameScreen, on_ViewZoomIn);
	ACTION_INTERFACE(GameScreen, on_ViewZoomOut);
	ACTION_INTERFACE(GameScreen, on_ViewZoomStop);
	ACTION_INTERFACE(GameScreen, on_ViewZoomStepIn);
	ACTION_INTERFACE(GameScreen, on_ViewZoomStepOut);
	ACTION_INTERFACE(GameScreen, on_ViewFovStepDec);
	ACTION_INTERFACE(GameScreen, on_ViewFovStepInc);
	ACTION_INTERFACE(GameScreen, on_SpinTheWorld);
	ACTION_INTERFACE(GameScreen, on_SpinTheWorldStop);
	ACTION_INTERFACE(GameScreen, on_ResetSpin);
	MOTION_INTERFACE(GameScreen, on_MouseView);

	virtual void InitInterface();

	void SetBattlefield(VirtualBattlefield *);

protected:
	static double const angleOffset;
	int m_iViewMode;
	float m_fangleRotX, m_fangleRotZ, m_fdisToObject, m_fangleOffsetX, m_fangleOffsetZ;
	float m_fscaleFactor;
	simdata::Vector3 m_fixedCamPos;

	bool m_bInternalView;
	bool m_bPreviousState;

	void NormalView();
	void TurnViewAboutX(double fangleMax = G_PI / 2);
	void TurnViewAboutZ(double fangleMax = G_PI);
	void ScaleView();
	simdata::Vector3 GetNewFixedCamPos(SimObject * const target) const;
	void SetCamera();

	simdata::Pointer<DynamicObject> m_ActiveObject;
	VirtualBattlefield *m_Battlefield;
};

#endif // __GAMESCREEN_H__

