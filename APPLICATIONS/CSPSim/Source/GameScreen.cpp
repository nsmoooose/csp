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



double const GameScreen::OffsetRate =  30.0 * (G_PI / 180.0); // x deg / s, even if it is used for Zoom also ...

simdata::Vector3 m_normalDirection;  

void GameScreen::normalView()
{
	m_fangleRotX = 0.0;
	m_fangleRotZ = 0.0;
	m_PanRateX = 0.0;
	m_PanRateZ = 0.0;
	m_ZoomRate = 0.0;
	m_fdisToObject = 30.0;
}


void GameScreen::initInterface()
{
	assert(!m_Interface);
	m_Interface = new VirtualHID(); //GameScreenInterface();
	m_Interface->bindObject(this);
	BIND_ACTION("QUIT", on_Quit);
	BIND_ACTION("PAUSE", on_Pause);
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
	CSP_LOG( APP , DEBUG, "turnViewAboutZ: 	m_fangleViewZ = " << m_fangleRotZ 
		<< "; m_PanRateZ = " << m_PanRateZ);
	
	m_fangleRotZ += m_PanRateZ * dt;
	/*if ( m_fangleRotZ > fangleMax )
		if ( fangleMax == pi )
			m_fangleRotZ -= 2 * pi;
		else
            m_fangleRotZ = fangleMax;
	if ( m_fangleRotZ < - fangleMax )
		if ( fangleMax == pi )
			m_fangleRotZ += 2 * pi;
		else
			m_fangleRotZ = - fangleMax;*/
}

void GameScreen::turnViewAboutX(double dt, double fangleMax)
{
	CSP_LOG( APP , DEBUG, "turnViewAboutX: 	m_fangleViewX = " << m_fangleRotX 
		     << "; m_PanRateX = " << m_PanRateX);

	m_fangleRotX += m_PanRateX * dt;
	/*if ( m_fangleRotX > fangleMax )
		m_fangleRotX = fangleMax;
	if ( m_fangleRotX < - fangleMax )
		m_fangleRotX = - fangleMax;*/
}

void GameScreen::scaleView(double dt)
{
	double ScaleFactor = 1.0 + m_ZoomRate * dt;
	if ( (m_fdisToObject > 2.0 && ScaleFactor < 1.0) ||
	     (m_fdisToObject < 2000.0 && ScaleFactor > 1.0) ) {
		m_fdisToObject *= ScaleFactor;
	}
	if (m_fdisToObject < 2.0) {
		m_fdisToObject = 2.0;
	} else
	if (m_fdisToObject > 2000.0) {
		m_fdisToObject = 2000.0;
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
	m_iViewMode = 1;
	m_bInternalView = true;
	m_bPreviousState = false;
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

	m_InfoView->getRenderStage()->setClearMask(0x0);

	m_ScreenInfoManager = new ScreenInfoManager(ScreenWidth,ScreenHeight);
	m_ScreenInfoManager->setName("ScreenInfoManager");
	m_ScreenInfoManager->setStatus("PAUSE", CSPSim::theSim->isPaused());

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
	m_ActiveObject = object;
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

void GameScreen::onRender()
{
	if (m_ActiveObject.valid()) {
		if (m_bPreviousState^m_bInternalView)
		{
			if (m_bInternalView )
			{
				// Hide the object if internal view
				// Draw the hud if internal view
				//m_ActiveObject->hideModel();
			}
			else
			{
				m_ActiveObject->showModel();
			}
			m_bPreviousState = m_bInternalView;
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

void GameScreen::onUpdate(double dt)
{	
	setCamera(dt);
	m_InfoView->update();
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
		h = battlefield->getElevation(objectPos.x, objectPos.y);
	}
	if ( camPos.z < h ) camPos.z = h;
	return camPos;
}

void GameScreen::on_View1()
{
	m_iViewMode = 1;
	normalView();
	m_bInternalView = true;
}

void GameScreen::on_View2()
{
	m_iViewMode = 2;
	normalView();
	m_bInternalView = false;
}


void GameScreen::on_View3()
{
	if (m_ActiveObject.valid()) {
		m_iViewMode = 3;
		m_normalDirection = m_ActiveObject->getDirection();
		m_bInternalView = false;
	}
}

void GameScreen::on_View4()
{
	m_iViewMode = 4;
	m_bInternalView = false;
}

void GameScreen::on_View5()
{
	m_iViewMode = 5;
	m_bInternalView = false;
}

void GameScreen::on_View6()
{
	m_iViewMode = 6;
	m_bInternalView = false;
}

void GameScreen::on_View7()
{
	m_iViewMode = 7;
	m_bInternalView = false;
}

void GameScreen::on_View8()
{
	if (m_ActiveObject.valid()) {
		m_iViewMode = 8;
		m_fixedCamPos = getNewFixedCamPos(m_ActiveObject.get());
		m_bInternalView = false;
	}
}

void GameScreen::on_View9()
{
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
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_PanRateZ = - OffsetRate;
}

void GameScreen::on_ViewPanRight()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_PanRateZ = + OffsetRate;
}

void GameScreen::on_ViewPanLeftStop() 
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_PanRateZ < 0.0) m_PanRateZ = 0.0;
}

void GameScreen::on_ViewPanRightStop() 
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_PanRateZ > 0.0) m_PanRateZ = 0.0;
}

