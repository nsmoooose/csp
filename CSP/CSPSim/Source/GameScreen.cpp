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
 * @file GameScreen.cpp
 *
 **/


#include "GameScreen.h"

#include <ctime>
#include <iomanip>

#include <osg/Image>
#include <osgDB/WriteFile>
#include <Producer/Camera>

#include "ConsoleCommands.h"
#include "CSPSim.h"
#include "EventMapIndex.h"
#include "Log.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "Views/CameraCommand.h"



/*
 * TODO
 *   padlock fixes:
 *       o view 1<->9 switch needs to get preserve view direction
 *       o add some histyresis to the head snap
 *       o model break lock
 *       o model acquire lock
 *       o model out of view (e.g. below fuselage)
 *       o same limits for padlock and view 1
 *       o g-force reduces range of motion and slew speed, eliminates offset
 *       o smooth the min/max functions
 *       o modularize
 *       o optimize
 *       o externalize parameters
 */

void GameScreen::initInterface()
{
	assert(!m_Interface);
	m_Interface = new VirtualHID(); //GameScreenInterface();
	m_Interface->bindObject(this);
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
	simdata::Ref<EventMapIndex> maps = CSPSim::theSim->getInterfaceMaps();
	if (maps.valid()) {
		EventMapping::Ref map = maps->getMap("__gamescreen__");
		if (map != NULL) {
			m_Interface->setMapping(map);
		} else {
			CSP_LOG( APP , ERROR, "HID interface map '__gamescreen__' not found.");
		}
	} else {
		CSP_LOG( APP , ERROR, "No HID interface maps defined, '__gamescreen__' not found.");
	}
}

// XXX: Preparing for the jump to OpenProducer ...
class SnapImageDrawCallback: public Producer::Camera::Callback {
	std::string m_Filename, m_Ext;
    bool m_SnapImageOnNextFrame;
	std::string getDate() {
		time_t timer;
		time(&timer);
		tm* time_ptr = localtime(&timer);
		std::ostringstream os;
		os.fill('0');
		os << std::setw(2) << time_ptr->tm_mon + 1 << std::setw(2) << time_ptr->tm_mday << std::setw(2) << time_ptr->tm_year - 100;
		std::string day = os.str();
		os.str("");
		os << std::setw(2) << time_ptr->tm_hour << std::setw(2) << time_ptr->tm_min << std::setw(2) << time_ptr->tm_sec;
		std::string hour = os.str();
		return day + '-' + hour;
	}
public:
	// XXX: next release of osg should allow saving to jpg instead of bmp
	SnapImageDrawCallback(const std::string& filename = "CSP",const std::string& ext = ".bmp"):
        m_Filename(filename),
		m_Ext(".bmp"),
        m_SnapImageOnNextFrame(false){
    }
    void setSnapImageOnNextFrame(bool flag) { 
		m_SnapImageOnNextFrame = flag; 
	}
    bool getSnapImageOnNextFrame() const { 
		return m_SnapImageOnNextFrame; 
	}
    virtual void operator()(const Producer::Camera& camera) {
		if (m_SnapImageOnNextFrame) {
			int x,y,width,height;
			//unsigned int width,height;

			//camera.getProjectionRectangle(x,y,width,height);
			VirtualScene* scene = CSPSim::theSim->getScene();
			scene->getViewport(x,y,width,height);

			osg::ref_ptr<osg::Image> image = new osg::Image;
			image->readPixels(x,y,width,height,GL_RGB,GL_UNSIGNED_BYTE);

			// save the file in the form CSPScreenmmddyy-hhmmss.ext
			osgDB::writeImageFile(*image,m_Filename + getDate() + m_Ext);
			m_SnapImageOnNextFrame = false;
		}
    }    
};

void GameScreen::on_PrintScreen() {
	// for now, it's on previous frame... but that should work.
	SnapImageDrawCallback sn;
	sn.setSnapImageOnNextFrame(true);
	osg::ref_ptr<Producer::Camera> camera = new Producer::Camera;
	sn(*camera);
}

void GameScreen::createCameraCommand() {
	m_PanLeft = new PanLeft;
	m_PanRight = new PanRight;
	m_PanLeftRightStop = new PanLeftRightStop;
	m_PanUp = new PanUp;
	m_PanDown = new PanDown;
	m_PanUpDownStop = new PanUpDownStop;
	m_ZoomIn = new ZoomIn;
	m_ZoomOut = new ZoomOut;
	m_ZoomStop = new ZoomStop;
	m_ZoomStepIn = new ZoomStepIn;
	m_ZoomStepOut = new ZoomStepOut;
	m_Mouse = new MouseCommand;
}

