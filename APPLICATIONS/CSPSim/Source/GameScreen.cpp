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


#include "stdinc.h"

#include <SimData/Types.h>
#include "GameScreen.h"
#include "EventMapIndex.h"
#include "VirtualBattlefield.h"
#include "CSPSim.h"



double const GameScreen::angleOffset = 0.02;

simdata::Vector3 m_normalDirection;  



void GameScreen::NormalView()
{
	m_fangleRotX = 0.0;
	m_fangleRotZ = 0.0;
	m_fangleOffsetX = 0.0;
	m_fangleOffsetZ = 0.0;
	m_fdisToObject = 30.0;
	m_fscaleFactor = 1.0;
}


void GameScreen::InitInterface()
{
	assert(!m_Interface);
	m_Interface = new VirtualHID(); //GameScreenInterface();
	m_Interface->bindObject(this);
	BIND_ACTION("QUIT", on_Quit);
	BIND_ACTION("PAUSE", on_Pause);
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
	BIND_MOTION("CAMERA_PAN", on_MouseView);
	EventMapIndex *maps = CSPSim::theSim->getInterfaceMaps();
	if (maps) {
		EventMapping *map = maps->getMap("__gamescreen__");
		if (map != NULL) {
			m_Interface->setMapping(map);
		} else {
			CSP_LOG( CSP_APP , CSP_ERROR, "HID interface map '__gamescreen__' not found.");
		}
	} else {
		CSP_LOG( CSP_APP , CSP_ERROR, "No HID interface maps defined, '__gamescreen__' not found.");
	}
}


void GameScreen::SetBattlefield(VirtualBattlefield *battlefield)
{
	m_Battlefield = battlefield;
}

void GameScreen::TurnViewAboutZ(double fangleMax)
{
	CSP_LOG( CSP_APP , CSP_DEBUG, "TurnViewAboutZ: 	m_fangleViewZ = " << m_fangleRotZ 
		<< "; m_fangleOffsetZ = " << m_fangleOffsetZ);
	
	m_fangleRotZ += m_fangleOffsetZ;
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

void GameScreen::TurnViewAboutX(double fangleMax)
{
	CSP_LOG( CSP_APP , CSP_DEBUG, "TurnViewAboutX: 	m_fangleViewX = " << m_fangleRotX 
		     << "; m_fangleOffsetX = " << m_fangleOffsetX);

	m_fangleRotX += m_fangleOffsetX;
	/*if ( m_fangleRotX > fangleMax )
		m_fangleRotX = fangleMax;
	if ( m_fangleRotX < - fangleMax )
		m_fangleRotX = - fangleMax;*/
}

void GameScreen::ScaleView()
{
	if ( (m_fdisToObject > 2.0 && m_fscaleFactor < 1.0) ||
	     (m_fdisToObject < 2000.0 && m_fscaleFactor > 1.0) ) {
		m_fdisToObject *= m_fscaleFactor;
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
	InitInterface();
}
	
GameScreen::~GameScreen() {
	if (m_Interface) delete m_Interface;
}

void GameScreen::OnInit()
{
	m_iViewMode = 1;
	m_bInternalView = true;
	m_bPreviousState = false;
	NormalView();
	if (m_ActiveObject.valid()) {
		m_ActiveObject->ShowRepresentant(1);
	}
}

void GameScreen::OnExit()
{

}

void GameScreen::setActiveObject(simdata::PointerBase &object) 
{
	m_ActiveObject = object;
	simdata::Pointer<DynamicObject> vehicle = object;
	if (vehicle.valid() && vehicle->isHuman() && vehicle->isLocal()) {
		on_View1();
	} else {
		on_View2();
	}
}

void GameScreen::OnRender()
{
	SetCamera();
	
	if (m_ActiveObject.valid()) {
		if (m_bPreviousState^m_bInternalView)
		{
			if (m_bInternalView )
			{
				// Hide the object if internal view
				// Draw the hud if internal view
				m_ActiveObject->ShowRepresentant(1);	
			}
			else
			{
				m_ActiveObject->ShowRepresentant(0);
			}
			m_bPreviousState = m_bInternalView;
		}
	}

	// Draw the whole scene
	if (m_Battlefield) {
		m_Battlefield->drawScene();
	}
}

void GameScreen::OnUpdateObjects(double dt)
{	
}

simdata::Vector3 GameScreen::GetNewFixedCamPos(SimObject * const target) const
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
	float h = 0;
	if (m_Battlefield) {
		m_Battlefield->getElevation(objectPos.x, objectPos.y);
	}
	if ( camPos.z < h ) camPos.z = h;
	return camPos;
}

void GameScreen::on_View1()
{
	m_iViewMode = 1;
	NormalView();
	m_bInternalView = true;
}

void GameScreen::on_View2()
{
	m_iViewMode = 2;
	NormalView();
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
		m_fixedCamPos = GetNewFixedCamPos(m_ActiveObject.ptr());
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
	CSPSim::theSim->Quit();
}

void GameScreen::on_Pause()
{
	CSPSim::theSim->togglePause();
}

void GameScreen::on_ChangeVehicle()
{
	simdata::Pointer<DynamicObject> object;
	object = m_Battlefield->getNextObject(m_ActiveObject, -1, -1, -1);
	if (object.valid()) CSPSim::theSim->setActiveObject(object);
}

void GameScreen::on_ViewPanLeft()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fangleOffsetZ = - angleOffset;
}

