// Combat Simulator Project - FlightSim Demo
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


#ifndef __AIRCRAFTINPUTSYSTEM_H__
#define __AIRCRAFTINPUTSYSTEM_H__

#include <SimData/Math.h>

#include "System.h"


class AircraftInputSystem: public System {

	class CombinedInput {
		double m_Rate;
		double m_Decay;
		int m_DecayCount;
		double m_Increment;
		double m_Minimum;
		double m_Maximum;
		DataChannel<double>::Ref m_Channel;
	public:
		CombinedInput(double rate=0.4, double decay=0.9, double minimum=-1.0, double maximum=1.0): 
			m_Rate(rate),
			m_Decay(decay),
			m_DecayCount(0),
			m_Increment(0.0),
			m_Minimum(minimum),
			m_Maximum(maximum)
		{
		}
		void connect(Bus *bus, std::string const &name) {
			m_Channel = bus->registerLocalDataChannel<double>(name, 0.0);
		}
		void update(double dt) {
			double v = m_Channel->value();
			v += m_Increment * dt * m_Rate;
			if (m_DecayCount > 0) {
				m_DecayCount--;
				if (m_Increment == 0.0) { 
					if (m_DecayCount == 0) {
						v = 0.0;
					} else {
						v *= m_Decay;
					}
				}
			}
			m_Channel->value() = simdata::clampTo(v, m_Minimum, m_Maximum);
		}
		double getValue() const {
			return m_Channel->value();
		}
		void setValue(double value) {
			m_Channel->value() = value;
		}
		void setIncrement(double increment) {
			m_Increment = increment;
		}
		void stopIncrement() {
			if (m_Increment > 0.0) m_Increment = 0.0; 
		}
		void stopDecrement() {
			if (m_Increment < 0.0) m_Increment = 0.0; 
		}
		void setDecay(int decay_count) {
			m_DecayCount = decay_count;
		}
	};

public:
	SIMDATA_DECLARE_OBJECT(AircraftInputSystem)

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

	virtual void setThrottle(MapEvent::AxisEvent const &event);
	virtual void setRudder(MapEvent::AxisEvent const &event);
	virtual void setRoll(MapEvent::AxisEvent const &event);
	virtual void setPitch(MapEvent::AxisEvent const &event);
	virtual void setWheelBrake(MapEvent::AxisEvent const &event);
	virtual void setAirbrake(MapEvent::AxisEvent const &event);
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
	// control inputs bus
	CombinedInput m_PitchInput;
	CombinedInput m_RollInput;
	CombinedInput m_RudderInput;
	CombinedInput m_LeftBrakeInput;
	CombinedInput m_RightBrakeInput;
	CombinedInput m_ThrottleInput;
	CombinedInput m_AirbrakeInput;

	bool m_WheelBrakes;

};


#endif // __AICRAFTINPUTSYSTEM_H__