void GameScreen::deleteCameraCommands() {
	delete m_PanLeft;
	delete m_PanRight;
	delete m_PanLeftRightStop;
	delete m_PanUp;
	delete m_PanDown;
	delete m_PanUpDownStop;
	delete m_ZoomIn;
	delete m_ZoomOut;
	delete m_ZoomStop;
	delete m_Mouse;
	delete m_ZoomStepIn;
	delete m_ZoomStepOut;
	m_CurrentCameraCommand = 0;
}

GameScreen::GameScreen(): 
	BaseScreen(), 
	m_ActiveObject(0),
	m_CameraAgent(ViewFactory()),
	m_CurrentCameraCommand(0) {
	initInterface();
	createCameraCommand();
}

GameScreen::~GameScreen() {
	deleteCameraCommands();
}

void GameScreen::onInit() {
	// add a layer for texts on screen
	m_InfoView = new osgUtil::SceneView();
	m_InfoView->setDefaults();

	int ScreenWidth = CSPSim::theSim->getSDLScreen()->w;
	int ScreenHeight = CSPSim::theSim->getSDLScreen()->h;

	m_InfoView->setViewport(0,0,ScreenWidth,ScreenHeight);

	m_InfoView->getRenderStage()->setClearMask(GL_DEPTH_BUFFER_BIT);

	m_ScreenInfoManager = new ScreenInfoManager(ScreenWidth,ScreenHeight);
	m_ScreenInfoManager->setName("ScreenInfoManager");
	m_ScreenInfoManager->setStatus("PAUSE", CSPSim::theSim->isPaused());
	m_ScreenInfoManager->setStatus("RECORD", false);

	m_Console = new PyConsole(ScreenWidth, ScreenHeight);
	m_Console->setName("PyConsole");

	m_InfoGroup = new osg::Group;
	m_InfoGroup->addChild(m_ScreenInfoManager.get());
	m_InfoGroup->addChild(m_Console.get());

	m_InfoView->setSceneData(m_InfoGroup.get());

	simdata::Ref<DynamicObject> ao = CSPSim::theSim->getActiveObject();
	if (ao.valid())
		setActiveObject(ao);
}

void GameScreen::onExit() {
}

void GameScreen::setActiveObject(simdata::Ref<DynamicObject> const &object) {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (m_ActiveObject.valid() && m_ActiveObject != object) {
		if (scene && m_ActiveObject->getNearFlag()) {
			scene->setNearObject(m_ActiveObject, false);
		}
		setRecorder(false);
	}
	m_ActiveObject = object;
	m_CameraAgent.setObject(object);
	if (object.valid() && object->isHuman() && object->isLocal()) {
		on_View1();
	} else {
		on_View2();
	}
	if (object.valid()) {
		int ScreenWidth = CSPSim::theSim->getSDLScreen()->w;
		int ScreenHeight = CSPSim::theSim->getSDLScreen()->h;
		m_ScreenInfoManager->changeObjectStats(ScreenWidth,ScreenHeight,object);
	}
}

void GameScreen::onRender() {
	// Draw the whole scene
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->drawScene();
	}
	m_InfoView->cull();
	m_InfoView->draw();
}

void GameScreen::onUpdate(double dt) {
	static short i = 0;
	setCamera(dt);
	if ((++i)%3 == 0) {
		m_InfoView->update();
	}
	if (m_DataRecorder.valid()) {
		m_DataRecorder->timeStamp(dt);
	}
}

void GameScreen::on_View1() {
	m_ViewMode = 1;
}

void GameScreen::on_View2() {
	m_ViewMode = 2;
}


void GameScreen::on_View3() {
	m_ViewMode = 3;
}

void GameScreen::on_View4() {
	m_ViewMode = 4;
}

void GameScreen::on_View5() {
	m_ViewMode = 5;
}

void GameScreen::on_View6() {
	m_ViewMode = 6;
}

void GameScreen::on_View7() {
	m_ViewMode = 7;
}

void GameScreen::on_View8() {
	m_ViewMode = 8;
}

void GameScreen::on_View9() {
	m_ViewMode = 9;
}

void GameScreen::on_View0() {
}

void GameScreen::on_Quit()
{
	CSPSim::theSim->quit();
}

void GameScreen::on_Pause()
{
	CSPSim::theSim->togglePause();
	m_ScreenInfoManager->setStatus("PAUSE", !m_ScreenInfoManager->getStatus("PAUSE"));
}

