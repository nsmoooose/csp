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
 * @file AircraftSimpleFCS.h
 *
 **/


#ifndef __CSPSIM_AIRCRAFTSIMPLEFCS_H__
#define __CSPSIM_AIRCRAFTSIMPLEFCS_H__

#include <algorithm>
#include <cmath>

#include <csp/csplib/util/Math.h>

#include <SimCore/Util/Log.h>

#include <System.h>

class AircraftSimpleFCS: public System {

	class Deflection {
		DataChannel<double>::CRef b_Input;
		DataChannel<double>::Ref b_Output;
		double m_Rate;
		double m_Limit0, m_Limit1, m_Limit;
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
				CSP_LOG(INPUT, WARNING, "AicraftSimpleFCS: input channel '" << name << "' unavailable."); 
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
	};


public:
	SIMDATA_DECLARE_OBJECT(AircraftSimpleFCS)

	AircraftSimpleFCS();

protected:
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);
	
	virtual void convertXML();
	virtual void postCreate();
	virtual double onUpdate(double dt);
	virtual void getInfo(InfoList &info) const;

private:
	Deflection m_Elevator;
	Deflection m_Aileron;
	Deflection m_Rudder;
	Deflection m_Airbrake;

	double m_ElevatorLimit0,m_ElevatorLimit1;
	double m_AileronLimit0, m_AileronLimit1;
	double m_RudderLimit;
	double m_AirbrakeLimit;
	double m_ElevatorRate;
	double m_AileronRate;
	double m_RudderRate;
	double m_AirbrakeRate;
	double m_MaxG;
	double m_MinG;
};


#endif // __CSPSIM_AICRAFTSIMPLEFCS_H__

