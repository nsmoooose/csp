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
#include <csp/cspsim/input/EventMapIndex.h>
#include <csp/cspsim/glDiagnostics.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/ScreenInfoManager.h>
#include <csp/cspsim/ScreenInfoNode.h>
#include <csp/cspsim/VirtualScene.h>
#include <csp/cspsim/battlefield/LocalBattlefield.h>
#include <csp/cspsim/views/CameraAgent.h>
#include <csp/cspsim/views/CameraCommand.h>
#include <csp/cspsim/views/View.h>
#include <csp/cspsim/weather/clouds/Cloud.h>
#include <csp/cspsim/weather/clouds/CloudRegistry.h>
#include <csp/cspsim/weather/clouds/CloudUtils.h>

#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Log.h>

#include <csp/cspwf/WindowManager.h>

#include <osg/Image>
#include <osg/MatrixTransform>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgText/Text>

#include <ctime>
#include <iomanip>

namespace csp {

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
	m_Interface = new input::VirtualHID(); 
	m_Interface->bindObject(this);
	Ref<input::EventMapIndex> maps = CSPSim::theSim->getInterfaceMaps();
	if (maps.valid()) {
		Ref<input::EventMapping> map = maps->getMap("__gamescreen__");
		if (map != NULL) {
			m_Interface->setMapping(map);
		} else {
			CSPLOG(ERROR, APP) << "HID interface map '__gamescreen__' not found.";
		}
	} else {
		CSPLOG(ERROR, APP) << "No HID interface maps defined, '__gamescreen__' not found.";
	}
}

GameScreen::GameScreen(int screenWidth, int screenHeight):
	BaseScreen(),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight),
	m_ActiveObject(0),
	m_CameraAgent(new CameraAgent(ViewFactory())),
	m_CameraCommands(new CameraCommands)
{
	m_OnPlayerJoin.init(this, &GameScreen::onPlayerJoin);
	m_OnPlayerQuit.init(this, &GameScreen::onPlayerQuit);
	initInterface();	
}

GameScreen::~GameScreen() {
}

void GameScreen::onInit() {
	// add a layer for overlay text on screen
	CSPLOG(DEBUG, APP) << "Inside GameScreen::onInit()";

	CSPLOG(DEBUG, APP) << "creating ScreenInfoManager";
	m_ScreenInfoManager = new ScreenInfoManager(m_screenWidth, m_screenHeight);
	m_ScreenInfoManager->setName("ScreenInfoManager");
	m_ScreenInfoManager->setStatus("RECORD", false);
	
	// Set general stats and object statistics to not visisble as default.
	m_ScreenInfoManager->setStatus("GENERAL STATS", false);
	m_ScreenInfoManager->setStatus("OBJECT STATS", false);

	CSPLOG(DEBUG, APP) << "attach ScreenInfoManager to scene";
	osg::Group *info = ScreenInfoNode::getGroup(CSPSim::theSim->getSceneData());
	info->removeChild(0, info->getNumChildren());
	info->addChild(m_ScreenInfoManager.get());

	CSPLOG(DEBUG, APP) << "trying to set ActiveObject";
	Ref<DynamicObject> ao = CSPSim::theSim->getActiveObject();
	if (ao.valid()) {
		setActiveObject(ao);
		CSPLOG(DEBUG, APP) << "successfully set ActiveObject";
	}

	CSPLOG(DEBUG, APP) << "trying to get Battlefield";
	LocalBattlefield *bf = CSPSim::theSim->getBattlefield();
	if (bf) {
		bf->registerPlayerJoinCallback(m_OnPlayerJoin);
		bf->registerPlayerQuitCallback(m_OnPlayerQuit);
		CSPLOG(DEBUG, APP) << "Battlefield available";
	}
}

void GameScreen::onPlayerJoin(int, const std::string& name) {
	m_ScreenInfoManager->addMessage(name + " has joined the game");
}

void GameScreen::onPlayerQuit(int, const std::string& name) {
	m_ScreenInfoManager->addMessage(name + " has left the game");
}


void GameScreen::onExit() {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) scene->hideScene();
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
		m_ScreenInfoManager->changeObjectStats(m_screenWidth, m_screenHeight, object);
	}
}

void GameScreen::onUpdate(double dt) {
	setCamera(dt);
	if (m_DataRecorder.valid()) {
		m_DataRecorder->timeStamp(dt);
	}
}

input::InputInterfaceWfAdapter* GameScreen::getInputInterfaceWfAdapter() {
	if (m_InputInterfaceWfAdapter.valid()) {
		return m_InputInterfaceWfAdapter.get();
	}

	m_InputInterfaceWfAdapter = new input::InputInterfaceWfAdapter(this);
	return m_InputInterfaceWfAdapter.get();
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

void GameScreen::on_ToggleClouds() {
	osg::ref_ptr<osg::MatrixTransform> model = new osg::MatrixTransform();
	osg::Matrix move = osg::Matrix::translate(osg::Vec3(-29495, -10530, 301.3));
	model->setMatrix(move);
	weather::clouds::CloudRegistry::CloudVector clouds = weather::clouds::CloudRegistry::CreateDefaultClouds();
	weather::clouds::CloudUtils::addRandomClouds(model.get(), clouds, osg::Vec3(2000, 2000, 20), 40);
	CSPSim::theSim->getScene()->getCloudGroup()->addChild(model.get());
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

void GameScreen::on_LeftClick(input::MapEvent::ClickEvent const &event) {
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene && !event.drag) {
		scene->pick(event.x, event.y);
	}
}

void GameScreen::on_MouseView(input::MapEvent::MotionEvent const &event) {
	m_CameraCommands->Mouse.set(event.x, event.y, event.dx, event.dy);
	m_CameraAgent->setCameraCommand(&m_CameraCommands->Mouse);
}

void GameScreen::on_ViewPanLeftRight(input::MapEvent::AxisEvent const &event) {
	m_CameraCommands->LeftRightAxis.set(event.value);
	m_CameraAgent->setCameraCommand(&m_CameraCommands->LeftRightAxis);
}

void GameScreen::on_ViewPanUpDown(input::MapEvent::AxisEvent const &event) {
	m_CameraCommands->UpDownAxis.set(event.value);
	m_CameraAgent->setCameraCommand(&m_CameraCommands->UpDownAxis);
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

bool GameScreen::onMouseButton(input::RawEvent::MouseButton const &event) {
	Ref<wf::WindowManager> windowManager = CSPSim::theSim->getWindowManager();
	if(event.type == input::RawEvent::MouseButton::PRESSED) {
		return windowManager->onMouseDown(event.x, event.y, event.button);
	}
	else if(event.type == input::RawEvent::MouseButton::RELEASED) {
		return windowManager->onMouseUp(event.x, event.y, event.button);
	}
	return false;
}

bool GameScreen::onMouseMove(input::RawEvent::MouseMotion const &event) {
	// Let the window manager process the input interface events.
	return CSPSim::theSim->getWindowManager()->onMouseMove(event.x, event.y);
}

} // namespace csp

