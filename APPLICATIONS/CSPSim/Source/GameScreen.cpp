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
#include "CSPSim.h"
#include "EventMapIndex.h"
#include "Log.h"
#include "VirtualBattlefield.h"
#include "VirtualScene.h"
#include "CSPSim.h"
#include "ConsoleCommands.h"

#include <SimData/Math.h>
#include <SimData/Types.h>


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

double const GameScreen::OffsetRate =  30.0 * (G_PI / 180.0); // x deg / s, even if it is used for Zoom also ...


void GameScreen::normalView()
{
	m_AngleRotX = 0.0;
	m_AngleRotZ = 0.0;
	m_NeckPhi = 0.0;
	m_NeckTheta = 0.5*G_PI;
	m_PanRateX = 0.0;
	m_PanRateZ = 0.0;
	m_ZoomRate = 0.0;
	m_DistanceToObject = 30.0;
	m_CameraOnGround = false;
	m_LookRelative = false;
	m_CameraHint = 0;
}


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
	BIND_ACTION("RESET_SPIN", on_ResetSpin);
	BIND_MOTION("CAMERA_PAN", on_MouseView);
	EventMapIndex *maps = CSPSim::theSim->getInterfaceMaps();
	if (maps) {
		EventMapping *map = maps->getMap("__gamescreen__");
		if (map != NULL) {
			m_Interface->setMapping(map);
		} else {
			CSP_LOG( APP , ERROR, "HID interface map '__gamescreen__' not found.");
		}
	} else {
		CSP_LOG( APP , ERROR, "No HID interface maps defined, '__gamescreen__' not found.");
	}
}



void GameScreen::turnViewAboutZ(double dt, double fangleMax)
{
	CSP_LOG( APP , DEBUG, "turnViewAboutZ: 	m_AngleViewZ = " << m_AngleRotZ 
		<< "; m_PanRateZ = " << m_PanRateZ);
	
	m_AngleRotZ += m_PanRateZ * dt;
	/*if ( m_AngleRotZ > fangleMax )
		if ( fangleMax == pi )
			m_AngleRotZ -= 2 * pi;
		else
            m_AngleRotZ = fangleMax;
	if ( m_AngleRotZ < - fangleMax )
		if ( fangleMax == pi )
			m_AngleRotZ += 2 * pi;
		else
			m_AngleRotZ = - fangleMax;*/
}

void GameScreen::turnViewAboutX(double dt, double fangleMax)
{
	CSP_LOG( APP , DEBUG, "turnViewAboutX: 	m_AngleViewX = " << m_AngleRotX 
		     << "; m_PanRateX = " << m_PanRateX);

	m_AngleRotX += m_PanRateX * dt;
	if (m_AngleRotX > 1.57) {
		m_AngleRotX = 1.57;
	}
	if (m_AngleRotX < -1.57) {
		m_AngleRotX = -1.57;
	}
	/*if ( m_AngleRotX > fangleMax )
		m_AngleRotX = fangleMax;
	if ( m_AngleRotX < - fangleMax )
		m_AngleRotX = - fangleMax;*/
}

void GameScreen::scaleView(double dt)
{
	double ScaleFactor = 1.0 + m_ZoomRate * dt;
	if ( (m_DistanceToObject > m_MinimumDistance && ScaleFactor < 1.0) ||
	     (m_DistanceToObject < 2000.0 && ScaleFactor > 1.0) ) {
		m_DistanceToObject *= ScaleFactor;
	}
	if (m_DistanceToObject < m_MinimumDistance) {
		m_DistanceToObject = m_MinimumDistance;
	} else
	if (m_DistanceToObject > 2000.0) {
		m_DistanceToObject = 2000.0;
	} 
}

GameScreen::GameScreen(): BaseScreen() {
	m_ActiveObject = NULL;
	initInterface();
}
	
GameScreen::~GameScreen() {
	if (m_Interface) delete m_Interface;
}

