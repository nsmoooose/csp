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

#include <osg/ref_ptr>
#include <SimData/Ref.h>
#include <SimData/ScopedPointer.h>
#include "BaseScreen.h"

class PyConsole;
class DynamicObject;
class MouseCommand;
class ScreenInfoManager;
class DataRecorder;
class CameraAgent;
class CameraCommand;
struct CameraCommands;

namespace osgUtil {
	class SceneView;
}

namespace osg {
	class Group;
}

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

	DECLARE_INPUT_INTERFACE(GameScreen, BaseScreen)
		BIND_ACTION("QUIT", on_Quit);
		BIND_ACTION("PAUSE", on_Pause);
		BIND_ACTION("TOGGLE_RECORDER", on_ToggleRecorder);
		BIND_ACTION("TOGGLE_WIREFRAME", on_ToggleWireframe);
		BIND_ACTION("STATS", on_Stats);
		BIND_ACTION("CONSOLE", on_Console);
		BIND_ACTION("CHANGE_VEHICLE", on_ChangeVehicle);
		BIND_ACTION("CAMERA_VIEW_0", on_View0);
		BIND_ACTION("CAMERA_VIEW_1", on_View1);
		BIND_ACTION("CAMERA_VIEW_2", on_View2);
		BIND_ACTION("CAMERA_VIEW_3", on_View3);
		BIND_ACTION("CAMERA_VIEW_4", on_View4);
		BIND_ACTION("CAMERA_VIEW_5", on_View5);
		BIND_ACTION("CAMERA_VIEW_6", on_View6);
		BIND_ACTION("CAMERA_VIEW_7", on_View7);
		BIND_ACTION("CAMERA_VIEW_8", on_View8);
		BIND_ACTION("CAMERA_VIEW_9", on_View9);
		BIND_ACTION("CAMERA_PAN_LEFT", on_ViewPanLeft);
		BIND_ACTION("CAMERA_PAN_RIGHT", on_ViewPanRight);
		BIND_ACTION("CAMERA_PAN_LEFT_STOP", on_ViewPanLeftStop);
		BIND_ACTION("CAMERA_PAN_RIGHT_STOP", on_ViewPanRightStop);
		BIND_ACTION("CAMERA_PAN_UP", on_ViewPanUp);
		BIND_ACTION("CAMERA_PAN_DOWN", on_ViewPanDown);
		BIND_ACTION("CAMERA_PAN_UP_STOP", on_ViewPanUpStop);
		BIND_ACTION("CAMERA_PAN_DOWN_STOP", on_ViewPanDownStop);
		BIND_ACTION("CAMERA_ZOOM_IN", on_ViewZoomIn);
		BIND_ACTION("CAMERA_ZOOM_OUT", on_ViewZoomOut);
		BIND_ACTION("CAMERA_ZOOM_STOP", on_ViewZoomStop);
		BIND_ACTION("CAMERA_ZOOM_STEP_IN", on_ViewZoomStepIn);
		BIND_ACTION("CAMERA_ZOOM_STEP_OUT", on_ViewZoomStepOut);
		BIND_ACTION("CAMERA_FOV_STEP_DEC", on_ViewFovStepDec);
		BIND_ACTION("CAMERA_FOV_STEP_INC", on_ViewFovStepInc);
		BIND_ACTION("SPIN_THE_WORLD", on_SpinTheWorld);
		BIND_ACTION("SPIN_THE_WORLD_STOP", on_SpinTheWorldStop);
		BIND_ACTION("PRINT_SCREEN", on_PrintScreen);
		BIND_ACTION("RESET_SPIN", on_ResetSpin);
		BIND_MOTION("CAMERA_PAN", on_MouseView);
	END_INPUT_INTERFACE

public:

	// input event handlers
	void on_Quit();
	void on_Pause();
	void on_ToggleRecorder();
	void on_ToggleWireframe();
	void on_Stats();
	void on_Console();
	void on_ChangeVehicle();
	void on_View0();
	void on_View1();
	void on_View2();
	void on_View3();
	void on_View4();
	void on_View5();
	void on_View6();
	void on_View7();
	void on_View8();
	void on_View9();
	void on_ViewPanLeft();
	void on_ViewPanRight();
	void on_ViewPanLeftStop();
	void on_ViewPanRightStop();
	void on_ViewPanUp();
	void on_ViewPanDown();
	void on_ViewPanUpStop();
	void on_ViewPanDownStop();
	void on_ViewZoomIn();
	void on_ViewZoomOut();
	void on_ViewZoomStop();
	void on_ViewZoomStepIn();
	void on_ViewZoomStepOut();
	void on_ViewFovStepDec();
	void on_ViewFovStepInc();
	void on_SpinTheWorld();
	void on_SpinTheWorldStop();
	void on_ResetSpin();
	void on_PrintScreen();
	void on_MouseView(int x, int y, int dx, int dy);

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
	simdata::ScopedPointer<CameraAgent> m_CameraAgent;
	simdata::ScopedPointer<CameraCommands> m_CameraCommands;
	CameraCommand* m_CurrentCameraCommand;
	/*
	CameraCommand *m_PanLeft,*m_PanRight,*m_PanLeftRightStop;
	CameraCommand *m_PanUp,*m_PanDown,*m_PanUpDownStop;
	CameraCommand *m_ZoomIn,*m_ZoomOut,*m_ZoomStop,*m_ZoomStepIn,*m_ZoomStepOut;
	MouseCommand* m_Mouse;
	CameraCommand* m_CurrentCameraCommand;
	void createCameraCommand();
	void deleteCameraCommands();
	*/
};

#endif // __GAMESCREEN_H__

