#ifndef __AIRPLANEINPUT_H__
#define __AIRPLANEINPUT_H__

#include "BaseInput.h"
#include "AirplaneObject.h"


/**
 * class AirplaneInput - Describe me!
 *
 * @author unknown
 */
class AirplaneInput : public BaseInput
{

public:

	AirplaneInput();

	virtual bool OnKeyDown(SDLKey key);
    virtual bool OnKeyUp(SDLKey key);
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseButtonDown(int num, int x, int y);
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val);
	virtual void OnJoystickHatMotion(int joynum, int hat, int val);
    virtual void OnJoystickButtonDown(int joynum, int butnum);
    virtual void OnUpdate() const;

	virtual void SetObject(BaseObject * );

protected:
	AirplaneObject * m_pAirplaneObject;
	double Clamp(double p_setting) const;

	double m_foffsetAileron;
	double m_foffsetElevator;
	double m_foffsetRudder;
    double m_foffsetThrottle;
    static double const offsetControl;
};

#endif
