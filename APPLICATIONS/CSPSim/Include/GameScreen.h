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

#include <osgUtil/SceneView>

#include <SimData/Ref.h>

#include "BaseScreen.h"
#include "ScreenInfoManager.h"
#include "DataRecorder.h"
#include "Views/CameraAgent.h"

class PyConsole;
class CameraCommand;
class DynamicObject;
class MouseCommand;

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

	virtual void onInit();
	virtual void onExit();

	virtual void onRender();
	virtual void onUpdate(double dt);

	virtual void setActiveObject(simdata::Ref<DynamicObject> const &);

	ACTION_INTERFACE(GameScreen, on_Quit);
	ACTION_INTERFACE(GameScreen, on_Pause);
	ACTION_INTERFACE(GameScreen, on_ToggleRecorder);
	ACTION_INTERFACE(GameScreen, on_ToggleWireframe);
	ACTION_INTERFACE(GameScreen, on_Stats);
	ACTION_INTERFACE(GameScreen, on_Console);
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
	ACTION_INTERFACE(GameScreen, on_PrintScreen);
	MOTION_INTERFACE(GameScreen, on_MouseView);

	virtual void initInterface();

protected: 
	size_t m_ViewMode;

	void setCamera(double dt);

	// text information
	osg::ref_ptr<ScreenInfoManager> m_ScreenInfoManager;
	osg::ref_ptr<osgUtil::SceneView> m_InfoView;
	osg::ref_ptr<osg::Group> m_InfoGroup;
	osg::ref_ptr<PyConsole> m_Console;

	simdata::Ref<DynamicObject> m_ActiveObject;

	// data recorder
	simdata::Ref<DataRecorder> m_DataRecorder;
	void setRecorder(bool on);

	// camera management by a command
	CameraAgent m_CameraAgent;
	CameraCommand *m_PanLeft,*m_PanRight,*m_PanLeftRightStop;
	CameraCommand *m_PanUp,*m_PanDown,*m_PanUpDownStop;
	CameraCommand *m_ZoomIn,*m_ZoomOut,*m_ZoomStop,*m_ZoomStepIn,*m_ZoomStepOut;
	MouseCommand* m_Mouse;
	CameraCommand* m_CurrentCameraCommand;
	void createCameraCommand();
	void deleteCameraCommands();
};

#endif // __GAMESCREEN_H__

