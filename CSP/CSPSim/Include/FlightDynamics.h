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
 * @file FlightDynamics.h
 *
 **/


#ifndef __FLIGHTDYNAMICS_H__
#define __FLIGHTDYNAMICS_H__

#include <SimData/InterfaceRegistry.h>

#include "BaseDynamics.h"

class FlightModel;


/**
 * class FlightDynamics - aircraft primary flight model implementation.
 *
 */
class FlightDynamics: public BaseDynamics {
	friend class FlightModel;
	virtual ~FlightDynamics();
public:
	FlightDynamics();

	SIMDATA_OBJECT(FlightDynamics, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(FlightDynamics, BaseDynamics)
		SIMDATA_XML("flight_model", FlightDynamics::m_FlightModel, true)
	END_SIMDATA_XML_INTERFACE

	void computeForceAndMoment(double x);
	void initializeSimulationStep(double dt);
	void postSimulationStep(double dt);

	double getAngleOfAttack() const { return m_Alpha; }
	double getSideSlip() const { return m_Beta; }
	double getAirspeed() const { return m_Airspeed; }

protected:

	simdata::Link<FlightModel> m_FlightModel;

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual void getInfo(InfoList &info) const;

	void updateAirflow(double dt);

	// control surfaces
	DataChannel<double>::CRef b_Aileron;
	DataChannel<double>::CRef b_Elevator;
	DataChannel<double>::CRef b_Rudder;
	DataChannel<double>::CRef b_Airbrake;
	DataChannel<double>::CRef b_Density;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<simdata::Vector3>::CRef b_AccelerationBody;
	DataChannel<simdata::Vector3>::CRef b_WindVelocity;

	// export channels
	DataChannel<double>::Ref b_Alpha;    // current angle of attack
	DataChannel<double>::Ref b_Beta;     // side slip angle
	DataChannel<double>::Ref b_Airspeed;
	DataChannel<double>::Ref b_QBar;     // dynamic pressure
	DataChannel<double>::Ref b_G;        // G force (includes gravity)

	double m_Beta;
	double m_Alpha;
	double m_Alpha0;     // discrete AOA
	double m_AlphaDot;
	double m_Airspeed;
	double m_QBar;

	simdata::Vector3 m_WindVelocityBody;

};


#endif  // __FLIGHTDYNAMICS_H__

