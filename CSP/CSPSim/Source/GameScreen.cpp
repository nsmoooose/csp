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
#include <osgUtil/SceneView>
#include <osgText/Text>
#include <Producer/Camera>

#include "Animation.h"
#include "ConsoleCommands.h"
#include "CSPSim.h"
#include "DataRecorder.h"
#include "DynamicObject.h"
#include "EventMapIndex.h"
#include "glDiagnostics.h"
#include "ObjectModel.h"
#include "ScreenInfoManager.h"
#include "Views/CameraAgent.h"
#include "Views/CameraCommand.h"
#include "VirtualScene.h"

#include <SimCore/Battlefield/LocalBattlefield.h>
#include <SimCore/Util/Log.h>


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


DEFINE_INPUT_INTERFACE(GameScreen);

void GameScreen::initInterface()
{
	assert(!m_Interface);
	m_Interface = new VirtualHID(); //GameScreenInterface();
	m_Interface->bindObject(this);
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
    virtual void operator()(const Producer::Camera& /*camera*/) {
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

/*
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
*/

GameScreen::GameScreen():
	BaseScreen(),
	m_ActiveObject(0),
	m_CameraAgent(new CameraAgent(ViewFactory())),
	m_CameraCommands(new CameraCommands),
	m_CurrentCameraCommand(0) {
	initInterface();
	//createCameraCommand();
}

GameScreen::~GameScreen() {
	//deleteCameraCommands();
	m_CurrentCameraCommand = 0;
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
		if (scene && m_ActiveObject->isNearField()) {
			scene->setNearObject(m_ActiveObject, false);
		}
		setRecorder(false);
	}
	m_ActiveObject = object;
	m_CameraAgent->setObject(object);
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

	// FIXME pushing and popping state around the SceneView.draw call should
	// _not_ be necessary, yet all three of the SceneViews currently used by
	// CSP leak state.  It's not clear where the state leaks occur, and the
	// whole problem may go away when we upgrade to the next version of OSG.
	glPushAttrib(GL_ALL_ATTRIB_BITS); // FIXME
	glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS); // FIXME
	//GlStateSnapshot snapshot; // <-- uncomment this to log the state leak
	m_InfoView->draw();
	//snapshot.logDiff("info view"); // <-- uncomment this to log the state leak
	glPopClientAttrib(); // FIXME
	glPopAttrib(); // FIXME
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
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View2() {
	m_ViewMode = 2;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View3() {
	m_ViewMode = 3;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View4() {
	m_ViewMode = 4;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View5() {
	m_ViewMode = 5;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View6() {
	m_ViewMode = 6;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View7() {
	m_ViewMode = 7;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View8() {
	m_ViewMode = 8;
	m_CameraAgent->setViewMode(m_ViewMode);
}

void GameScreen::on_View9() {
	m_ViewMode = 9;
	m_CameraAgent->setViewMode(m_ViewMode);
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
	LocalBattlefield *battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		simdata::Ref<DynamicObject> object;
		object = battlefield->getNextUnit(m_ActiveObject, 1, 1, 0);
		if (object.valid()) CSPSim::theSim->setActiveObject(object);
	}
}

void GameScreen::on_ViewPanLeft() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanLeft;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanLeft);
}

void GameScreen::on_ViewPanRight() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanRight;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanRight);
}

void GameScreen::on_ViewPanLeftStop() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanLeftRightStop;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanLeftRightStop);
}

void GameScreen::on_ViewPanRightStop() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanLeftRightStop;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanLeftRightStop);
}

void GameScreen::on_ViewPanUpStop() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanUpDownStop;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanUpDownStop);
}

void GameScreen::on_ViewPanDownStop() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanUpDownStop;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanUpDownStop);
}

void GameScreen::on_ViewZoomStop() {
//	m_CurrentCameraCommand = &m_CameraCommands->ZoomStop;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomStop);
}

void GameScreen::on_ViewPanUp() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanUp;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanUp);
}

void GameScreen::on_ViewPanDown() {
//	m_CurrentCameraCommand = &m_CameraCommands->PanDown;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanDown);
}

void GameScreen::on_ViewZoomIn() {
//	m_CurrentCameraCommand = &m_CameraCommands->ZoomIn;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomIn);
}

void GameScreen::on_ViewZoomOut() {
//	m_CurrentCameraCommand = &m_CameraCommands->ZoomOut;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomOut);
}

void GameScreen::on_ViewZoomStepIn() {
//	m_CurrentCameraCommand = &m_CameraCommands->ZoomStepIn;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomStepIn);
}

void GameScreen::on_ViewZoomStepOut() {
//	m_CurrentCameraCommand = &m_CameraCommands->ZoomStepOut;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomStepOut);
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
	m_CameraCommands->Mouse.set(x,y,dx,dy);
//	m_CurrentCameraCommand = &m_CameraCommands->Mouse;
	m_CameraAgent->setCameraCommand(&m_CameraCommands->Mouse);
}

void GameScreen::setCamera(double dt) {
	//m_CameraAgent->set(m_ViewMode, m_CurrentCameraCommand);
	m_CameraAgent->updateCamera(dt);
	m_CurrentCameraCommand = 0;

	LocalBattlefield* battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		battlefield->setCamera(m_CameraAgent->getEyePoint(), m_CameraAgent->getLookPoint(), m_CameraAgent->getUpVector());
	}
}