void GameScreen::on_ViewPanRight()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fangleOffsetZ = + angleOffset;
}

void GameScreen::on_ViewPanLeftStop() 
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_fangleOffsetZ < 0.0) m_fangleOffsetZ = 0.0;
}

void GameScreen::on_ViewPanRightStop() 
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_fangleOffsetZ > 0.0) m_fangleOffsetZ = 0.0;
}

void GameScreen::on_ViewPanUpStop()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_fangleOffsetX < 0.0) m_fangleOffsetX = 0.0;
}

void GameScreen::on_ViewPanDownStop()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		if (m_fangleOffsetX > 0.0) m_fangleOffsetX = 0.0;
}

void GameScreen::on_ViewZoomStop()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fscaleFactor = 1.0;
}

void GameScreen::on_ViewPanUp()
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fangleOffsetX = - angleOffset;
}

void GameScreen::on_ViewPanDown() 
{
	if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fangleOffsetX = angleOffset;
}

void GameScreen::on_ViewZoomIn() 
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fscaleFactor = (1.00 / 1.05);
}

void GameScreen::on_ViewZoomOut()
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fscaleFactor = 1.05;
}

void GameScreen::on_ViewZoomStepIn() 
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fdisToObject *= 0.75;
}

void GameScreen::on_ViewZoomStepOut()
{
	if ( m_iViewMode == 2 || m_iViewMode == 3 ) 
		m_fdisToObject *= 1.25;
}

void GameScreen::on_ViewFovStepDec() 
{
	float fov = m_Battlefield->getViewAngle() * 0.80;
	if (fov > 10.0) {
		m_Battlefield->setViewAngle(fov);
	}
}

void GameScreen::on_ViewFovStepInc()
{
	float fov = m_Battlefield->getViewAngle() * 1.20;
	if (fov < 90.0) {
		m_Battlefield->setViewAngle(fov);
	}
}

void GameScreen::on_MouseView(int x, int y, int dx, int dy)
{
	m_fangleOffsetX = 0.0;
	m_fangleOffsetZ = 0.0;
	m_fangleRotZ += dx * 0.001;
	m_fangleRotX += dy * 0.001;
}

