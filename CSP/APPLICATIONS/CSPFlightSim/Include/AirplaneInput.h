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
 * @file AirplaneInput.h
 *
 **/


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
    virtual void OnUpdate();

	virtual void SetObject(BaseObject * );

protected:
	AirplaneObject * m_pAirplaneObject;
	double Clamp(double p_setting) const;
    double Smooth(double p_setting);

	double m_oldsetting;
	double m_foffsetAileron;
	double m_foffsetElevator;
	double m_foffsetRudder;
    double m_foffsetThrottle;
    static double const offsetControl;
};

#endif // __AIRPLANEINPUT_H__

