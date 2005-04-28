// Combat Simulator Project - FlightSim Demo
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
 * @file LandingGear.h
 *
 **/


#ifndef __LANDINGGEAR_H__
#define __LANDINGGEAR_H__

#include <vector>

#include <SimData/InterfaceRegistry.h>
#include <SimData/Link.h>
#include <SimData/Math.h>
#include <SimData/Object.h>
#include <SimData/Quat.h>
#include <SimData/Real.h>
#include <SimData/Vector3.h>

#include "BaseDynamics.h"

class GearAnimation;


/** LandingGear encapsulates a single landing gear.  Provides physical
 *  simulation of the shock absorber, brakes, tire friction and rolling,
 *  wind drag, etc.  Also provides animation drivers for extending and
 *  retracting the gear and internal motions such as tire rotation.
 *
 *  See also GearDynamics, which contains one or more LandingGear
 *  instances and which drives the simulation of each gear.
 */
class LandingGear: public simdata::Object {
public:
	SIMDATA_OBJECT(LandingGear, 2, 0)

	BEGIN_SIMDATA_XML_INTERFACE(LandingGear)
		SIMDATA_XML("gear_name", LandingGear::m_Name, true)
		SIMDATA_XML("max_position", LandingGear::m_MaxPosition, true)
		SIMDATA_XML("motion", LandingGear::m_Motion, true)
		SIMDATA_XML("damage_limit", LandingGear::m_DamageLimit, true)
		SIMDATA_XML("K", LandingGear::m_K, true)
		SIMDATA_XML("beta", LandingGear::m_Beta, true)
		SIMDATA_XML("chain", LandingGear::m_Chained, false)
		SIMDATA_XML("brake_limit", LandingGear::m_BrakeLimit, false)
		SIMDATA_XML("brake_slip", LandingGear::m_BrakeSlip, false)
		SIMDATA_XML("tire_static_friction", LandingGear::m_TireStaticFriction, false)
		SIMDATA_XML("tire_skid_friction", LandingGear::m_TireSkidFriction, false)
		SIMDATA_XML("tire_radius", LandingGear::m_TireRadius, false)
		SIMDATA_XML("compression_limit", LandingGear::m_CompressionLimit, true)
		SIMDATA_XML("steering_limit", LandingGear::m_SteeringLimit, false)
		SIMDATA_XML("tire_K", LandingGear::m_TireK, false)
		SIMDATA_XML("tire_beta", LandingGear::m_TireBeta, false)
		SIMDATA_XML("abs", LandingGear::m_ABS, false)
		SIMDATA_XML("rolling_friction", LandingGear::m_RollingFriction, false)
		SIMDATA_XML("drag_factor", LandingGear::m_DragFactor, false)
		SIMDATA_XML("gear_animation", LandingGear::m_GearAnimation, false)
	END_SIMDATA_XML_INTERFACE

	LandingGear();
	~LandingGear();
	virtual void postCreate();

	// Each gear has a name defined in XML that is used to identify data channels
	// and animations associated with this gear.  These names are not predefined and
	// any clear identifier will do (e.g., "FrontGear" or "LeftMainGear") as long
	// as the associated animations use the same convention.
	const std::string& getName() const {return m_Name;}

	double getWheelSpeed() const { return m_TireRotationRate * m_TireRadius; }

	// Returns true if there is weight on this gear (weight-on-wheel).
	bool getWOW() const { return b_WOW->value(); }

	// Returns true if the wheel is locked and skidding in contact with the ground.
	double getSkidding() const { return m_Skidding; }

	// Returns true if anti-lock braking is actively preventing skidding.
	bool getABSActive() const { return m_ABSActiveTimer > 0.0; }

	// Returns the brake disc temperature in degrees kelvin.
	double getBrakeTemperature() const { return m_BrakeTemperature; }

	// Returns true if WOW has been true since the last call to resetTouchdown.
	bool getTouchdown() const { return m_Touchdown; }

