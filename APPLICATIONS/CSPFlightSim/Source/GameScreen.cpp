#include "stdinc.h"


#include "GameScreen.h"
#include "TypesMath.h"
#include "VirtualBattlefield.h"
#include "AirplaneObject.h"
#include "MissileObject.h"
#include "ObjectFactory.h"
#include "SDL_keysym.h"

extern VirtualBattlefield * g_pBattlefield;
extern BaseObject * g_pPlayerObject;
extern ObjectFactory * g_pObjectFactory;

double const GameScreen::angleOffset = 0.03;

double const offsetControl = 0.01;

double m_foffsetAileron = 0.0, m_foffsetElevator = 0.0, m_foffsetRudder = 0.0;
double m_foffsetThrottle = 0.0;

void GameScreen::NormalView()
{
	m_fangleRotX = 0.0;
	m_fangleRotZ = 0.0;
	m_fangleOffsetX = 0.0;
	m_fangleOffsetZ = 0.0;
	m_fdisToPlane = 30.0;
	m_fscaleFactor = 1.0;
}

void GameScreen::TurnViewAboutZ(float fangleMax)
{
    CSP_LOG( CSP_APP , CSP_DEBUG, "TurnViewAboutZ: 	m_fangleViewZ = " << m_fangleRotZ 
		<< "; m_fangleOffsetZ = " << m_fangleOffsetZ);
	
	m_fangleRotZ += m_fangleOffsetZ;
	if ( m_fangleRotZ > fangleMax )
		if ( fangleMax == pi )
			m_fangleRotZ -= 2 * pi;
		else
            m_fangleRotZ = fangleMax;
	if ( m_fangleRotZ < - fangleMax )
		if ( fangleMax == pi )
			m_fangleRotZ += 2 * pi;
		else
			m_fangleRotZ = - fangleMax;
}

void GameScreen::TurnViewAboutX(float fangleMax)
{
	CSP_LOG( CSP_APP , CSP_DEBUG, "TurnViewAboutX: 	m_fangleViewX = " << m_fangleRotX 
		     << "; m_fangleOffsetX = " << m_fangleOffsetX);

	m_fangleRotX += m_fangleOffsetX;
	if ( m_fangleRotX > fangleMax )
		m_fangleRotX = fangleMax;
	if ( m_fangleRotX < - fangleMax )
		m_fangleRotX = - fangleMax;
}

void GameScreen::ScaleView()
{
	if ( m_fdisToPlane > 1.0 )
		m_fdisToPlane *= m_fscaleFactor;
}

void GameScreen::OnInit()
{
    m_iViewMode = 1;
    missile_delay = 0.0;
	NormalView();
}

void GameScreen::OnExit()
{

}

void GameScreen::OnRender()
{
    SetCamera();
	
	// Hide the plane if internal view
	if (m_bInteralView)
		g_pPlayerObject->hideObject(true);
	else
		g_pPlayerObject->hideObject(false);

	// Draw the whole scene
    g_pBattlefield->drawScene();

	// Draw the cockpit if interval view
	if (m_bInteralView )
		g_pBattlefield->drawCockpit();

}

void GameScreen::OnUpdateKeyboardControl(BaseObject * p_pPlayerPlane)
{

	// this needs to be moved to AirplaneInput
//	if (m_foffsetAileron != 0.0)
//	{
//		double setting = p_pPlayerPlane->getAileron() + m_foffsetAileron;
//		if ( setting < - 1.0)
//			setting = - 1.0;
//		else 
//			if ( setting > 1.0 )
//				setting = 1.0;
//			p_pPlayerPlane->setAileron( setting );
//	}
//	
//    if (m_foffsetElevator != 0.0)
//	{
//		double setting = p_pPlayerPlane->getElevator() + m_foffsetElevator;
//		if ( setting < - 1.0)
//			setting = - 1.0;
//		else 
//			if ( setting > 1.0 )
//				setting = 1.0;
//			p_pPlayerPlane->setElevator( setting );
//	}
//	
//	if (m_foffsetRudder != 0.0)
//	{
//		double setting = p_pPlayerPlane->getRudder() + m_foffsetRudder;
//		if ( setting < - 1.0)
//			setting = - 1.0;
//		else 
//			if ( setting > 1.0 )
//				setting = 1.0;
//			p_pPlayerPlane->setRudder( setting );
//    }
	
//    if (m_foffsetThrottle != 0.0)
//	{
//		double setting = p_pPlayerPlane->getThrottle() + m_foffsetThrottle;
//		if ( setting < 0.0)
//			setting = 0.0;
//		else 
//			if ( setting > 2.0 )
//				setting = 2.0;
//			p_pPlayerPlane->setThrottle( setting );
//    }
	
}

