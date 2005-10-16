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

#include "BaseDynamics.h"
#include "Filters.h"
#include "Stores/DragProfile.h"
#include "Stores/StoresDynamics.h"

class FlightModel;
class ThrustData;


/**
 * The primary dynamics class for modeling aircraft flight.  Each aircraft
 * has a FlightDynamics instance.  Each FlightDynamics instance drives a
 * FlightModel instance, which simulates the aerodynamic response of the
 * aircraft.  Note that FlightModels are static simdata objects, which are
 * shared by one or more FlightDynamics instances.
 */
class FlightDynamics: public BaseDynamics {
	friend class FlightModel;
	virtual ~FlightDynamics();
public:
	FlightDynamics();

	SIMDATA_DECLARE_OBJECT(FlightDynamics)

	void computeForceAndMoment(double x);
	void initializeSimulationStep(double dt);
	void postSimulationStep(double dt);

	double getAngleOfAttack() const { return m_Alpha; }
	double getSideSlip() const { return m_Beta; }
	double getAirspeed() const { return m_Airspeed; }

protected:

	simdata::Link<FlightModel> m_FlightModel;
	simdata::Link<DragProfile> m_DragProfile;

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual void getInfo(InfoList &info) const;

	void updateAirflow(double dt);
	void addExternalDrag();

	// control surfaces
	DataChannel<double>::CRef b_Aileron;
	DataChannel<double>::CRef b_Elevator;
	DataChannel<double>::CRef b_Rudder;
	DataChannel<double>::CRef b_Airbrake;
	DataChannel<double>::CRef b_LeadingEdgeFlap;
	DataChannel<double>::CRef b_TrailingEdgeFlap;
	DataChannel<double>::CRef b_Density;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<double>::CRef b_Mach;
	DataChannel<simdata::Vector3>::CRef b_AccelerationBody;
	DataChannel<simdata::Vector3>::CRef b_WindVelocity;
	DataChannel<simdata::Vector3>::CRef b_CenterOfMassOffset;
	DataChannel<StoresDynamics>::CRef b_StoresDynamics;

	// export channels
	DataChannel<double>::Ref b_Alpha;    // current angle of attack
	DataChannel<double>::Ref b_Beta;     // side slip angle
	DataChannel<double>::Ref b_Airspeed;
	DataChannel<double>::Ref b_QBar;     // dynamic pressure
	DataChannel<double>::Ref b_G;  // normal acceleration in G (includes gravity and rotation)
	DataChannel<double>::Ref b_LateralG; // lateral acceleration in G (includes gravity and rotation)

	double m_Beta;
	double m_Alpha;
	double m_Alpha0;     // discrete AOA
	double m_AlphaDot;
	double m_Airspeed;
	double m_QBar;

	simdata::Vector3 m_WindVelocityBody;

	LagFilter m_FilterG;
	LagFilter m_FilterLateralG;
};


#endif  // __FLIGHTDYNAMICS_H__