/*
bool GameScreen::OnKeyUp(SDLKey key)
{
    CSP_LOG( CSP_APP , CSP_DEBUG, "GameScreen::OnKeyUp: m_iViewMode = " << m_iViewMode << "; key = " << key); 

	switch ( key )
	{
	case SDLK_KP4:
    case SDLK_KP6:
			if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
				m_fangleOffsetZ = 0.0;
			break;
	case SDLK_KP2:
	case SDLK_KP8:
		    if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
				m_fangleOffsetX = 0.0;
	    	break;
	case SDLK_KP_MINUS:
    case SDLK_KP_PLUS:
		if ( m_iViewMode == 1 || m_iViewMode == 2 || m_iViewMode == 3 ) 
				m_fscaleFactor = 1.0;
		    break;
	default:
		return false;
		break;
	}
	return true;
}
 
void GameScreen::OnMouseMove(int x, int y)
{

}
 
void GameScreen::OnMouseButtonDown(int num, int x, int y)
{
  // if the joystick button is down fire a missile

}

void GameScreen::OnJoystickAxisMotion(int joynum, int axis, int val)
{
}

void GameScreen::OnJoystickHatMotion(int joynum, int hat, int val)
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "GameScreen::OnJoystickHatMotion, joystick: " << joynum
		<< ", hat: " << hat << ", val: " << val );
	
	switch ( joynum )
	{
	case 0:
		switch ( hat )
		{
		case 0:
			switch ( val )
			{
			case SDL_HAT_RIGHT:
				m_fangleOffsetZ = angleOffset;
				break;
			case SDL_HAT_LEFT:
				m_fangleOffsetZ = - angleOffset;
				break;
			case SDL_HAT_UP:
				m_fangleOffsetX = - angleOffset;
				break;
			case SDL_HAT_DOWN:
				m_fangleOffsetX = angleOffset;
				break;
			case SDL_HAT_RIGHTUP:
				
			case SDL_HAT_RIGHTDOWN:
				
			case SDL_HAT_LEFTUP:
				
			case SDL_HAT_LEFTDOWN:
				break;
			case SDL_HAT_CENTERED:
				m_fangleOffsetX = 0.0;
				m_fangleOffsetZ = 0.0;
			    break;
			}
		}
		break;
	}
}

void GameScreen::OnJoystickButtonDown(int joynum, int butnum)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "OnJoystickButtonDown, joystick: " << joynum
                << ", butnum: " << butnum );
// this has been removed for now.  this was just for testing anyway, since missile firing will be
// done via the active object's callbacks, not the gamescreen.  since i'm dumping the ObjectFactory
// infrastructure i've had to comment it out, but it may still a useful starting point in the
// future.
#if 0


  if (butnum == 0 && missile_delay > 0.5)
  {

      MissileObject * pMissile = (MissileObject * )g_pObjectFactory->createNamedObject("AMRAAM", "UNNAMED MISSILE");

      pMissile->setOrientation( m_ActiveObject->getOrientation());
      pMissile->setGlobalPosition( m_ActiveObject->getGlobalPosition() 
		                         - 3.0 * ( m_ActiveObject->getUpDirection() ) 
								 - 8.0 * (  m_ActiveObject->getDirection()) );
      pMissile->setVelocity( m_ActiveObject->getVelocity() );
      g_pBattlefield->addObject( pMissile );

      missile_delay = 0;
  }
#endif

}
*/

// TODO All references to planes should be made generic!

void GameScreen::SetCamera()
{	
	simdata::Vector3 eyePos;
	simdata::Vector3 lookPos;
	simdata::Vector3 upVec;

	if (!m_ActiveObject) {
		// temporary view if there are no active objects... probably
		// should switch to an "action" view mode in this case.
		TurnViewAboutX();
		TurnViewAboutZ();
		eyePos = simdata::Vector3(483000.0, 499000.0, 2000.0);
		//lookPos = simdata::Vector3(483000.0, 499005.0, 2000.0);
		upVec = simdata::Vector3::ZAXIS;
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(upVec,  - m_fangleRotZ);
		RotX.FromAxisAngle(RotZ * simdata::Vector3::XAXIS, - m_fangleRotX);
		simdata::Matrix3 R;
		R = RotX * RotZ;
		lookPos = RotX * RotZ * simdata::Vector3::YAXIS + eyePos;
		CSP_LOG( CSP_APP , CSP_DEBUG, "SetCamera: " << R.asString() << " : " << (simdata::Vector3::XAXIS).asString());
	}
	else switch ( m_iViewMode )
	{
	case 1: // view_mode one is normal inside the cockpit view
	{
		TurnViewAboutX(M_PI / 3);
		TurnViewAboutZ(M_PI / 3);
		simdata::Vector3 planePos = m_ActiveObject->getGlobalPosition();
		simdata::Vector3 planeDir = m_ActiveObject->getDirection();
		simdata::Vector3 planeUp = m_ActiveObject->getUpDirection();
		simdata::Vector3 planeRight = planeDir^planeUp;
		simdata::Matrix3 RotZ, RotX;
		RotZ.FromAxisAngle(planeUp,  - m_fangleRotZ);
		RotX.FromAxisAngle(RotZ * planeRight, - m_fangleRotX);
		eyePos = planePos;
		lookPos = 5.0 * RotX * RotZ * planeDir + eyePos;
		upVec = planeUp;
		break;
	}
	case 2: // view mode two is external view; normal view is behind the plane
	{ 	
		TurnViewAboutX();
		TurnViewAboutZ();
		ScaleView();
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
		TurnViewAboutX();
		TurnViewAboutZ();
		ScaleView();
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
			m_fixedCamPos = GetNewFixedCamPos(m_ActiveObject.ptr());
		eyePos = m_fixedCamPos;
		upVec = simdata::Vector3::ZAXIS;
		break;
	}    
	if (m_Battlefield) {	
		m_Battlefield->setLookAt(eyePos, lookPos, upVec);
	}
}
