// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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


#include <csp/cspsim/systems/AircraftSimpleFCS.h>
#include <csp/cspsim/ControlInputsChannels.h>
#include <csp/cspsim/ControlSurfacesChannels.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <sstream>
#include <iomanip>

CSP_NAMESPACE

CSP_XML_BEGIN(AircraftSimpleFCS)
	CSP_DEF("elevator_limit0", m_ElevatorLimit0, false)
	CSP_DEF("elevator_limit1", m_ElevatorLimit1, false)
	CSP_DEF("aileron_limit0", m_AileronLimit0, false)
	CSP_DEF("aileron_limit1", m_AileronLimit1, false)
	CSP_DEF("rudder_limit", m_RudderLimit, false)
	CSP_DEF("airbrake_limit", m_AirbrakeLimit, false)
	CSP_DEF("elevator_rate", m_ElevatorRate, false)
	CSP_DEF("aileron_rate", m_AileronRate, false)
	CSP_DEF("rudder_rate", m_RudderRate, false)
	CSP_DEF("airbrake_rate", m_AirbrakeRate, false)
	CSP_DEF("max_g", m_MaxG, false)
	CSP_DEF("min_g", m_MinG, false)
CSP_XML_END


class AircraftSimpleFCS::Deflection {
public:
	Deflection(): m_Rate(0.5), m_Limit0(-0.3), m_Limit1(-m_Limit0), m_Limit(m_Limit1) {}
	virtual ~Deflection() {}
	void setParameters(double rate, double limit) {
		limit = std::abs(limit);
		setParameters(rate,-limit,limit);
	}
	void setParameters(double rate, double limit0, double limit1) {
		m_Rate = rate;
		m_Limit0 = limit0;
		m_Limit1 = limit1;
		m_Limit = std::max(std::abs(m_Limit0),std::abs(m_Limit1));
	}
	void bindInput(Bus *bus, std::string const &name) {
		b_Input = bus->getChannel(name, false);
		if (!b_Input) {
			CSPLOG(WARNING, INPUT) << "AircraftSimpleFCS: input channel '" << name << "' unavailable.";
		}
	}
	void registerOutput(Bus *bus, std::string const &name) {
		b_Output = bus->registerLocalDataChannel(name, 0.0);
	}
	// experiment a poor flatting function
	virtual double flat(double x) const {
		double abs_x = std::abs(x);
		double scale = x*x*(3.0 - 2.0*abs_x);
		return scale * x;
	}
	virtual void update(double dt) {
		double input = 0.0;
		double output = b_Output->value();
		if (b_Input.valid()) input = flat(b_Input->value()) * m_Limit;
		double smooth = std::min(1.0, 10.0*std::abs(output-input));
		if (output < input) {
			output = std::min(output + smooth*m_Rate*dt, m_Limit1);
		} else {
			output = std::max(output - smooth*m_Rate*dt, m_Limit0);
		}
		b_Output->value() = output;
	}
	double getDeflectionAngle() const { return b_Output->value(); }

private:
	DataChannel<double>::CRefT b_Input;
	DataChannel<double>::RefT b_Output;
	double m_Rate;
	double m_Limit0, m_Limit1, m_Limit;
};


AircraftSimpleFCS::AircraftSimpleFCS():
	m_Elevator(new Deflection),
	m_Aileron(new Deflection),
	m_Rudder(new Deflection),
	m_Airbrake(new Deflection)
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
	m_Elevator->setParameters(m_ElevatorRate, m_ElevatorLimit0, m_ElevatorLimit1);
	m_Aileron->setParameters(m_AileronRate, m_AileronLimit0, m_AileronLimit1);
	m_Rudder->setParameters(m_RudderRate, m_RudderLimit);
	m_Airbrake->setParameters(m_AirbrakeRate, m_AirbrakeLimit);
}

void AircraftSimpleFCS::registerChannels(Bus *bus) {
	m_Elevator->registerOutput(bus, bus::ControlSurfaces::ElevatorDeflection);
	m_Aileron->registerOutput(bus, bus::ControlSurfaces::AileronDeflection);
	m_Rudder->registerOutput(bus, bus::ControlSurfaces::RudderDeflection);
	m_Airbrake->registerOutput(bus, bus::ControlSurfaces::AirbrakeDeflection);
}

void AircraftSimpleFCS::importChannels(Bus *bus) {
	m_Elevator->bindInput(bus, bus::ControlInputs::PitchInput);
	m_Aileron->bindInput(bus, bus::ControlInputs::RollInput);
	m_Rudder->bindInput(bus, bus::ControlInputs::RudderInput);
	m_Airbrake->bindInput(bus, bus::ControlInputs::AirbrakeInput);
}


double AircraftSimpleFCS::onUpdate(double dt) {
	m_Elevator->update(dt);
	m_Aileron->update(dt);
	m_Rudder->update(dt);
	m_Airbrake->update(dt);
	return 0.0;
}

void AircraftSimpleFCS::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line.precision(0);
	line << "de: " << std::setw(3) << toDegrees(m_Elevator->getDeflectionAngle())
	     << ", da: " << std::setw(3) <<  toDegrees(m_Aileron->getDeflectionAngle())
	     << ", dr: " << std::setw(3) <<  toDegrees(m_Rudder->getDeflectionAngle())
	     << ", dsb: " << std::setw(3) << toDegrees(m_Airbrake->getDeflectionAngle());
	info.push_back(line.str());
}

CSP_NAMESPACE_END

