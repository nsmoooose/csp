#include "stdinc.h"


#include "AirplaneInput.h"
#include "AirplaneObject.h"


AirplaneInput::AirplaneInput()
{


}

void AirplaneInput::SetObject(BaseObject * pObject)
{

	m_pAirplaneObject = dynamic_cast<AirplaneObject* >(pObject);
}

bool AirplaneInput::OnKeyDown(SDLKey key)
{

	return false;
}

bool AirplaneInput::OnKeyUp(SDLKey key)
{

	return false;
}

void AirplaneInput::OnMouseMove(int x, int y)
{

}

void AirplaneInput::OnMouseButtonDown(int num, int x, int y)
{

}

void AirplaneInput::OnJoystickAxisMotion(int joynum, int axis, int val)
{
    CSP_LOG(CSP_APP, CSP_DEBUG, "OnJoystickAxisMotion, joystick: " << joynum
                << ", axis: " << axis << ", val: " << val );

    float setting = val / 32768.0;

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
