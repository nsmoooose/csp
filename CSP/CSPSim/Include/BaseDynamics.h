// Combat Simulator Project - CSPSim
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
 * @file BaseDynamics.h
 *
 */

#ifndef __BASEDYNAMICS_H__
#define __BASEDYNAMICS_H__

#include <SimData/Quat.h>
#include <SimData/Vector3.h>

#include <System.h>

namespace simdata {
	class Quat;
}

/**
 * Abstract base class for all object dynamics (physics) modelling.
 *
 * Derived classes must implement computeForceAndMoment(x) to compute 
 * the force and moment acting on the object at a point within the 
 * simulation step.  See documentation associated with this method for 
 * important details.
 */
class BaseDynamics: public System {

protected:
	simdata::Vector3 m_Force, m_Moment;

	simdata::Vector3 const *m_PositionLocal;
	simdata::Vector3 const *m_VelocityBody;
	simdata::Vector3 const *m_AngularVelocityBody;
	simdata::Quat const *m_Attitude;
#if 0
	double const *m_Height;
	bool const *m_NearGround;
	simdata::Vector3 const *m_NormalGround;
	double const *m_qBar;
	simdata::Vector3 const *m_WindBody;
#endif

public:

	typedef simdata::Ref<BaseDynamics> Ref;

	EXTEND_SIMDATA_XML_VIRTUAL_INTERFACE(BaseDynamics, System)
	END_SIMDATA_XML_INTERFACE

	BaseDynamics();
	virtual ~BaseDynamics();

	/**
	 * Called once before one or more integration steps.  Internal 
	 * state changes and slowly varying parameters can be evaluated 
	 * here.
	 *
	 * @param dt the time interval of each of the following 
	 *           integration steps.
	 */
	virtual void initializeSimulationStep(double dt);

	/**
	 * Called before each integration step.  Internal state changes
	 * and slowly varying parameters can be evaluated here.
	 *
	 * @param dt the time interval of the following integration
	 *           step.
	 */
	virtual void preSimulationStep(double dt);

	/**
	 * Called after each integration step.  Internal state changes
	 * and slowly varying parameters can be evaluated here.
	 *
	 * @param dt the time interval of the preceding integration
	 *           step.
	 */
	virtual void postSimulationStep(double dt);

	/**
	 * Evaluate the force and moment acting on the object at a point
	 * within the simulation step.
	 *
	 * This method will be called repeatedly during the Runge-Kutta 
	 * integration.  The parameter x is the time from the start of
	 * the integration, not the absolute time or a monotonic time
	 * interval.  In general you should not need to use this 
	 * parameter, and if you do be sure you really understand what 
	 * it means.  Also, do not modify internal state variables of
	 * the Dynamics class during the computeForceAndMoment() call.  
	 * State changes should only occur instead in the preSimulationStep() 
	 * and postSimulationStep() methods.  In particular, do not do
	 * anything that results in discontinuous force or moment
	 * variations that can prevent the RK solver from converging.
	 *
	 * @param x the time relative to the start of the integration
	 *          step. 
	 */
	virtual void computeForceAndMoment(double x) = 0;

	/**
	 * Get the total force computed in the last update() call.
	 */
	inline simdata::Vector3 getForce() const { return m_Force; }

	/**
	 * Get the total moment computed in the last update() call.
	 */
	inline simdata::Vector3 getMoment() const { return m_Moment; }

	virtual bool needsImpulse() const { return false; }

	/**
	 * Bind object kinematic state variables.  These values can be
	 * used freely by any of the simulation step methods.
	 *
	 * @param position_local the position of the object in global coordinates
	 * @param velocity_body the velocity of the object in body coordinates
	 * @param angular_velocity_body the angular velocity of the object in 
	 *        body cooordinates
	 * @param attitude the orientation of the object
	 */
	void bindKinematics(simdata::Vector3 const &position_local, simdata::Vector3 const &velocity_body, 
	                    simdata::Vector3 const &angular_velocity_body, simdata::Quat const &attitude);

#if 0
	/**
	 * Bind parameters related to the ground directly beneath the object.
	 *
	 * @param near_ground true if the object bounding sphere overlaps the
	 *                    ground.
	 * @param height the (z) height above the ground of the center of the 
	 *               object
	 * @param normal_ground the normal (unit) vector of the ground beneath
	 *                      the object.
	 */
	void bindGroundParameters(bool const &near_ground, double const &height, simdata::Vector3 const &normal_ground);

	/**
	 *  Bind parameters related to aerodynamics.
	 *
	 *  @param qbar the atmospheric density.
	 *  @param wind_body the wind speed in body coordinates.
	 */
	void bindAeroParameters(double const &qbar, simdata::Vector3 const &wind_body);
#endif
};


/**
 * Convenience class for calling the initializeSimulationStep method
 * of multiple dynamics classes.
 */
class InitializeSimulationStep {
	double m_dt;
public:
	InitializeSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics::Ref bd) {
		bd->initializeSimulationStep(m_dt);
	}
};


/**
 * Convenience class calling the postSimulationStep method of
 * multiple dynamics classes.
 */
class PostSimulationStep {
	double m_dt;
public:
	PostSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics::Ref bd) {
		bd->postSimulationStep(m_dt);
	}
};


/**
 * Convenience class calling the preSimulationStep method of
 * multiple dynamics classes.
 */
class PreSimulationStep {
	double m_dt;
public:
	PreSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics::Ref bd) {
		bd->preSimulationStep(m_dt);
	}
};

#endif //__BASEDYNAMICS_H__


