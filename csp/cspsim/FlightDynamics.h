#pragma once
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

#include <csp/cspsim/BaseDynamics.h>
#include <csp/cspsim/Filters.h>
#include <csp/cspsim/stores/StoresDynamics.h>

namespace csp {

class DragProfile;
class FlightModel;
class ThrustData;


/**
 * The primary dynamics class for modeling aircraft flight.  Each aircraft
 * has a FlightDynamics instance.  Each FlightDynamics instance drives a
 * FlightModel instance, which simulates the aerodynamic response of the
 * aircraft.  Note that FlightModels are static objects, which are shared
 * by one or more FlightDynamics instances.
 */
class FlightDynamics: public BaseDynamics {
	friend class FlightModel;
	virtual ~FlightDynamics();
public:
	FlightDynamics();

	CSP_DECLARE_OBJECT(FlightDynamics)

	void computeForceAndMoment(double x);
	void initializeSimulationStep(double dt);
	void postSimulationStep(double dt);

	double getAngleOfAttack() const { return m_Alpha; }
	double getSideSlip() const { return m_Beta; }
	double getAirspeed() const { return m_Airspeed; }

protected:
	Link<FlightModel> m_FlightModel;
	Link<DragProfile> m_DragProfile;

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual void getInfo(InfoList &info) const;

	void updateAirflow(double dt);
	void addExternalDrag();

	// control surfaces
	DataChannel<double>::CRefT b_Aileron;
	DataChannel<double>::CRefT b_Elevator;
	DataChannel<double>::CRefT b_Rudder;
	DataChannel<double>::CRefT b_Airbrake;
	DataChannel<double>::CRefT b_LeadingEdgeFlap;
	DataChannel<double>::CRefT b_TrailingEdgeFlap;
	DataChannel<double>::CRefT b_Density;
	DataChannel<double>::CRefT b_GroundZ;
	DataChannel<double>::CRefT b_Mach;
	DataChannel<Vector3>::CRefT b_AccelerationBody;
	DataChannel<Vector3>::CRefT b_WindVelocity;
	DataChannel<Vector3>::CRefT b_CenterOfMassOffset;
	DataChannel<StoresDynamics>::CRefT b_StoresDynamics;

	// export channels
	DataChannel<double>::RefT b_Alpha;    /** current angle of attack */
	DataChannel<double>::RefT b_Beta;     /** side slip angle */
	DataChannel<double>::RefT b_Airspeed;
	DataChannel<double>::RefT b_QBar;     /** dynamic pressure */
	DataChannel<double>::RefT b_G;  /** normal acceleration in G (includes gravity and rotation) */
	DataChannel<double>::RefT b_LateralG; /** lateral acceleration in G (includes gravity and rotation) */

	double m_Beta;
	double m_Alpha;
	double m_Alpha0;     /** discrete AOA */
	double m_AlphaDot;
	double m_Airspeed;
	double m_QBar;

	Vector3 m_WindVelocityBody;

	LagFilter m_FilterG;
	LagFilter m_FilterLateralG;
};

} // namespace csp
