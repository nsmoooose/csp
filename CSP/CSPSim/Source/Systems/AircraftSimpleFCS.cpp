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
 * @file AircraftSimpleFCS.cpp
 *
 **/


#include <Systems/AircraftSimpleFCS.h>

#include <SimData/Math.h>

#include <sstream>
#include <iomanip>

using simdata::toRadians;
using simdata::toDegrees;


SIMDATA_REGISTER_INTERFACE(AircraftSimpleFCS)


AircraftSimpleFCS::AircraftSimpleFCS()
{
	m_ElevatorLimit1 = 0.3;
	m_ElevatorLimit0 = -m_ElevatorLimit1;
	m_ElevatorRate = 0.5;
	m_AileronLimit1 = 0.3;
	m_AileronLimit0 = -m_AileronLimit1;
	m_AileronRate = 0.5;
	m_RudderLimit = 0.3;
	m_RudderRate = 0.5;
	m_AirbrakeLimit = 0.3;
	m_AirbrakeRate = 0.1;
	m_MaxG = 5.0;
	m_MinG = -2.0;
}

void AircraftSimpleFCS::convertXML() {
	m_ElevatorLimit1 = toRadians(m_ElevatorLimit1);
	m_ElevatorLimit0 = toRadians(m_ElevatorLimit0);
	m_AileronLimit1 = toRadians(m_AileronLimit1);
	m_AileronLimit0 = toRadians(m_AileronLimit0);
	m_RudderLimit = toRadians(m_RudderLimit);
	m_AirbrakeLimit = toRadians(m_AirbrakeLimit);
	m_ElevatorRate = toRadians(m_ElevatorRate);
	m_AileronRate = toRadians(m_AileronRate);
	m_RudderRate = toRadians(m_RudderRate);
	m_AirbrakeRate = toRadians(m_AirbrakeRate);
}

void AircraftSimpleFCS::postCreate() {
	m_Elevator.setParameters(m_ElevatorRate, m_ElevatorLimit0, m_ElevatorLimit1);
	m_Aileron.setParameters(m_AileronRate, m_AileronLimit0, m_AileronLimit1);
	m_Rudder.setParameters(m_RudderRate, m_RudderLimit);
	m_Airbrake.setParameters(m_AirbrakeRate, m_AirbrakeLimit);
	
}

void AircraftSimpleFCS::registerChannels(Bus *bus) {
	m_Elevator.registerOutput(bus, "ControlSurfaces.ElevatorDeflection");
	m_Aileron.registerOutput(bus, "ControlSurfaces.AileronDeflection");
	m_Rudder.registerOutput(bus, "ControlSurfaces.RudderDeflection");
	m_Airbrake.registerOutput(bus, "ControlSurfaces.AirbrakeDeflection");
}

void AircraftSimpleFCS::importChannels(Bus *bus) {
	m_Elevator.bindInput(bus, "ControlInputs.PitchInput");
	m_Aileron.bindInput(bus, "ControlInputs.RollInput");
	m_Rudder.bindInput(bus, "ControlInputs.RudderInput");
	m_Airbrake.bindInput(bus, "ControlInputs.AirbrakeInput");
}


double AircraftSimpleFCS::onUpdate(double dt)
{
	// TODO add G control
	m_Elevator.update(dt);
	m_Aileron.update(dt);
	m_Rudder.update(dt);
	m_Airbrake.update(dt);
	return 0.0;
}

void AircraftSimpleFCS::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line.precision(0);
	line << "de: " << std::setw(3) << toDegrees(m_Elevator.getDeflectionAngle())
	     << ", da: " << std::setw(3) <<  toDegrees(m_Aileron.getDeflectionAngle())
	     << ", dr: " << std::setw(3) <<  toDegrees(m_Rudder.getDeflectionAngle())
	     << ", dsb: " << std::setw(3) << toDegrees(m_Airbrake.getDeflectionAngle());
	info.push_back(line.str());
}


#if 0
double FlightDynamics::controlIVbasis(double p_t) const {
	double cIV = p_t * p_t  * ( 3.0 - 2.0 * p_t);
	return cIV;
}

double FlightDynamics::controlInputValue(double p_gForce) const { 
	// to reduce G, decrease deflection control surface
	/* FIXME move to FCS class
	if (p_gForce > m_GMax && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce < m_GMin && m_ElevatorInput < 0.0) return 0.0;
	if (m_Alpha > m_stallAOA && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce > m_GMax - m_depsilon && m_ElevatorInput > 0.0) {
		return controlIVbasis((m_GMax - p_gForce) / m_depsilon);
	} 
	if ( p_gForce < m_GMin + m_depsilon && m_ElevatorInput < 0.0) {
		return controlIVbasis((p_gForce - m_GMin) / m_depsilon);
	}*/
	return 1.0;
}
	double u = 0.05 + dt;
	m_ElevatorScale	= (1.0 - u) * m_ElevatorScale + u * controlInputValue(m_GForce);
	m_Elevator = m_ElevatorInput * m_ElevatorScale;
#endif