void GameScreen::onInit()
{
	m_ViewMode = 1;
	m_InternalView = true;
	m_PreviousState = false;
	normalView();
	if (m_ActiveObject.valid()) {
		m_ActiveObject->showModel();
	}

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

void GameScreen::onExit()
{

}

void GameScreen::setActiveObject(simdata::Ref<DynamicObject> const &object) 
{
	if (m_ActiveObject.valid() && m_ActiveObject != object) {
		VirtualScene *scene = CSPSim::theSim->getScene();
		if (scene && m_ActiveObject->getNearFlag()) {
			scene->setNearObject(m_ActiveObject, false);
		}
		setRecorder(false);
	}
	m_ActiveObject = object;
	m_Padlock = object;
	if (object.valid() && object->isHuman() && object->isLocal()) {
		on_View1();
	} else {
		on_View2();
	}
	if (object.valid()) {
		int ScreenWidth = CSPSim::theSim->getSDLScreen()->w;
		int ScreenHeight = CSPSim::theSim->getSDLScreen()->h;
		m_ScreenInfoManager->changeObjectStats(ScreenWidth,ScreenHeight,object);
		m_MinimumDistance = object->getModel()->getBoundingSphereRadius() + 2.0;
	} else {
		m_MinimumDistance = 10.0;
	}
}

void GameScreen::onRender()
{
	if (m_ActiveObject.valid()) {
		if (m_PreviousState^m_InternalView)
		{
			if (m_InternalView )
			{
				// Hide the object if internal view
				// Draw the hud if internal view
				//m_ActiveObject->hideModel(); 
			}
			else
			{
				m_ActiveObject->showModel();
			}
			m_PreviousState = m_InternalView;
		}
	}
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

simdata::Vector3 GameScreen::getNewFixedCamPos(SimObject * const target) const
{
	simdata::Vector3 camPos;
	simdata::Vector3 objectPos = target->getGlobalPosition();
	DynamicObject *dynamic = dynamic_cast<DynamicObject *>(target);
	if (dynamic) {
		simdata::Vector3 upVec = dynamic->getUpDirection();
		simdata::Vector3 objectDir = dynamic->getDirection();
		camPos = objectPos + 500.0 * objectDir + ( 12.0 - (rand() % 5) ) * (objectDir^upVec) 
			    + ( 6.0 + (rand () % 3) ) * upVec;
	} else {
		camPos = objectPos + 100.0 * simdata::Vector3::ZAXIS + 100.0 * simdata::Vector3::XAXIS;
	}
	float h = 0.0;
	VirtualBattlefield *battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		h = battlefield->getGroundElevation(objectPos.x, objectPos.y);
	}
	if ( camPos.z < h ) camPos.z = h;
	return camPos;
}

void GameScreen::on_View1()
{
	m_ViewMode = 1;
	normalView();
	m_InternalView = true;
	m_LookRelative = false;
	if (m_ViewMode == 9) {
		m_AngleRotX = 0.5*G_PI-m_NeckTheta;
		m_AngleRotZ = m_NeckPhi;
	}
}

void GameScreen::on_View2()
{
	m_ViewMode = 2;
	normalView();
	m_InternalView = false;
	m_LookRelative = true;
}


void GameScreen::on_View3()
{
	if (m_ActiveObject.valid()) {
		m_ViewMode = 3;
		m_NormalDirection = m_ActiveObject->getDirection();
		m_InternalView = false;
		m_LookRelative = true;
	}
}

void GameScreen::on_View4()
{
	m_ViewMode = 4;
	m_InternalView = false;
	m_LookRelative = false;
}

void GameScreen::on_View5()
{
	m_ViewMode = 5;
	m_InternalView = false;
	m_LookRelative = false;
}

void GameScreen::on_View6()
{
	m_ViewMode = 6;
	m_InternalView = false;
	m_LookRelative = false;
}

void GameScreen::on_View7()
{
	m_ViewMode = 7;
	m_InternalView = false;
	m_LookRelative = false;
}

void GameScreen::on_View8()
{
	if (m_ActiveObject.valid()) {
		m_ViewMode = 8;
		m_FixedCameraPosition = getNewFixedCamPos(m_ActiveObject.get());
		m_InternalView = false;
		m_LookRelative = false;
	}
}

// padlock testing
void GameScreen::on_View9()
{
	if ((m_ViewMode == 1 || m_ViewMode == 9) && m_ActiveObject.valid()) {
		simdata::Ref<DynamicObject> object;
		VirtualBattlefield *battlefield = CSPSim::theSim->getBattlefield();
		m_Padlock = battlefield->getNextUnit(m_Padlock, -1, -1, -1);
		if (m_Padlock == m_ActiveObject) {
			m_Padlock = battlefield->getNextUnit(m_Padlock, -1, -1, -1);
		}
		if (m_Padlock != m_ActiveObject) {
			if (m_ViewMode == 1) {
				m_NeckTheta = m_AngleRotX-0.5*G_PI;
				m_NeckPhi = m_AngleRotZ;
			}
			m_ViewMode = 9;
			m_InternalView = true;
			m_LookRelative = false;
		}
	}
}

void GameScreen::on_View0()
{
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

void GameScreen::on_Stats()
{
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

void GameScreen::on_ViewPanLeft()
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_PanRateZ = - OffsetRate;
}

void GameScreen::on_ViewPanRight()
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_PanRateZ = + OffsetRate;
}

void GameScreen::on_ViewPanLeftStop() 
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		if (m_PanRateZ < 0.0) m_PanRateZ = 0.0;
}