void GameScreen::on_ViewPanUpStop()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_PanRateX < 0.0) m_PanRateX = 0.0;
}

void GameScreen::on_ViewPanDownStop()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_PanRateX > 0.0) m_PanRateX = 0.0;
}

void GameScreen::on_ViewZoomStop()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_ZoomRate = 0.0;
}

void GameScreen::on_ViewPanUp()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_PanRateX = - OffsetRate;
}

void GameScreen::on_ViewPanDown() 
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_PanRateX = OffsetRate;
}

void GameScreen::on_ViewZoomIn() 
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_ZoomRate = - OffsetRate;
}

void GameScreen::on_ViewZoomOut()
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_ZoomRate = OffsetRate;
}

void GameScreen::on_ViewZoomStepIn() 
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fdisToObject *= 0.8  ;
}

void GameScreen::on_ViewZoomStepOut()
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fdisToObject *= 1.20;
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
	m_fangleRotZ += dx * 0.001;
	m_fangleRotX += dy * 0.001;
}


// TODO All references to planes should be made generic!

void GameScreen::setCamera(double dt)
{	
	simdata::Vector3 eyePos;
	simdata::Vector3 lookPos;
	simdata::Vector3 upVec;

	if (!m_ActiveObject) {
		// temporary view if there are no active objects... probably
		// should switch to an "action" view mode in this case.
		turnViewAboutX(dt);
		turnViewAboutZ(dt);
		eyePos = simdata::Vector3(483000.0, 499000.0, 2000.0);
		//lookPos = simdata::Vector3(483000.0, 499005.0, 2000.0);
		upVec = simdata::Vector3::ZAXIS;
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(upVec,  - m_fangleRotZ);
		RotX.FromAxisAngle(RotZ * simdata::Vector3::XAXIS, - m_fangleRotX);
		simdata::Matrix3 R;
		R = RotX * RotZ;
		lookPos = RotX * RotZ * simdata::Vector3::YAXIS + eyePos;
		CSP_LOG( APP , DEBUG, "setCamera: " << R.asString() << " : " << (simdata::Vector3::XAXIS).asString());
	}
	else switch ( m_iViewMode )
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
		RotZ.FromAxisAngle(planeUp,  - m_fangleRotZ);
		RotX.FromAxisAngle(RotZ * planeRight, - m_fangleRotX);
		eyePos = planePos;
		eyePos += m_ActiveObject->getViewPoint();
		lookPos = 50.0 * RotX * RotZ * planeDir + eyePos;
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
		RotZ.FromAxisAngle(planeUp,  m_fangleRotZ);
		simdata::Vector3 axisX = RotZ * planeRight;
		RotX.FromAxisAngle(axisX, m_fangleRotX);
		eyePos = - m_fdisToObject * RotX * RotZ * planeDir;
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
		RotZ.FromAxisAngle(simdata::Vector3::ZAXIS,  m_fangleRotZ);
		simdata::Vector3 axisX = - RotZ * m_normalDirection;
		RotX.FromAxisAngle(simdata::Vector3::ZAXIS^axisX, m_fangleRotX);
		eyePos = - m_fdisToObject * RotX * RotZ * m_normalDirection;
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
		if ((lookPos - m_fixedCamPos).Length() > 900.0 )
			m_fixedCamPos = getNewFixedCamPos(m_ActiveObject.get());
		eyePos = m_fixedCamPos;
		upVec = simdata::Vector3::ZAXIS;
		break;
	}    
	VirtualScene *scene = CSPSim::theSim->getScene();
	if (scene) {	
		scene->setLookAt(eyePos, lookPos, upVec);
		VirtualBattlefield *battlefield = CSPSim::theSim->getBattlefield();
		if (battlefield) {
			battlefield->updateOrigin(eyePos);
			battlefield->setCamera(eyePos);
		}
	}
}