	// Get the position of the wheel in world coordinates at the first touch
	// down since resetTouchdown was last called.
	simdata::Vector3 const &getTouchdownPoint() const { return m_TouchdownPoint; }

	// Reset the touchdown trigger, which will record the position of the wheel at the
	// next instant that WOW is true.
	void resetTouchdown() { m_Touchdown = false; }

	// Get damage associated with this gear (not yet implemented)
	double getDamage() const { return m_Damage; }

	// Get the position of the wheel in model coordinates.
	simdata::Vector3 const &getPosition() const { return m_Position; }

	// Get the position of the wheel when completely unweighted in model coordinates.
	simdata::Vector3 const &getMaxPosition() const { return m_MaxPosition; }

	// Get the displacement of the wheel (absorber compression) relative to
	// the fully unweighted position.
	simdata::Vector3 getDisplacement() const { return getPosition() - getMaxPosition(); }

	// Extend the gear and trigger the gear extension animation.
	void extend();

	// Retract the gear and trigger the gear extension animation.
	void retract();

	// Enable or disable anti-lock braking.
	void setABS(bool antiskid) { m_ABS = antiskid; }

	// Get the current steering angle in degrees.
	double getSteeringAngle() const { return m_SteeringAngle; }

	// Returns a drag coefficient between 0.0 when fully retracted and drag_factor
	// (see the XML interface) when fully extended.
	double getDragFactor() const;

	// Get the tire rotation angle in radians.
	double getTireRotation() const { return m_TireRotation; }

	// Prepare for the next dynamics calculation step.
	virtual void preSimulationStep(double dt);

	// Run additional updates and cleanup after dynamics calculation.
	// Note that origin is the model origin in local coordinates (*not* the cm origin).
	virtual void postSimulationStep(double dt, simdata::Vector3 const &origin, simdata::Vector3 const &vBody, simdata::Quat const &q, double const height, simdata::Vector3 const &normalGroundBody);

	// Compute the total force in body coordinates from this landing gear.
	// Note that origin is the model origin in local coordinates (*not* the cm origin).
	simdata::Vector3 simulateSubStep(simdata::Vector3 const &origin, simdata::Vector3 const &vBody, simdata::Quat const &q, double height, simdata::Vector3 const &normalGroundBody);

	// Drive the animation channels.  This method should be called frequently (at
	// least several times per second if not every frame) regardless of gear state.
	void updateAnimation(double dt);

	// A low cost update that is called periodically when the gear is fully retracted.
	void residualUpdate(double dt, double airspeed);

	// Export data channels to the bus.
	void registerChannels(Bus* bus);

	// Import data channels from the bus.
	void bindChannels(Bus* bus);

	// Returns true if the gear is fully retracted.
	bool isFullyExtended() const { return b_FullyExtended->value(); }
	// Returns true if the gear is fully extended.
	bool isFullyRetracted() const { return b_FullyRetracted->value(); }

	// Returns the gear extension from 0.0 when fully retracted to 1.0 when fully extended.
	double getExtension() const;

protected:
	void resetForces();
	void updateWOW(simdata::Vector3 const &origin, simdata::Quat const &q);
	void updateBraking(double dt);
	void updateBrakeTemperature(double dt, double dissipation, double airspeed);
	void updateWheel(double dt,
	                 simdata::Vector3 const &origin,
	                 simdata::Vector3 const &vBody,
	                 simdata::Quat const &q,
	                 simdata::Vector3 const &normalGroundBody,
	                 bool updateContact);
	void updateSuspension(const double dt,
	                      simdata::Vector3 const &origin,
	                      simdata::Vector3 const &vBody,
	                      simdata::Quat const &q,
	                      double const height,
	                      simdata::Vector3 const &normalGroundBody);
	void updateTireRotation(double dt);
	void updateSteeringAngle(double dt);


protected:
	simdata::Vector3 m_MaxPosition;
	simdata::Vector3 m_Motion;
	simdata::Real m_DamageLimit;

	bool m_Chained;