void GameScreen::on_ViewPanRightStop() 
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		if (m_PanRateZ > 0.0) m_PanRateZ = 0.0;
}

void GameScreen::on_ViewPanUpStop()
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		if (m_PanRateX < 0.0) m_PanRateX = 0.0;
}

void GameScreen::on_ViewPanDownStop()
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		if (m_PanRateX > 0.0) m_PanRateX = 0.0;
}

void GameScreen::on_ViewZoomStop()
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_ZoomRate = 0.0;
}

void GameScreen::on_ViewPanUp()
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_PanRateX = - OffsetRate;
}

void GameScreen::on_ViewPanDown() 
{
	if ( m_ViewMode == 1 || m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_PanRateX = OffsetRate;
}

void GameScreen::on_ViewZoomIn() 
{
	if ( m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_ZoomRate = - OffsetRate;
}

void GameScreen::on_ViewZoomOut()
{
	if ( m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_ZoomRate = OffsetRate;
}

void GameScreen::on_ViewZoomStepIn() 
{
	if ( m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_DistanceToObject *= 0.8  ;
}

void GameScreen::on_ViewZoomStepOut()
{
	if ( m_ViewMode == 2 || m_ViewMode == 3 ) 
		m_DistanceToObject *= 1.20;
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

void GameScreen::on_SpinTheWorldStop()
{
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->spinTheWorld(false);
	}
}

void GameScreen::on_ResetSpin()
{
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {
		scene->resetSpin();
	}
}

void GameScreen::on_MouseView(int x, int y, int dx, int dy)
{
	m_PanRateX = 0.0;
	m_PanRateZ = 0.0;
	m_AngleRotZ += dx * 0.001;
	m_AngleRotX += dy * 0.001;
	if (m_AngleRotX > 1.57) {
		m_AngleRotX = 1.57;
	}
	if (m_AngleRotX < -1.57) {
		m_AngleRotX = -1.57;
	}
}


void GameScreen::setCamera(double dt)
{	
	simdata::Vector3 eyePos;
	simdata::Vector3 lookPos;
	simdata::Vector3 upVec;
	VirtualScene *scene = CSPSim::theSim->getScene();

	if ((scene != 0) && m_ActiveObject.valid()) {
		bool isNear = m_ActiveObject->getNearFlag();
		if (isNear && m_ViewMode != 1 && m_ViewMode != 9) {
			scene->setNearObject(m_ActiveObject, false);
		} else
		if (!isNear && (m_ViewMode == 1 || m_ViewMode == 9)) {
			scene->setNearObject(m_ActiveObject, true);		
		}
	}

	if (!m_ActiveObject) {
		// temporary view if there are no active objects... probably
		// should switch to an "action" view mode in this case.
		turnViewAboutX(dt);
		turnViewAboutZ(dt);
		eyePos = simdata::Vector3(483000.0, 499000.0, 2000.0);
		//lookPos = simdata::Vector3(483000.0, 499005.0, 2000.0);
		upVec = simdata::Vector3::ZAXIS;
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(upVec,  - m_AngleRotZ);
		RotX.FromAxisAngle(RotZ * simdata::Vector3::XAXIS, - m_AngleRotX);
		simdata::Matrix3 R;
		R = RotX * RotZ;
		lookPos = RotX * RotZ * simdata::Vector3::YAXIS + eyePos;
		CSP_LOG( APP , DEBUG, "setCamera: " << R.asString() << " : " << (simdata::Vector3::XAXIS).asString());
	}
	else switch ( m_ViewMode )
	{
	case 1: // view_mode one is normal inside the cockpit view
	{
		turnViewAboutX(dt,G_PI / 3);
		turnViewAboutZ(dt,G_PI / 3);
		simdata::Vector3 planePos = m_ActiveObject->getGlobalPosition();
		simdata::Vector3 planeDir = m_ActiveObject->getDirection();
		simdata::Vector3 planeUp = m_ActiveObject->getUpDirection();
		simdata::Vector3 planeRight = planeDir^planeUp;
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(planeUp,  - m_AngleRotZ);
		RotX.FromAxisAngle(RotZ * planeRight, - m_AngleRotX);
		eyePos = planePos;
		eyePos += m_ActiveObject->getViewPoint();
		lookPos = 1000 * RotX * RotZ * planeDir + eyePos;
		upVec = planeUp;
		break;
	}
	case 2: // view mode two is external view; normal view is behind the plane
	{ 	
		turnViewAboutX(dt);
		turnViewAboutZ(dt);
		scaleView(dt);
		simdata::Vector3 planePos = m_ActiveObject->getGlobalPosition();
		simdata::Vector3 planeDir = m_ActiveObject->getDirection();
		simdata::Vector3 planeUp = m_ActiveObject->getUpDirection();
		simdata::Vector3 planeRight = planeDir^planeUp;
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(planeUp,  m_AngleRotZ);
		simdata::Vector3 axisX = RotZ * planeRight;
		RotX.FromAxisAngle(axisX, m_AngleRotX);
		eyePos = - m_DistanceToObject * RotX * RotZ * planeDir;
		upVec = eyePos^axisX;
		eyePos += planePos;
		lookPos = planePos;
		break;
	}
	case 3: // view mode three is external fixed view around the plane
	{
		turnViewAboutX(dt);
		turnViewAboutZ(dt);
		scaleView(dt);
		simdata::Vector3 planePos = m_ActiveObject->getGlobalPosition();
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(simdata::Vector3::ZAXIS,  m_AngleRotZ);
		simdata::Vector3 axisX = - RotZ * m_NormalDirection;
		RotX.FromAxisAngle(simdata::Vector3::ZAXIS^axisX, m_AngleRotX);
		eyePos = m_DistanceToObject * RotX * axisX;
		upVec = simdata::Vector3::ZAXIS;
		eyePos += planePos;
		lookPos = planePos;
		break;
	}
	case 4: // view mode four is view looking straight up
		eyePos = m_ActiveObject->getGlobalPosition();
		lookPos = eyePos + 40 * m_ActiveObject->getUpDirection();
		upVec = simdata::Vector3::YAXIS;
		break;
	case 5: // view mode five is sat view
		lookPos = eyePos = m_ActiveObject->getGlobalPosition();
		eyePos.z += 1000;
		lookPos.z = 0;
		upVec = simdata::Vector3::XAXIS;
		break;
	case 6: // view mode six is 50 meters above plane
		eyePos = m_ActiveObject->getGlobalPosition() + 50 * simdata::Vector3::ZAXIS;
		lookPos = m_ActiveObject->getGlobalPosition();
		upVec = m_ActiveObject->getUpDirection();
		break;
	case 7:
		eyePos = m_ActiveObject->getGlobalPosition() + simdata::Vector3(0,15,0);
		lookPos = m_ActiveObject->getGlobalPosition() - simdata::Vector3(0,15, 0);
		upVec = simdata::Vector3(0,0,1);
		break;
	case 8: // view mode 8 is a fly by view 
		lookPos = m_ActiveObject->getGlobalPosition();
		if ((lookPos - m_FixedCameraPosition).Length() > 900.0 )
			m_FixedCameraPosition = getNewFixedCamPos(m_ActiveObject.get());
		eyePos = m_FixedCameraPosition;
		upVec = simdata::Vector3::ZAXIS;
		break;
	case 9: // view mode 8 is a fly by view 
	{
		static int xxx = 0;
		simdata::Vector3 planePos = m_ActiveObject->getGlobalPosition();
		simdata::Quaternion attitude = m_ActiveObject->getAttitude();
		eyePos = planePos;
		eyePos += m_ActiveObject->getViewPoint();
		lookPos = m_Padlock->getGlobalPosition();
		simdata::Vector3 dir = simdata::Normalized(lookPos - eyePos);
		attitude.InverseRotate(dir);
		float phi = atan2(-dir.x, dir.y);
		float theta = acos(dir.z);
		float phi_max = 2.6 - std::max(0.0, 1.57 - 2.0*theta);
		if (phi > phi_max) {
			phi = phi_max; 
			m_NeckLimit = true;
		} else 
		if (phi < -phi_max) {
			phi = -phi_max;
			m_NeckLimit = true;
		} else {
			m_NeckLimit = false;
		}
		float motion = std::min(3.0*dt, 0.3);
		phi = m_NeckPhi * (1.0-motion) + phi * motion;
		float psi = phi * std::max(0.0, std::min(1.0, 2.0*theta));
		float offset = std::max(0.0, std::min(0.4, 0.3*(fabs(psi) - 1.57)));
		if (++xxx % 20 == 0) {
			std::cout << phi << ":" << theta << " = " << offset << std::endl;
		}
		if (psi > 0.0) offset = -offset;
		m_NeckTheta = theta;
		m_NeckPhi = phi;
		dir.Set(-sin(theta)*sin(phi), sin(theta)*cos(phi), cos(theta));
		simdata::Vector3 d(sin(psi), -cos(psi), 0.0);
		upVec.Set(d.x*cos(theta), d.y*cos(theta), sin(theta));
		attitude.Rotate(upVec);
		eyePos += attitude.GetRotated(offset * simdata::Vector3::XAXIS);
		attitude.Rotate(dir);
		lookPos = eyePos + 100.0 * dir;
		break;
	}
	}    

	VirtualBattlefield *battlefield = CSPSim::theSim->getBattlefield();
	if (battlefield) {
		float h = 1.0 + battlefield->getGroundElevation(eyePos.x, eyePos.y, m_CameraHint);
		m_CameraOnGround = (eyePos.z <= h);
		if (m_CameraOnGround) {
			if (m_LookRelative) {
				simdata::Vector3 adjustment = eyePos - lookPos;
				double dh = h - lookPos.z;
				adjustment.z = 0.0;
				adjustment.Normalize();
				adjustment *= sqrt(m_DistanceToObject*m_DistanceToObject - dh*dh);
				adjustment.z = dh;
				eyePos = adjustment + lookPos;
				// view 2 would be a bit tricky to adjust so that any
				// upward camera motion would immediately bring the
				// camera off the ground... pass on that for now.
				//m_CameraGroundAngle = asin(dh, m_disToObject) - asin(dz, m_disToObject);
				//m_PanRateX = 0.0;
			} else {
				eyePos.z = h;
			}
		}
		battlefield->updateOrigin(eyePos); 
		battlefield->setCamera(eyePos);
	} else {
		m_CameraOnGround = false;
	}

	if (scene) {	
		scene->setLookAt(eyePos, lookPos, upVec);
	}
}

