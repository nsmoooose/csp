#pragma once
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
 * @file LandingGear.h
 *
 **/

#include <csp/cspsim/BaseDynamics.h>

#include <csp/csplib/data/Link.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Vector3.h>

#include <vector>

namespace csp {

class GearAnimation;


/** LandingGear encapsulates a single landing gear.  Provides physical
 *  simulation of the shock absorber, brakes, tire friction and rolling,
 *  wind drag, etc.  Also provides animation drivers for extending and
 *  retracting the gear and internal motions such as tire rotation.
 *
 *  See also GearDynamics, which contains one or more LandingGear
 *  instances and which drives the simulation of each gear.
 */
class LandingGear: public Object {
public:
	CSP_DECLARE_OBJECT(LandingGear)

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

	// If getTouchdown() is true, returns a flag that indicates if the wheel
	// skidded on touchdown.  Skidding is assumed to happen if the wheel speed
	// is significantly different from the ground speed, or if the vertical
	// velocity at touchdown was high.
	bool getTouchdownSkid() const { return m_TouchdownSkid; }

	// If getTouchdown() is true, returns the vertical velocity at touchdown
	// in m/s.
	double getTouchdownVerticalVelocity() const { return m_TouchdownVerticalVelocity; }

	// Get the position of the wheel in world coordinates at the first touch
	// down since resetTouchdown was last called.
	Vector3 const &getTouchdownPoint() const { return m_TouchdownPoint; }

	// Reset the touchdown trigger, which will record the position of the wheel at the
	// next instant that WOW is true.
	void resetTouchdown() { m_Touchdown = false; }

	// Get damage associated with this gear (not yet implemented)
	double getDamage() const { return m_Damage; }

	// Get the position of the wheel in model coordinates.
	Vector3 const &getPosition() const { return m_Position; }

	// Get the position of the wheel when completely unweighted in model coordinates.
	Vector3 const &getMaxPosition() const { return m_MaxPosition; }

	// Get the displacement of the wheel (absorber compression) relative to
	// the fully unweighted position.
	Vector3 getDisplacement() const { return getPosition() - getMaxPosition(); }

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
	virtual void postSimulationStep(double dt, Vector3 const &origin, Vector3 const &vBody, Quat const &q, double const height, Vector3 const &normalGroundBody);

	// Compute the total force in body coordinates from this landing gear.
	// Note that origin is the model origin in local coordinates (*not* the cm origin).
	Vector3 simulateSubStep(Vector3 const &origin, Vector3 const &vBody, Quat const &q, double height, Vector3 const &normalGroundBody);

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

	// bind a SoundModel and a ResourceBundle to the gear
	void bindSounds(SoundModel *model, ResourceBundle *bundle=0);

	Ref<SoundEffect> getTouchdownSound() const { return m_TouchdownSound; }

protected:
	void resetForces();
	void updateWOW(Vector3 const &origin, Quat const &q, Vector3 const &vBody, Vector3 const &normalGroundBody);
	void updateBraking(double dt);
	void updateBrakeTemperature(double dt, double dissipation, double airspeed);
	void updateWheel(double dt,
	                 Vector3 const &origin,
	                 Vector3 const &vBody,
	                 Quat const &q,
	                 Vector3 const &normalGroundBody,
	                 bool updateContact);
	void updateSuspension(const double dt,
	                      Vector3 const &origin,
	                      Vector3 const &vBody,
	                      Quat const &q,
	                      double const height,
	                      Vector3 const &normalGroundBody);
	void updateTireRotation(double dt);
	void updateSteeringAngle(double dt);


protected:
	Vector3 m_MaxPosition;
	Vector3 m_Motion;
	Real m_DamageLimit;

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
	Real m_RollingFriction;

	double m_TireK;
	double m_TireBeta;
	double m_TireFriction;
	double m_TireSkidFriction;
	double m_TireStaticFriction;
	double m_TireRotationRate;
	double m_TireRotation;
	double m_TireRadius;
	Vector3 m_TireContactPoint;

	double m_SteeringAngle;
	double m_SteeringLimit;
	double m_TargetSteerAngle;
	Quat m_SteerTransform;

	// Export sensor data
	DataChannel<bool>::RefT b_FullyExtended;
	DataChannel<bool>::RefT b_FullyRetracted;
	DataChannel<bool>::RefT b_WOW;
	DataChannel<bool>::RefT b_AntilockBrakingActive;

	// Import controls
	DataChannel<double>::CRefT b_BrakeCommand;
	DataChannel<double>::CRefT b_SteeringCommand;

	bool m_Initialize;
	bool m_Extend;
	bool m_ABS;
	bool m_Skidding;
	bool m_SkidFlag;
	double m_ABSActiveTimer;

	double m_DragFactor;

	Vector3 m_Position;
	Vector3 m_NormalForce;
	Vector3 m_TangentForce;

	bool m_Touchdown;
	bool m_TouchdownSkid;
	double m_TouchdownVerticalVelocity;
	Vector3 m_TouchdownPoint;
	std::string m_Name;

	Link<GearAnimation> m_GearAnimation;

	// Sounds
	Ref<SoundEffect> m_TouchdownSound;
};


/** A dynamic simulation of an aircraft's landing gear.  Delegates most of the
 *  detailed simulation to child LandingGear instances.  Also acts as an receiver
 *  for input events related to the landing gear.
 */
class GearDynamics: public BaseDynamics {
	typedef Link<LandingGear>::vector GearSet;

public:
	CSP_DECLARE_OBJECT(GearDynamics)

	DECLARE_INPUT_INTERFACE(GearDynamics, BaseDynamics)
		BIND_ACTION("GEAR_UP", GearUp);
		BIND_ACTION("GEAR_DOWN", GearDown);
		BIND_ACTION("GEAR_TOGGLE", GearToggle);
	END_INPUT_INTERFACE  // protected:

public:
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

	DataChannel<bool>::RefT b_GearCommand;
	DataChannel<bool>::RefT b_GearExtendSelected;  // tracks intent, not actual state.
	DataChannel<bool>::RefT b_FullyRetracted;
	DataChannel<bool>::RefT b_FullyExtended;
	DataChannel<bool>::RefT b_WOW;
	DataChannel<bool>::CRefT b_NearGround;
	DataChannel<double>::CRefT b_Density;
	DataChannel<double>::CRefT b_GroundZ;
	DataChannel<Vector3>::CRefT b_GroundN;
	DataChannel<Vector3>::CRefT b_WindVelocity;
	DataChannel<Vector3>::CRefT b_CenterOfMassOffset;

	Vector3 m_CenterOfMassOffsetLocal;
	Vector3 m_GroundNormalBody;
	Vector3 m_WindVelocityBody;
	double m_Height;

	virtual double onUpdate(double dt);

private:
	// Get the model position in global coordinates.  This is the physical
	// location of the aircraft model origin, as opposed to m_Position which
	// tracks the center of mass.
	Vector3 getModelPosition() const {
		return *m_Position - fromBody(b_CenterOfMassOffset->value());
	}

	void doComplexPhysics(double x);
	void doSimplePhysics(double x);
	void onGearCommand();
};

} // namespace csp
