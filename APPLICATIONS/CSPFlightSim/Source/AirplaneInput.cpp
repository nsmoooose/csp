 #include "stdinc.h"

#include "AirplaneInput.h"
#include "AirplaneObject.h"

extern double frameSeconds();

double const AirplaneInput::offsetControl = 0.05;

AirplaneInput::AirplaneInput()
{
  m_foffsetAileron = 0.0;
  m_foffsetElevator = 0.0;
  m_foffsetRudder = 0.0;
  m_foffsetThrottle = 0.0;
  m_oldsetting = 0.0;
}

void AirplaneInput::SetObject(BaseObject * pObject) 
{
	m_pAirplaneObject = dynamic_cast<AirplaneObject* >(pObject);
}

double AirplaneInput::Smooth(double p_setting)
{
 double u = 0.8 * frameSeconds();
 p_setting = u * m_oldsetting + (1.0 - u) * p_setting;
 m_oldsetting = p_setting;
 return p_setting;
}

double AirplaneInput::Clamp(double p_setting) const
{
	if ( p_setting < - 1.0)
		p_setting = - 1.0;
	else 
		if ( p_setting > 1.0 )
			p_setting = 1.0;
	return p_setting;
}

void AirplaneInput::OnUpdate()
{
	if (m_foffsetAileron != 0.0)
	{
		double setting = m_pAirplaneObject->getAileron() + m_foffsetAileron;
		setting = Smooth(Clamp(setting));
		m_pAirplaneObject->setAileron( setting );
	}
	else
	{
		if (m_foffsetElevator != 0.0)
		{
			double setting = m_pAirplaneObject->getElevator() + m_foffsetElevator;
			setting = Smooth(Clamp(setting));
			m_pAirplaneObject->setElevator( setting );
		}
		else
		{
			if (m_foffsetRudder != 0.0)
			{
				double setting = m_pAirplaneObject->getRudder() + m_foffsetRudder;
				setting = Smooth(Clamp(setting));
				m_pAirplaneObject->setRudder( setting );
			}
			else
				if (m_foffsetThrottle != 0.0)
				{
					double setting = m_pAirplaneObject->getThrottle() + m_foffsetThrottle;
					setting = Smooth(Clamp(setting));
					m_pAirplaneObject->setThrottle( setting );
				}
		}
	}
}

bool AirplaneInput::OnKeyDown(SDLKey key)
{
	CSP_LOG( CSP_APP , CSP_DEBUG, "AirplaneInput::OnKeyDown; key = " << key); 
	switch ( key )
	{
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
	default:
		return false;
		break;
	}
	OnUpdate();
	return true;
}

bool AirplaneInput::OnKeyUp(SDLKey key)
{
	CSP_LOG( CSP_APP , CSP_DEBUG, "AirplaneInput::OnKeyUp; key = " << key);
	switch ( key )
	{
	case SDLK_LEFT:
	case SDLK_RIGHT:
		m_foffsetAileron = 0.0;
		//m_pAirplaneObject->setAileron(0.0);
		break;
	case SDLK_UP:
	case SDLK_DOWN:
		m_foffsetElevator = 0.0;
		//m_pAirplaneObject->setElevator(0.0);
		break;
	case SDLK_COMMA:
	case SDLK_PERIOD:
        m_foffsetRudder = 0.0;
		//m_pAirplaneObject->setRudder(0.0);
		break;
	case SDLK_EQUALS:
	case SDLK_MINUS:
		m_foffsetThrottle = 0.0;
		break;
	default:
		return false;
		break;
	}
	return true;
}

void AirplaneInput::OnMouseMove(int x, int y)
{

}

void AirplaneInput::OnMouseButtonDown(int num, int x, int y)
{

}

void AirplaneInput::OnJoystickAxisMotion(int joynum, int axis, int val)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "AirplaneInput::OnJoystickAxisMotion, joystick: " << joynum
                << ", axis: " << axis << ", val: " << val );

    float setting = Smooth(val / 32768.0);

	switch ( joynum )
	{
	case 0:
		switch ( axis )
		{
		case 0:
            m_pAirplaneObject->setAileron( setting );
			break;
		case 1:
            m_pAirplaneObject->setElevator( setting );
			break;
		case 2:
            m_pAirplaneObject->setThrottle( - setting + 1.0 );
			break;
        case 3:
            m_pAirplaneObject->setRudder( setting );
			break;
		}
	break;
	}

}

void AirplaneInput::OnJoystickHatMotion(int joynum, int hat, int val)
{

}

void AirplaneInput::OnJoystickButtonDown(int joynum, int butnum)
{

}
