// Combat Simulator Project
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

#ifndef __CSPSIM_GAMESCREEN_H__
#define __CSPSIM_GAMESCREEN_H__

#include <csp/cspsim/BaseScreen.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/util/Callback.h>

#include <osg/ref_ptr>

namespace osg { class Group; }
namespace osgUtil { class SceneView; }

CSP_NAMESPACE

class DynamicObject;
class MouseCommand;
class ScreenInfoManager;
class DataRecorder;
class CameraAgent;
class CameraCommand;
struct CameraCommands;


class GameScreen : public BaseScreen {
public:
	GameScreen();
	virtual ~GameScreen();

	virtual void onInit();
	virtual void onExit();

	virtual void onRender();
	virtual void onUpdate(double dt);

	virtual void setActiveObject(Ref<DynamicObject> const &);

	DECLARE_INPUT_INTERFACE(GameScreen, BaseScreen)
		BIND_ACTION("QUIT", on_Quit);
		BIND_ACTION("PAUSE", on_Pause);
		BIND_ACTION("TOGGLE_RECORDER", on_ToggleRecorder);
		BIND_ACTION("TOGGLE_WIREFRAME", on_ToggleWireframe);
		BIND_ACTION("STATS", on_Stats);
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
		BIND_ACTION("CAMERA_SELECT", on_Select);
		BIND_ACTION("CAMERA_RESELECT", on_Reselect);
		BIND_ACTION("CAMERA_DESELECT", on_Deselect);
		BIND_ACTION("CAMERA_FORWARD", on_LookForward);
		BIND_ACTION("CAMERA_BACKWARD", on_LookBackward);
		BIND_ACTION("CAMERA_RIGHT", on_LookRight);
		BIND_ACTION("CAMERA_LEFT", on_LookLeft);
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
		//BIND_ACTION("PRINT_SCREEN", on_PrintScreen);
		BIND_ACTION("RESET_SPIN", on_ResetSpin);
		BIND_ACTION("LABELS_OFF", on_LabelsOff);
		BIND_ACTION("LABELS_ON", on_LabelsOn);
		BIND_ACTION("LABELS_TOGGLE", on_LabelsToggle);
		BIND_CLICK("LEFT_CLICK", on_LeftClick);
		BIND_MOTION("CAMERA_PAN", on_MouseView);
	END_INPUT_INTERFACE  // protected:

public:

	// input event handlers
	void on_Quit();
	void on_Pause();
	void on_ToggleRecorder();
	void on_ToggleWireframe();
	void on_Stats();
	void on_ChangeVehicle();
	void on_LookForward();
	void on_LookBackward();
	void on_LookRight();
	void on_LookLeft();
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
	void on_Select();
	void on_Reselect();
	void on_Deselect();
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
	//void on_PrintScreen();
	void on_LabelsOn();
	void on_LabelsOff();
	void on_LabelsToggle();
	void on_LeftClick(MapEvent::ClickEvent const &event);
	void on_MouseView(MapEvent::MotionEvent const &event);

	virtual void initInterface();

private:
	size_t m_ViewMode;

	void setCamera(double dt);

	// text information
	osg::ref_ptr<ScreenInfoManager> m_ScreenInfoManager;

	Ref<DynamicObject> m_ActiveObject;

	// data recorder
	Ref<DataRecorder> m_DataRecorder;
	void setRecorder(bool on);

	// camera management by a command
	ScopedPointer<CameraAgent> m_CameraAgent;
	ScopedPointer<CameraCommands> m_CameraCommands;

	void onPlayerJoin(int, const std::string&);
	void onPlayerQuit(int, const std::string&);
	callback<void, int, const std::string&> m_OnPlayerJoin;
	callback<void, int, const std::string&> m_OnPlayerQuit;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_GAMESCREEN_H__

