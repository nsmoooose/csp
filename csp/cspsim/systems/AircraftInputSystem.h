#pragma once
// Combat Simulator Project
// Copyright (C) 2002, 2003 The Combat Simulator Project
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
 * @file AircraftInputSystem.h
 *
 **/

#include <csp/cspsim/System.h>

#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/ScopedPointer.h>

namespace csp {

class AircraftInputSystem: public System {
public:
	CSP_DECLARE_OBJECT(AircraftInputSystem)

	DECLARE_INPUT_INTERFACE(AircraftInputSystem, System)
		BIND_AXIS("THROTTLE", setThrottle);
		BIND_AXIS("AILERON", setRoll);
		BIND_AXIS("ELEVATOR", setPitch);
		BIND_AXIS("RUDDER", setRudder);
		BIND_AXIS("AIRBRAKE", setAirbrake);
		BIND_AXIS("WHEEL_BRAKE", setWheelBrake);
		BIND_ACTION("INC_THROTTLE", IncThrottle);
		BIND_ACTION("STOP_INC_THROTTLE", noIncThrottle);
		BIND_ACTION("DEC_THROTTLE", DecThrottle);
		BIND_ACTION("STOP_DEC_THROTTLE", noDecThrottle);
		BIND_ACTION("INC_AILERON", IncRoll);
		BIND_ACTION("STOP_INC_AILERON", noIncRoll);
		BIND_ACTION("DEC_AILERON", DecRoll);
		BIND_ACTION("STOP_DEC_AILERON", noDecRoll);
		BIND_ACTION("INC_ELEVATOR", IncPitch);
		BIND_ACTION("STOP_INC_ELEVATOR", noIncPitch);
		BIND_ACTION("DEC_ELEVATOR", DecPitch);
		BIND_ACTION("STOP_DEC_ELEVATOR", noDecPitch);
		BIND_ACTION("INC_RUDDER", IncRudder);
		BIND_ACTION("STOP_INC_RUDDER", noIncRudder);
		BIND_ACTION("DEC_RUDDER", DecRudder);
		BIND_ACTION("STOP_DEC_RUDDER", noDecRudder);
		BIND_ACTION("INC_AIRBRAKE", IncAirbrake);
		BIND_ACTION("DEC_AIRBRAKE", DecAirbrake);
		BIND_ACTION("OPEN_AIRBRAKE", OpenAirbrake);
		BIND_ACTION("CLOSE_AIRBRAKE", CloseAirbrake);
		BIND_ACTION("WHEEL_BRAKE_PULSE", WheelBrakePulse);
		BIND_ACTION("WHEEL_BRAKE_ON", WheelBrakeOn);
		BIND_ACTION("WHEEL_BRAKE_OFF", WheelBrakeOff);
		BIND_ACTION("WHEEL_BRAKE_TOGGLE", WheelBrakeToggle);
	END_INPUT_INTERFACE  // protected:

public:
	AircraftInputSystem();

	virtual void setThrottle(input::MapEvent::AxisEvent const &event);
	virtual void setRudder(input::MapEvent::AxisEvent const &event);
	virtual void setRoll(input::MapEvent::AxisEvent const &event);
	virtual void setPitch(input::MapEvent::AxisEvent const &event);
	virtual void setWheelBrake(input::MapEvent::AxisEvent const &event);
	virtual void setAirbrake(input::MapEvent::AxisEvent const &event);
	virtual void IncPitch();
	virtual void noIncPitch();
	virtual void DecPitch();
	virtual void noDecPitch();
	virtual void IncRoll();
	virtual void noIncRoll();
	virtual void DecRoll();
	virtual void noDecRoll();
	virtual void IncRudder();
	virtual void noIncRudder();
	virtual void DecRudder();
	virtual void noDecRudder();
	virtual void IncThrottle();
	virtual void noIncThrottle();
	virtual void DecThrottle();
	virtual void noDecThrottle();
	virtual void WheelBrakePulse();
	virtual void WheelBrakeOn();
	virtual void WheelBrakeOff();
	virtual void WheelBrakeToggle();
	virtual void OpenAirbrake();
	virtual void CloseAirbrake();
	virtual void IncAirbrake();
	virtual void DecAirbrake();

protected:
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);
	
	virtual double onUpdate(double dt);

private:
	class CombinedInput;

	// control inputs bus
	ScopedPointer<CombinedInput> m_PitchInput;
	ScopedPointer<CombinedInput> m_RollInput;
	ScopedPointer<CombinedInput> m_RudderInput;
	ScopedPointer<CombinedInput> m_LeftBrakeInput;
	ScopedPointer<CombinedInput> m_RightBrakeInput;
	ScopedPointer<CombinedInput> m_ThrottleInput;
	ScopedPointer<CombinedInput> m_AirbrakeInput;

	bool m_WheelBrakes;
};

} // namespace csp
