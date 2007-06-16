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
 * @file GameScreen.cpp
 *
 **/

#include <csp/cspsim/GameScreen.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DataRecorder.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/EventMapIndex.h>
#include <csp/cspsim/glDiagnostics.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/ScreenInfoManager.h>
#include <csp/cspsim/VirtualScene.h>
#include <csp/cspsim/battlefield/LocalBattlefield.h>
#include <csp/cspsim/views/CameraAgent.h>
#include <csp/cspsim/views/CameraCommand.h>
#include <csp/cspsim/views/View.h>
#include <csp/cspsim/wf/WindowManager.h>

#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Log.h>

#include <osg/Image>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgUtil/SceneView>
#include <osgText/Text>
//#include <Producer/Camera>

#include <ctime>
#include <iomanip>

CSP_NAMESPACE

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


DEFINE_INPUT_INTERFACE(GameScreen)

void GameScreen::initInterface()
{
	assert(!m_Interface);
	m_Interface = new VirtualHID(); //GameScreenInterface();
	m_Interface->bindObject(this);
	Ref<EventMapIndex> maps = CSPSim::theSim->getInterfaceMaps();
	if (maps.valid()) {
		Ref<EventMapping> map = maps->getMap("__gamescreen__");
		if (map != NULL) {
			m_Interface->setMapping(map);
		} else {
			CSPLOG(ERROR, APP) << "HID interface map '__gamescreen__' not found.";
		}
	} else {
		CSPLOG(ERROR, APP) << "No HID interface maps defined, '__gamescreen__' not found.";
	}
}

#if 0
// XXX: Preparing for the jump to OpenProducer ...
class SnapImageDrawCallback: public Producer::Camera::Callback {
	std::string m_Filename, m_Ext, m_Directory;
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
	SnapImageDrawCallback(const std::string& filename = "CSP",const std::string& ext = ".jpg"):
        m_Filename(filename),
		m_Ext(ext),
		m_Directory(g_Config.getString("Paths", "Screenshots", "../Screenshots", true)),
        m_SnapImageOnNextFrame(false){
			if (!osgDB::makeDirectory(m_Directory))
				std::cerr << "Warning: can't create target: " << m_Directory << "; no snapshot will be saved." << std::endl;
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

			// save the file in the form CSPmmddyy-hhmmss.ext
			osgDB::writeImageFile(*image, ospath::join(m_Directory, m_Filename + getDate() + m_Ext));
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
#endif

GameScreen::GameScreen():
	BaseScreen(),
	m_ActiveObject(0),
	m_CameraAgent(new CameraAgent(ViewFactory())),
	m_CameraCommands(new CameraCommands),
	m_OnPlayerJoin(this, &GameScreen::onPlayerJoin),
	m_OnPlayerQuit(this, &GameScreen::onPlayerQuit)
{
	initInterface();
}

GameScreen::~GameScreen() {
}

void GameScreen::onInit() {
	// add a layer for overlay text on screen
	const int ScreenWidth = CSPSim::theSim->getSDLScreen()->w;
	const int ScreenHeight = CSPSim::theSim->getSDLScreen()->h;

	m_ScreenInfoManager = new ScreenInfoManager(ScreenWidth, ScreenHeight);
	m_ScreenInfoManager->setName("ScreenInfoManager");
	m_ScreenInfoManager->setStatus("RECORD", false);

	osg::Group *info = CSPSim::theSim->getScene()->getInfoGroup();
	info->removeChild(0, info->getNumChildren());
	info->addChild(m_ScreenInfoManager.get());

	Ref<DynamicObject> ao = CSPSim::theSim->getActiveObject();
	if (ao.valid()) {
		setActiveObject(ao);
	}

	LocalBattlefield *bf = CSPSim::theSim->getBattlefield();
	if (bf) {
		bf->registerPlayerJoinCallback(m_OnPlayerJoin);
		bf->registerPlayerQuitCallback(m_OnPlayerQuit);
	}
}

void GameScreen::onPlayerJoin(int, const std::string& name) {
	m_ScreenInfoManager->addMessage(name + " has joined the game");
}

void GameScreen::onPlayerQuit(int, const std::string& name) {
	m_ScreenInfoManager->addMessage(name + " has left the game");
}


void GameScreen::onExit() {
}

void GameScreen::setActiveObject(Ref<DynamicObject> const &object) {
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
}

void GameScreen::onUpdate(double dt) {
	setCamera(dt);
	if (m_DataRecorder.valid()) {
		m_DataRecorder->timeStamp(dt);
	}
}

wf::WindowManager* GameScreen::getWindowManager() {
	return CSPSim::theSim->getScene()->getWindowManager();
}

wf::InputInterfaceManager* GameScreen::getInputInterfaceManager() {
	if(m_InputInterfaceManager.valid()) {
		return m_InputInterfaceManager.get();
	}
	
	m_InputInterfaceManager = new wf::InputInterfaceManager(this);
	return m_InputInterfaceManager.get(); 
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

void GameScreen::on_ChangeVehicle() {
	LocalBattlefield *battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		Ref<DynamicObject> object;
		// for now, allow ai-controlled vehicles to be selected as the
		// active object to help diagose the ai logic.
		object = battlefield->getNextUnit(m_ActiveObject, -1, 1, 0);
		if (object.valid()) CSPSim::theSim->setActiveObject(object);
	}
}

void GameScreen::on_Select() {
	m_CameraAgent->select();
}

void GameScreen::on_Reselect() {
	m_CameraAgent->reselect();
}

void GameScreen::on_Deselect() {
	m_CameraAgent->deselect();
}

void GameScreen::on_LookForward() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->LookForward);
}