void GameScreen::setRecorder(bool on) {
	if (on && !m_DataRecorder && m_ActiveObject.valid()) {
		static int n = 0;
		char fn[128];
		sprintf(fn, "data-%03d.rec", n);
		m_DataRecorder = new DataRecorder(fn);
		m_ActiveObject->setDataRecorder(m_DataRecorder.get());
	} else
	if (!on && m_DataRecorder.valid()) {
		if (m_ActiveObject.valid()) {
			m_ActiveObject->setDataRecorder(NULL);
		}
		m_DataRecorder->close();
		m_DataRecorder = NULL;
	}
	m_ScreenInfoManager->setStatus("RECORD", m_DataRecorder.valid());
}

void GameScreen::on_ToggleRecorder() {
	setRecorder(!m_DataRecorder);
}

void GameScreen::on_ToggleWireframe() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->setWireframeMode(!scene->getWireframeMode());
	}
}

void GameScreen::on_Stats() {
	m_ScreenInfoManager->setStatus("GENERAL STATS", !m_ScreenInfoManager->getStatus("GENERAL STATS"));
	m_ScreenInfoManager->setStatus("OBJECT STATS", !m_ScreenInfoManager->getStatus("OBJECT STATS"));
}

void GameScreen::on_Console()
{
	CSPSim::theSim->runConsole(m_Console.get());
}

void GameScreen::on_ChangeVehicle()
{
	simdata::Ref<DynamicObject> object;
	VirtualBattlefield *battlefield = CSPSim::theSim->getBattlefield();
	object = battlefield->getNextUnit(m_ActiveObject, -1, -1, -1);
	if (object.valid()) CSPSim::theSim->setActiveObject(object);
}

void GameScreen::on_ViewPanLeft() {
	m_CurrentCameraCommand = m_PanLeft;
}

void GameScreen::on_ViewPanRight() {
	m_CurrentCameraCommand = m_PanRight;
}

void GameScreen::on_ViewPanLeftStop() {
	m_CurrentCameraCommand = m_PanLeftRightStop;
}

void GameScreen::on_ViewPanRightStop() {
	m_CurrentCameraCommand = m_PanLeftRightStop;
}

void GameScreen::on_ViewPanUpStop() {
	m_CurrentCameraCommand = m_PanUpDownStop;
}

void GameScreen::on_ViewPanDownStop() {
	m_CurrentCameraCommand = m_PanUpDownStop;
}

void GameScreen::on_ViewZoomStop() {
	m_CurrentCameraCommand = m_ZoomStop;
}

void GameScreen::on_ViewPanUp() {
	m_CurrentCameraCommand = m_PanUp;
}

void GameScreen::on_ViewPanDown() {
	m_CurrentCameraCommand = m_PanDown;
}

void GameScreen::on_ViewZoomIn() {
	m_CurrentCameraCommand = m_ZoomIn;
}

void GameScreen::on_ViewZoomOut() {
	m_CurrentCameraCommand = m_ZoomOut;
}

void GameScreen::on_ViewZoomStepIn() {
	m_CurrentCameraCommand = m_ZoomStepIn;
}

void GameScreen::on_ViewZoomStepOut() {
	m_CurrentCameraCommand = m_ZoomStepOut;
}

void GameScreen::on_ViewFovStepDec() 
{
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		float fov = scene->getViewAngle() * 0.8;
		if (fov > 10.0) {
			scene->setViewAngle(fov);
		}
	}
}

void GameScreen::on_ViewFovStepInc()
{
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		float fov = scene->getViewAngle() * 1.20;
		if (fov < 90.0) {
			scene->setViewAngle(fov);
		}
	}
}

void GameScreen::on_SpinTheWorld()
{
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->spinTheWorld(true);
	}
}

void GameScreen::on_SpinTheWorldStop() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->spinTheWorld(false);
	}
}

void GameScreen::on_ResetSpin() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->resetSpin();
	}
}

void GameScreen::on_MouseView(int x, int y, int dx, int dy) {
	m_Mouse->set(x,y,dx,dy);
	m_CurrentCameraCommand = m_Mouse;	
}

void GameScreen::setCamera(double dt) {	
	m_CameraAgent.set(m_ViewMode,m_CurrentCameraCommand);
	m_CameraAgent.updateCamera(dt);
	m_CurrentCameraCommand = 0;

	VirtualBattlefield* battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		battlefield->updateOrigin(m_CameraAgent.getEyePoint()); 
		battlefield->setCamera(m_CameraAgent.getEyePoint());
	}

	VirtualScene* scene = CSPSim::theSim->getScene();
	if (scene) {	
		scene->setLookAt(m_CameraAgent.getEyePoint(),m_CameraAgent.getLookPoint(),m_CameraAgent.getUpVector());
	}
}

