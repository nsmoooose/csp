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
		DataChannel<double>::Ref m_Channel;
	public:
		CombinedInput(double rate=0.4, double decay=0.9): 
			m_Rate(rate),
			m_Decay(decay), 
			m_DecayCount(0), 
			m_Increment(0.0) 
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
				if (m_Increment == 0.0) v *= m_Decay;
			}
			m_Channel->value() = simdata::clampTo(v,-1.0,1.0);
		}
		double getValue() {
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
	SIMDATA_OBJECT(AircraftInputSystem, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(AircraftInputSystem, System)
	END_SIMDATA_XML_INTERFACE

	AircraftInputSystem();

	AXIS_INTERFACE(AircraftInputSystem, setThrottle);
	AXIS_INTERFACE(AircraftInputSystem, setRudder);
	AXIS_INTERFACE(AircraftInputSystem, setRoll);
	AXIS_INTERFACE(AircraftInputSystem, setPitch);
	AXIS_INTERFACE(AircraftInputSystem, setWheelBrake);
	AXIS_INTERFACE(AircraftInputSystem, setAirbrake);
	ACTION_INTERFACE(AircraftInputSystem, IncPitch);
	ACTION_INTERFACE(AircraftInputSystem, noIncPitch);
	ACTION_INTERFACE(AircraftInputSystem, DecPitch);
	ACTION_INTERFACE(AircraftInputSystem, noDecPitch);
	ACTION_INTERFACE(AircraftInputSystem, IncRoll);
	ACTION_INTERFACE(AircraftInputSystem, noIncRoll);
	ACTION_INTERFACE(AircraftInputSystem, DecRoll);
	ACTION_INTERFACE(AircraftInputSystem, noDecRoll);
	ACTION_INTERFACE(AircraftInputSystem, IncRudder);
	ACTION_INTERFACE(AircraftInputSystem, noIncRudder);
	ACTION_INTERFACE(AircraftInputSystem, DecRudder);
	ACTION_INTERFACE(AircraftInputSystem, noDecRudder);
	ACTION_INTERFACE(AircraftInputSystem, IncThrottle);
	ACTION_INTERFACE(AircraftInputSystem, noIncThrottle);
	ACTION_INTERFACE(AircraftInputSystem, DecThrottle);
	ACTION_INTERFACE(AircraftInputSystem, noDecThrottle);
	ACTION_INTERFACE(AircraftInputSystem, WheelBrakePulse);
	ACTION_INTERFACE(AircraftInputSystem, WheelBrakeOn);
	ACTION_INTERFACE(AircraftInputSystem, WheelBrakeOff);
	ACTION_INTERFACE(AircraftInputSystem, WheelBrakeToggle);
	ACTION_INTERFACE(AircraftInputSystem, OpenAirbrake);
	ACTION_INTERFACE(AircraftInputSystem, CloseAirbrake);
	ACTION_INTERFACE(AircraftInputSystem, IncAirbrake);
	ACTION_INTERFACE(AircraftInputSystem, DecAirbrake);

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