void GameScreen::on_LookBackward() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->LookBackward);
}

void GameScreen::on_LookRight() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->LookRight);
}

void GameScreen::on_LookLeft() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->LookLeft);
}

void GameScreen::on_ViewPanLeft() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanLeft);
}

void GameScreen::on_ViewPanRight() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanRight);
}

void GameScreen::on_ViewPanLeftStop() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanLeftRightStop);
}

void GameScreen::on_ViewPanRightStop() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanLeftRightStop);
}

void GameScreen::on_ViewPanUpStop() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanUpDownStop);
}

void GameScreen::on_ViewPanDownStop() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanUpDownStop);
}

void GameScreen::on_ViewZoomStop() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomStop);
}

void GameScreen::on_ViewPanUp() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanUp);
}

void GameScreen::on_ViewPanDown() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->PanDown);
}

void GameScreen::on_ViewZoomIn() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomIn);
}

void GameScreen::on_ViewZoomOut() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomOut);
}

void GameScreen::on_ViewZoomStepIn() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomStepIn);
}

void GameScreen::on_ViewZoomStepOut() {
	m_CameraAgent->setCameraCommand(&m_CameraCommands->ZoomStepOut);
}

void GameScreen::on_ViewFovStepDec() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		float fov = scene->getViewAngle() * 0.8;
		if (fov > 10.0) {
			scene->setViewAngle(fov);
		}
	}
}

void GameScreen::on_ViewFovStepInc() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		float fov = scene->getViewAngle() * 1.20;
		if (fov < 90.0) {
			scene->setViewAngle(fov);
		}
	}
}

void GameScreen::on_SpinTheWorld() {
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

void GameScreen::on_LabelsToggle() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) scene->setLabels(!scene->getLabels());
}

void GameScreen::on_LabelsOn() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) scene->setLabels(true);
}

void GameScreen::on_LabelsOff() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) scene->setLabels(false);
}

void GameScreen::on_LeftClick(MapEvent::ClickEvent const &event) {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene && !event.drag) {
		scene->pick(event.x, event.y);
	}
}

void GameScreen::on_MouseView(MapEvent::MotionEvent const &event) {
	m_CameraCommands->Mouse.set(event.x, event.y, event.dx, event.dy);
	m_CameraAgent->setCameraCommand(&m_CameraCommands->Mouse);
}

void GameScreen::setCamera(double dt) {
	VirtualScene* scene = CSPSim::theSim->getScene();
	if (scene) {
		TerrainObject const *terrain = scene->getTerrain().get();
		m_CameraAgent->setCameraParameters(scene->getViewAngle(), scene->getNearPlane(), scene->getAspect());
		m_CameraAgent->updateCamera(dt, terrain);
	}
	LocalBattlefield* battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		battlefield->setCamera(m_CameraAgent->getEyePoint(), m_CameraAgent->getLookPoint(), m_CameraAgent->getUpVector());
	}
}

CSP_NAMESPACE_END

