// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file FlightModel.h
 *
 **/


#ifndef __CSPSIM_FLIGHTMODEL_H__
#define __CSPSIM_FLIGHTMODEL_H__


#include <csp/csplib/data/LUT.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector3.h>


/**
 * class FlightModel
 *
 * An interface for aircraft flight models.  FlightModel subclass instances are
 * are shared by all aircraft of a given type.  Each aircraft has its own
 * FlightDynamics instance that stores state information and drives the
 * FlightModel.  To pass parameters more efficiently, the FlightDynamics
 * instance sets a number of temporary state variables within the FlightModel,
 * then performs a number of computations using these variables.  Hence, the
 * FlightModel methods are not suitable for multithreaded environments in which
 * more than one aircraft is simulated at a time.
 *
 */
class FlightModel: public simdata::Object {
public:
	SIMDATA_DECLARE_ABSTRACT_OBJECT(FlightModel)

	FlightModel();
	virtual ~FlightModel();

	/** Initialize the flight model with the current airstream parameters.  Called before forces
	 *  and moments are calculated.
	 */
	inline void setAirstream(double alpha, double alphaDot, double beta, double mach, double airspeed, double qBar) {
		m_Alpha = alpha;
		m_AlphaDot = alphaDot;
		m_Beta = beta;
		m_Mach = mach;
		m_AirSpeed = airspeed;
		m_qBarS = qBar * m_WingArea;
		m_Inv2V = 0.5 / std::max(0.5, airspeed);
	}

	/** Initialize the flight model with the control surface deflections of the aircraft.  Called
	 *  before forces and moments are calculated.
	 */
	inline void setControlSurfaces(double aileron, double elevator, double rudder, double airbrake, double lef, double tef) {
		m_Aileron = aileron;
		m_Elevator = elevator;
		m_Rudder = rudder;
		m_Airbrake = airbrake;
		m_LeadingEdgeFlap = lef;
		m_TrailingEdgeFlap = tef;
	}

	/** Initialize the flight model with kinetic parameters of the aircraft.  Called before
	 *  forces and moments are calculated.
	 */
	inline void setKinetics(simdata::Vector3 const &cg_body, simdata::Vector3 const &angular_velocity_body, double height) {
		m_CenterOfMassBody = cg_body;
		m_AngularVelocityBody = angular_velocity_body;
		m_GE = (height + m_GroundEffectOffset < m_WingSpan) ? groundEffect(height) : 1.0;
	}

	/** Calculate the net aerodynamic force and torque on the aircraft for the current
	 *  flight conditions.  This method is called after the flight conditions have been
	 *  initialized.
	 */
	virtual void calculateForceAndMoment(simdata::Vector3 &force, simdata::Vector3 &moment) = 0;

protected:

	/** Compute static values derived from the xml parameters.
	 */
	virtual void postCreate();

	/** Compute the ground effect scaling at the given model height.  The ground effect must
	 *  be 1.0 when the height (plus the ground effect offset) is greater than the wing span.
	 */
	virtual double groundEffect(double height);

	// xml interface parameters
	double m_WingSpan;
	double m_WingChord;            // mean chord length
	double m_WingArea;             // surface area of wings
	double m_GroundEffectOffset;   // relative height of the wing relative to the model origin for ge

	// derived quantities
	double m_HalfWingArea;
	double m_AspectRatio;

	// airstream parameters
	double m_Alpha;
	double m_AlphaDot;
	double m_Beta;
	double m_qBarS;
	double m_Inv2V;
	double m_GE;
	double m_AirSpeed;
	double m_Mach;

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;
	double m_Airbrake;
	double m_LeadingEdgeFlap;
	double m_TrailingEdgeFlap;

	// kinetics
	simdata::Vector3 m_AngularVelocityBody;
	simdata::Vector3 m_CenterOfMassBody;
};


#endif  // __FLIGHTMODEL_H__