void GameScreen::OnUpdateObjects(float dt)
{
	OnUpdateKeyboardControl(g_pPlayerObject);
	
    g_pBattlefield->OnUpdate(dt);
    missile_delay += dt;
}

StandardVector3 GameScreen::GetNewFixedCamPos(BaseObject * const p_pPlayerPlane) const
{
	StandardVector3 upVec = p_pPlayerPlane->getUpDirection();
	StandardVector3 planeDir = p_pPlayerPlane->getDirection();

	return (p_pPlayerPlane->getGlobalPosition() + 500.0 * planeDir + ( 50.0 - rand() % 25 ) * (planeDir^upVec) 
		    + ( 25.0 + rand () % 12 ) * upVec);
}

bool GameScreen::OnKeyDown(SDLKey key)
{
	CSP_LOG( CSP_APP , CSP_DEBUG, "OnKeyDown: m_iViewMode = " << m_iViewMode << "; key = " << key); 

	switch ( key )
	{
	case SDLK_1:
		m_iViewMode = 1;
		NormalView();
		m_bInteralView = true;
		break;
	case SDLK_2:
		m_iViewMode = 2;
		NormalView();
		m_bInteralView = false;
		break;
	case SDLK_3:
		m_iViewMode = 3;
		m_bInteralView = false;
		break;
	case SDLK_4:
		m_iViewMode = 4;
		m_bInteralView = false;
		break;
	case SDLK_5:
		m_iViewMode = 5;
		m_bInteralView = false;
		break;
	case SDLK_6:
		m_iViewMode = 6;
		m_bInteralView = false;
		break;
	case SDLK_7:
		m_iViewMode = 7;
		m_bInteralView = false;
		break;
	case SDLK_KP6:
			if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fangleOffsetZ = angleOffset;
			break;
	case SDLK_KP4:
			if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fangleOffsetZ = - angleOffset;
			break;
	case SDLK_KP8:
		    if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fangleOffsetX = - angleOffset;
	    	break;
	case SDLK_KP2:
		    if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fangleOffsetX = angleOffset;
	    	break;
	case SDLK_KP_MINUS:
		if ( m_iViewMode == 2 ) 
				m_fscaleFactor = 2.0;
		    break;
	case SDLK_KP_PLUS:
		if ( m_iViewMode == 2 ) 
				m_fscaleFactor = 0.5;
	    	break;
	case SDLK_8:
			m_iViewMode = 8;
			m_fixedCamPos = GetNewFixedCamPos(g_pPlayerObject);
			m_bInteralView = false;
			break;
	case SDLK_LEFT:
		m_foffsetAileron = - offsetControl;
		break;
	case SDLK_RIGHT:
		m_foffsetAileron = offsetControl;
		break;
	case SDLK_UP:
		m_foffsetElevator = - offsetControl;
		break;
	case SDLK_DOWN:
		m_foffsetElevator = offsetControl;
		break;
	case SDLK_COMMA:
		m_foffsetRudder = - offsetControl;
		break;
	case SDLK_PERIOD:
        m_foffsetRudder = offsetControl;
		break;
    case SDLK_EQUALS:
		m_foffsetThrottle = offsetControl;
		break;
	case SDLK_MINUS:
		m_foffsetThrottle = - offsetControl;
		break;
	case SDLK_ESCAPE:
		    break;
	return true;

	}
	return false;
}