	double m_K;
	double m_Beta;
	double m_Compression;
	double m_CompressionLimit;
	double m_Damage;

	double m_Brake;
	double m_BrakeLimit;
	double m_BrakeFriction;
	double m_BrakeSlip;
	double m_BrakeTemperature;
	simdata::Real m_RollingFriction;

	double m_TireK;
	double m_TireBeta;
	double m_TireFriction;
	double m_TireSkidFriction;
	double m_TireStaticFriction;
	double m_TireRotationRate;
	double m_TireRotation;
	double m_TireRadius;
	simdata::Vector3 m_TireContactPoint;

	double m_SteeringAngle;
	double m_SteeringLimit;
	double m_TargetSteerAngle;
	simdata::Quat m_SteerTransform;

	// Export sensor data
	DataChannel<bool>::Ref b_FullyExtended;
	DataChannel<bool>::Ref b_FullyRetracted;
	DataChannel<bool>::Ref b_WOW;
	DataChannel<bool>::Ref b_AntilockBrakingActive;

	// Import controls
	DataChannel<double>::CRef b_BrakeCommand;
	DataChannel<double>::CRef b_SteeringCommand;

	bool m_Initialize;
	bool m_Extend;
	bool m_ABS;
	bool m_Skidding;
	bool m_SkidFlag;
	double m_ABSActiveTimer;

	double m_DragFactor;

	simdata::Vector3 m_Position;
	simdata::Vector3 m_NormalForce;
	simdata::Vector3 m_TangentForce;

	bool m_Touchdown;
	simdata::Vector3 m_TouchdownPoint;
	std::string m_Name;

	simdata::Link<GearAnimation> m_GearAnimation;
};


/** A dynamic simulation of an aircraft's landing gear.  Delegates most of the
 *  detailed simulation to child LandingGear instances.  Also acts as an receiver
 *  for input events related to the landing gear.
 */
class GearDynamics: public BaseDynamics {

	typedef simdata::Link<LandingGear>::vector GearSet;
	
	void doComplexPhysics(double x);
	void doSimplePhysics(double x);

public:
	SIMDATA_OBJECT(GearDynamics, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(GearDynamics, BaseDynamics)
		SIMDATA_XML("gear_set", GearDynamics::m_Gear, true)
	END_SIMDATA_XML_INTERFACE

	DECLARE_INPUT_INTERFACE(GearDynamics, BaseDynamics)
		BIND_ACTION("GEAR_UP", GearUp);
		BIND_ACTION("GEAR_DOWN", GearDown);
		BIND_ACTION("GEAR_TOGGLE", GearToggle);
	END_INPUT_INTERFACE

	GearDynamics();

	virtual bool allowGearUp() const;

	// Input event handlers.
	virtual void GearUp();
	virtual void GearDown();
	virtual void GearToggle();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual void getInfo(InfoList &info) const;

	// Returns true if the gear has been commanded to extend (regardless of the
	// actual state of the gear).
	bool isGearExtendSelected() const;

	// Returns true if any wheel is in contact with the ground.
	bool getWOW() const;

	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt);
	void computeForceAndMoment(double x);

protected:
	GearSet m_Gear;

	DataChannel<bool>::Ref b_GearExtendSelected;  // tracks intent, not actual state.
	DataChannel<bool>::Ref b_FullyRetracted;
	DataChannel<bool>::Ref b_FullyExtended;
	DataChannel<bool>::Ref b_WOW;
	DataChannel<bool>::CRef b_NearGround;
	DataChannel<double>::CRef b_Density;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<simdata::Vector3>::CRef b_GroundN;
	DataChannel<simdata::Vector3>::CRef b_WindVelocity;
	DataChannel<simdata::Vector3>::CRef b_CenterOfMassOffset;

	simdata::Vector3 m_CenterOfMassOffsetLocal;
	simdata::Vector3 m_GroundNormalBody;
	simdata::Vector3 m_WindVelocityBody;
	double m_Height;

	virtual double onUpdate(double dt);
};

#endif // __LANDINGGEAR_H__

