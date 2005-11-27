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

#ifndef __CSPSIM_BASEDYNAMICS_H__
#define __CSPSIM_BASEDYNAMICS_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/data/Vector3.h>

#include <csp/cspsim/System.h>

CSP_NAMESPACE


class Quat;

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
	Vector3 m_Force, m_Moment;

	Vector3 const *m_PositionLocal;
	Vector3 const *m_VelocityBody;
	Vector3 const *m_AngularVelocityBody;
	Quat const *m_Attitude;
	Vector3 const *m_CenterOfMassOffsetLocal;

public:
	typedef Ref<BaseDynamics> RefT;

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
	inline Vector3 const &getForce() const { return m_Force; }

	/**
	 * Get the total moment computed in the last update() call.
	 */
	inline Vector3 const &getMoment() const { return m_Moment; }

	virtual bool needsImpulse() const { return false; }

	/**
	 * Get the model position in local coordinates.  This is the physical
	 * location of the aircraft, as opposed to m_PositionLocal which tracks
	 * the center of mass.
	 */
	inline Vector3 getModelPositionLocal() const {
		return *m_PositionLocal - *m_CenterOfMassOffsetLocal;
	}

	/**
	 * Bind object kinematic state variables.  These values can be
	 * used freely by any of the simulation step methods.
	 *
	 * @param position_local the position of the object's center of mass
	 *   in global coordinates
	 * @param velocity_body the velocity of the object in body coordinates
	 * @param angular_velocity_body the angular velocity of the object in
	 *   body cooordinates
	 * @param attitude the orientation of the object
	 * @param center_of_mass_local the offset from the model position to the
	 *   center of mass in local coordinates.
	 */
	void bindKinematics(Vector3 const &position_local, Vector3 const &velocity_body,
	                    Vector3 const &angular_velocity_body, Quat const &attitude,
	                    Vector3 const &center_of_mass_offset_local);

};


/**
 * Convenience class for calling the initializeSimulationStep method
 * of multiple dynamics classes.
 */
class InitializeSimulationStep {
	double m_dt;
public:
	InitializeSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics::RefT bd) {
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
	void operator()(BaseDynamics::RefT bd) {
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
	void operator()(BaseDynamics::RefT bd) {
		bd->preSimulationStep(m_dt);
	}
};


CSP_NAMESPACE_END

#endif //__BASEDYNAMICS_H__