bool GameScreen::OnKeyUp(SDLKey key)
{
    CSP_LOG( CSP_APP , CSP_DEBUG, "OnKeyUp: m_iViewMode = " << m_iViewMode << "; key = " << key); 

	switch ( key )
	{
	case SDLK_KP4:
    case SDLK_KP6:
			if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fangleOffsetZ = 0.0;
			break;
	case SDLK_KP2:
	case SDLK_KP8:
		    if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fangleOffsetX = 0.0;
	    	break;
	case SDLK_KP_MINUS:
    case SDLK_KP_PLUS:
		if ( m_iViewMode == 1 || m_iViewMode == 2 ) 
				m_fscaleFactor = 1.0;
		    break;
	case SDLK_LEFT:
	case SDLK_RIGHT:
		m_foffsetAileron = 0.0;
		break;
	case SDLK_UP:
	case SDLK_DOWN:
		m_foffsetElevator = 0.0;
		break;
	case SDLK_COMMA:
	case SDLK_PERIOD:
        m_foffsetRudder = 0.0;
		break;
	case SDLK_EQUALS:
	case SDLK_MINUS:
		m_foffsetThrottle = 0.0;
		break;
	case SDLK_ESCAPE:
		    break;
    return true;
	}
	return false;
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
	CSP_LOG(CSP_APP, CSP_DEBUG, "OnJoystickHatMotion, joystick: " << joynum
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

  if (butnum == 0 && missile_delay > 0.5)
  {

      MissileObject * pMissile = (MissileObject * )g_pObjectFactory->createNamedObject("AMRAAM", "UNNAMED MISSILE");
      
      pMissile->setOrientation( g_pPlayerObject->getOrientation());
      pMissile->setGlobalPosition( g_pPlayerObject->getGlobalPosition() - 3.0*g_pPlayerObject->getUpDirection() );
      pMissile->setVelocity( 2.0*g_pPlayerObject->getVelocity() );
      g_pBattlefield->addObject( pMissile );

      missile_delay = 0;
  }

}

void GameScreen::SetCamera()
{	
    StandardVector3 eyePos;
    StandardVector3 lookPos;
    StandardVector3 upVec;
    
	switch ( m_iViewMode )
	{
	case 1: // view_mode one is normal inside the cockpit view
		TurnViewAboutX(pi / 3);
		TurnViewAboutZ(pi / 3);
		{
		StandardVector3 planePos = g_pPlayerObject->getGlobalPosition();
		StandardVector3 planeDir = g_pPlayerObject->getDirection();
		StandardVector3 planeUp = g_pPlayerObject->getUpDirection();
		StandardVector3 planeRight = planeDir^planeUp;
		
		StandardMatrix3 RotZ, RotX;
		RotZ.FromAxisAngle(planeUp,  - m_fangleRotZ);
		RotX.FromAxisAngle(RotZ * planeRight, - m_fangleRotX);
        eyePos = planePos;
        lookPos = 5 * RotX * RotZ * planeDir + eyePos;
        upVec = planeUp;
		}
		break;
	case 2: // view mode two is external view; normal view is behind the plane
		{ 	
			TurnViewAboutX();
			TurnViewAboutZ();
			ScaleView();
			
			StandardVector3 planePos = g_pPlayerObject->getGlobalPosition();
			StandardVector3 planeDir = g_pPlayerObject->getDirection();
			StandardVector3 planeUp = g_pPlayerObject->getUpDirection();
			StandardVector3 planeRight = planeDir^planeUp;
			
			StandardMatrix3 RotZ, RotX;
			
			RotZ.FromAxisAngle(planeUp,  m_fangleRotZ);
			StandardVector3 axisX = RotZ * planeRight;
			RotX.FromAxisAngle(axisX, m_fangleRotX);
			
			eyePos = - m_fdisToPlane * RotX * RotZ * planeDir;
			upVec = eyePos^axisX;
			eyePos += planePos;
			lookPos = planePos;
		}
		break;
	case 3: // view mode three is external fixed view behind the plane
		lookPos = g_pPlayerObject->getGlobalPosition();
		eyePos = lookPos - m_fdisToPlane * g_pPlayerObject->getDirection();
		upVec = StandardVector3::ZAXIS;
		break;
	case 4: // view mode four is view looking straight up
		eyePos = g_pPlayerObject->getGlobalPosition();
		lookPos = eyePos + 40 * g_pPlayerObject->getUpDirection();
		upVec = StandardVector3::YAXIS;
		break;
	case 5: // view mode five is sat view
		lookPos = eyePos = g_pPlayerObject->getGlobalPosition();
		eyePos.z += 1000;
		lookPos.z = 0;
		upVec = StandardVector3::XAXIS;
		break;
	case 6: // view mode six is 50 meters above plane
		eyePos = g_pPlayerObject->getGlobalPosition() + 50 * StandardVector3::ZAXIS;
		lookPos = g_pPlayerObject->getGlobalPosition();
		upVec = g_pPlayerObject->getUpDirection();
		break;
	case 7:
		eyePos = g_pPlayerObject->getGlobalPosition() + StandardVector3(0,15,0);
		lookPos = g_pPlayerObject->getGlobalPosition() - StandardVector3(0,15, 0);
		upVec = StandardVector3(0,0,1);
		break;
	case 8: // view mode 8 is a fly by view 
		lookPos = g_pPlayerObject->getGlobalPosition();
		if ((lookPos - m_fixedCamPos).Length() > 900.0 )
			m_fixedCamPos = GetNewFixedCamPos(g_pPlayerObject);
		eyePos = m_fixedCamPos;
		upVec = StandardVector3::ZAXIS;
		break;
    }    
	
    g_pBattlefield->setLookAt(eyePos, lookPos, upVec);
}
