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
 * @file AircraftSimpleFCS.h
 *
 **/


#ifndef __AIRCRAFTSIMPLEFCS_H__
#define __AIRCRAFTSIMPLEFCS_H__

#include <algorithm>
#include <cmath>

#include <System.h>
#include <Log.h>


class AircraftSimpleFCS: public System {

	class Deflection {
		DataChannel<double>::CRef b_Input;
		DataChannel<double>::Ref b_Output;
		double m_Rate;
		double m_Limit;
	public:
		Deflection(): m_Rate(0.5), m_Limit(0.3) {}
		void setParameters(double rate, double limit) {
			m_Rate = rate;
			m_Limit = limit;
		}
		void bindInput(Bus *bus, std::string const &name) {
			b_Input = bus->getChannel(name, false);
			if (!b_Input) {
				CSP_LOG(INPUT, WARNING, "AicraftSimpleFCS: input channel '" << name << "' unavailable."); 
			}
		}
		void registerOutput(Bus *bus, std::string const &name) {
			b_Output = bus->registerLocalDataChannel(name, 0.0);
		}
		void update(double dt) {
			double input = 0.0;
			double output = b_Output->value();
			if (b_Input.valid()) input = b_Input->value() * m_Limit;
			double smooth = std::min(1.0, 10.0*fabs(output-input));
			if (output < input) {
				output = std::min(output + smooth*m_Rate*dt, m_Limit);
			} else {
				output = std::max(output - smooth*m_Rate*dt, -m_Limit);
			}
			b_Output->value() = output;
		}
		double getDeflectionAngle() const { return b_Output->value(); }
	};


public:
	SIMDATA_OBJECT(AircraftSimpleFCS, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(AircraftSimpleFCS, System)
		SIMDATA_XML("elevator_limit", AircraftSimpleFCS::m_ElevatorLimit, false)
		SIMDATA_XML("aileron_limit", AircraftSimpleFCS::m_AileronLimit, false)
		SIMDATA_XML("rudder_limit", AircraftSimpleFCS::m_RudderLimit, false)
		SIMDATA_XML("airbrake_limit", AircraftSimpleFCS::m_AirbrakeLimit, false)
		SIMDATA_XML("elevator_rate", AircraftSimpleFCS::m_ElevatorRate, false)
		SIMDATA_XML("aileron_rate", AircraftSimpleFCS::m_AileronRate, false)
		SIMDATA_XML("rudder_rate", AircraftSimpleFCS::m_RudderRate, false)
		SIMDATA_XML("airbrake_rate", AircraftSimpleFCS::m_AirbrakeRate, false)
		SIMDATA_XML("max_g", AircraftSimpleFCS::m_MaxG, false)
		SIMDATA_XML("min_g", AircraftSimpleFCS::m_MinG, false)
	END_SIMDATA_XML_INTERFACE

	AircraftSimpleFCS();

protected:
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);
	
	virtual void serialize(simdata::Archive &);
	virtual void convertXML();
	virtual void postCreate();
	virtual double onUpdate(double dt);
	virtual void getInfo(InfoList &info) const;

private:
	Deflection m_Elevator;
	Deflection m_Aileron;
	Deflection m_Rudder;
	Deflection m_Airbrake;

	double m_ElevatorLimit;
	double m_AileronLimit;
	double m_RudderLimit;
	double m_AirbrakeLimit;
	double m_ElevatorRate;
	double m_AileronRate;
	double m_RudderRate;
	double m_AirbrakeRate;
	double m_MaxG;
	double m_MinG;
};


#endif // __AICRAFTSIMPLEFCS_H__

