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
#include "Animation.h"

class GearSequenceAnimation;
class GearStructureAnimation;


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
		SIMDATA_XML("brake_steering_linkage", LandingGear::m_BrakeSteeringLinkage, false)
		SIMDATA_XML("drag_factor", LandingGear::m_DragFactor, false)
		SIMDATA_XML("gear_sequence_animation", LandingGear::m_GearSequenceAnimation, false)
		SIMDATA_XML("gear_structure_animation", LandingGear::m_GearStructureAnimation, false)
	END_SIMDATA_XML_INTERFACE

	LandingGear();
	virtual void postCreate();

	// Each gear has a name defined in XML that is used to identify data channels
	// and animations associated with this gear.  These names are not predefined and
	// any clear identifier will do (e.g., "FrontGear" or "LeftMainGear") as long
	// as the associated animations use the same convention.
	const std::string& getName() const {return m_Name;}

	// Returns true if there is weight on this gear (weight-on-wheel).
	bool getWOW() const { return m_WOW; }

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

	// Get the position of the wheel in body coordinates.
	simdata::Vector3 const &getPosition() const { return m_Position; }

	// Get the position of the wheel when completely unweighted.
	simdata::Vector3 const &getMaxPosition() const { return m_MaxPosition; }

	// Get the displacement of the wheel (absorber compression) relative to
	// the fully unweighted position.
	simdata::Vector3 getDisplacement() const { return getPosition() - getMaxPosition(); }

	// Extend the gear and trigger the gear extension animation.
	void extend();

	// Retract the gear and trigger the gear extension animation.
	void retract();

	// Set the steering angle (in radians).  The link_brakes parameter is used to
	// drive the brakes on non-steering wheels.  Braking is set to link_brakes times
	// the value of brake_steering_linkage (see the XML interface), where 1.0 is full
	// braking and values less than zero release the brake.
	double setSteering(double setting, double link_brakes);

	// Set the braking value, where 1.0 is full braking and 0.0 is fully released.
	void setBraking(double setting);

	// Enable or disable anti-lock braking.
	void setABS(bool antiskid) { m_ABS = antiskid; }

	// Get the current steering angle (as set by setSteering).
	double getSteeringAngle() const { return m_SteerAngle; }

	// Returns a drag coefficient between 0.0 when fully retracted and drag_factor
	// (see the XML interface) when fully extended.
	double getDragFactor() const;

	// Get the tire rotation angle in radians.
	double getTireRotation() const { return m_TireRotation; }

	// dynamics interface
	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt, simdata::Vector3 const &origin, simdata::Vector3 const &vBody, simdata::Quat const &q, double const height, simdata::Vector3 const &normalGroundBody);
	simdata::Vector3 simulateSubStep(simdata::Vector3 const &origin, simdata::Vector3 const &vBody, simdata::Quat const &q, double height, simdata::Vector3 const &normalGroundBody);

	// Drive the animation channels.  This method should be called frequently (at
	// least several times per second if not every frame) regardless of gear state.
	void updateAnimations(double dt);

	// A low cost update that is called periodically when the gear is fully retracted.
	void residualUpdate(double dt, double airspeed);

	// Export data channels to the bus.
	void registerChannels(Bus* bus);

	// Returns true if the gear is fully retracted.
	bool isFullyExtended() const { return m_FullyExtended; }
	// Returns true if the gear is fully extended.
	bool isFullyRetracted() const { return m_FullyRetracted; }

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
	void updateSuspension(simdata::Vector3 const &origin,
                              simdata::Vector3 const &vBody,
                              simdata::Quat const &q,
                              double const height,
                              simdata::Vector3 const &normalGroundBody);
	void updateTireRotation(double dt);


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
	double m_BrakeSetting;
	double m_BrakeFriction;
	double m_BrakeSlip;
	double m_BrakeSteeringLinkage;
	double m_BrakeSteer;
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

	double m_SteerAngle;
	double m_SteeringLimit;
	simdata::Quat m_SteerTransform;

	bool m_FullyExtended;
	bool m_FullyRetracted;
	bool m_Extend;
	bool m_WOW;
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

	simdata::Link<GearSequenceAnimation> m_GearSequenceAnimation;
	simdata::Link<GearStructureAnimation> m_GearStructureAnimation;
};


/** A TimedSequence adapter for driving the animation that extends and retracts
 *  the landing gear.  Each LandingGear uses an instance of this class.
 *
 *  Exported channels:
 *    LandingGear.<gearname>.NormalizedTime
 *    LandingGear.<gearname>.ReferenceTime
 */
class GearSequenceAnimation: public TimedSequence {
public:
	SIMDATA_OBJECT(GearSequenceAnimation, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(GearSequenceAnimation, TimedSequence)
	END_SIMDATA_XML_INTERFACE
	virtual void setGearName(const std::string &name) {
		m_GearName = name;
		setNameIfEmpty(name);
	}
	virtual void update(double dt) { onUpdate(dt); }
	virtual double getExtension() const { return getNormalizedTime(); }
	virtual void extend() { setDirection(1.0); }
	virtual void retract() { setDirection(-1.0); }
protected:
	std::string m_GearName;
};


/**
 * Abstract base class for the animation of a gear's internal structure.  Mainly
 * reflects suspension and tire rotation.
 * The 'm2k' subclass shows how to do the work "manually" by connecting to individual
 * animation channels to drive the various moving parts, while the 'path' subclass
 * shows how to use a TimedSequence to drive an animation path defined in the model
 * that coordinates the motion of the various parts.
 */
class GearStructureAnimation: public simdata::Object {
public:
	BEGIN_SIMDATA_XML_VIRTUAL_INTERFACE(GearStructureAnimation)
		// TODO shouldn't this be taken from the 'motion' field in LandingGear?
		SIMDATA_XML("displacement_axis", GearStructureAnimation::m_DisplacementAxis, false)
	END_SIMDATA_XML_INTERFACE

	GearStructureAnimation(): m_DisplacementAxis(simdata::Vector3::ZAXIS) { }
	virtual ~GearStructureAnimation() { }

	// Called by LandingGear to update the animation.  Displacement is the motion of the wheel
	// relative to its unweighted position in body coordinates.  Tire rotation is the rotation
	// angle in radians.  dt is the elapsed time since the last update in seconds.
	virtual void update(const simdata::Vector3 &displacement, double tire_rotation, double dt)=0;

	// Extend in subclasses to register output channels for driving animations.
	virtual void registerChannels(Bus*) { }

	// Called by LandingGear to set the gear name used for exproted channels.
	virtual void setGearName(const std::string &name) { m_GearName = name; }

	// Get the name of the associated LandingGear.
	virtual std::string const &getGearName() const { return m_GearName; }

protected:
	virtual void postCreate() {
		m_DisplacementAxis.normalized();
	}

	// Internal parameters.
	simdata::Vector3 m_DisplacementAxis;
	std::string m_GearName;
};


/** An implementation of the GearStructureAnimation interface that manually drives the
 *  animations of the various moving parts involved in shock absorber compression and
 *  tire rotation.  The parts are fairly specific to the M2k.  This approach is
 *  deprecated for new models, which should define animation paths to synchonize the
 *  motion of the various parts.  See DefinedGearStructureAnimation for details.
 *
 *  Exported channels:
 *    LandingGear.<gearname>Absorber02
 *    LandingGear.<gearname>Absorber03
 *    LandingGear.<gearname>Displacement
 *    LandingGear.<gearname>TireRotation
 */
class M2kGearStructureAnimation: public GearStructureAnimation {
	// Internal parameters
	float m_Absorber02Length, m_Absorber03Length, m_Offset;

	// Channels to be exposed to drive animations.
	DataChannel<double>::Ref b_Absorber02Angle;
	DataChannel<double>::Ref b_Absorber03Angle;
	DataChannel<double>::Ref b_TireRotation;
	DataChannel<simdata::Vector3>::Ref b_Displacement;

public:
	SIMDATA_OBJECT(M2kGearStructureAnimation, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(M2kGearStructureAnimation, GearStructureAnimation)
		SIMDATA_XML("absorber02_length", M2kGearStructureAnimation::m_Absorber02Length, false)
		SIMDATA_XML("absorber03_length", M2kGearStructureAnimation::m_Absorber03Length, false)
		SIMDATA_XML("offset", M2kGearStructureAnimation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	M2kGearStructureAnimation(): m_Absorber02Length(1.0f), m_Absorber03Length(1.0f), m_Offset(0.0f) { }

	virtual ~M2kGearStructureAnimation(){ }

	// Update the channels that drive the animated parts.
	virtual void update(const simdata::Vector3 &displacement, double tire_rotation, double /*dt*/) {
		double vertical_displacement = displacement * m_DisplacementAxis;
		b_Absorber02Angle->value() = asin((vertical_displacement-m_Offset)/(2*m_Absorber02Length));
		b_Absorber03Angle->value() = asin((vertical_displacement-m_Offset)/(2*m_Absorber03Length));
		b_TireRotation->value() = tire_rotation;
		b_Displacement->value() = displacement;
	}

	// Register the various channels that drive the animated parts.
	virtual void registerChannels(Bus* bus) {
		const std::string prefix = "LandingGear." + getGearName();
		b_Absorber02Angle = bus->registerLocalDataChannel<double>(prefix + "Absorber02", 0.0);
		b_Absorber03Angle = bus->registerLocalDataChannel<double>(prefix + "Absorber03", 0.0);
		b_Displacement = bus->registerLocalDataChannel<simdata::Vector3>(prefix + "Displacement", simdata::Vector3::ZERO);
		b_TireRotation = bus->registerLocalDataChannel<double>(prefix + "TireRotation", 0.0);
	}
};


/**
 */
class DefinedGearStructureAnimation: public GearStructureAnimation {
	double m_DisplacementLength;
	simdata::Link<TimedSequence> m_DisplacementSequence;
	simdata::Link<TimedSequence> m_TireRotationSequence;
	DataChannel<double>::Ref b_TireRotation;
public:
	SIMDATA_OBJECT(DefinedGearStructureAnimation, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(DefinedGearStructureAnimation, GearStructureAnimation)
		SIMDATA_XML("displacement_length", DefinedGearStructureAnimation::m_DisplacementLength, true)
		SIMDATA_XML("displacement_sequence", DefinedGearStructureAnimation::m_DisplacementSequence, true)
		SIMDATA_XML("tire_rotation_sequence", DefinedGearStructureAnimation::m_TireRotationSequence, false)
	END_SIMDATA_XML_INTERFACE

	DefinedGearStructureAnimation(): m_DisplacementLength(0.1) { }
	virtual ~DefinedGearStructureAnimation(){ }

	// Update the channels that drive the animated parts.
	virtual void update(const simdata::Vector3 &displacement, double tire_rotation, double dt) {
		if (m_DisplacementSequence.valid()) {
			double normalized_time = displacement.length() / m_DisplacementLength;
			m_DisplacementSequence->setNormalizedTime(normalized_time);
		}
		if (m_TireRotationSequence.valid()) {
			m_TireRotationSequence->setNormalizedTime(tire_rotation * (0.5 / simdata::PI));
		} else {
			assert(b_TireRotation.valid());
			b_TireRotation->value() = tire_rotation;
		}
	}

	// Register the various channels that drive the animated parts.
	virtual void registerChannels(Bus* bus) {
		const std::string prefix = "LandingGear." + getGearName();
		if (m_DisplacementSequence.valid()) {
			m_DisplacementSequence->setNameIfEmpty(prefix);
			m_DisplacementSequence->registerChannels(bus);
		}
		if (m_TireRotationSequence.valid()) {
			m_TireRotationSequence->setNameIfEmpty(prefix);
			m_TireRotationSequence->registerChannels(bus);
		} else {
			b_TireRotation = bus->registerLocalDataChannel<double>(prefix + "TireRotation", 0.0);
		}
	}
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

	// Sets the braking of the left and right wheels, where 1.0 is full braking and
	// 0.0 is full release.
	void setBraking(double left, double right);

	// Set the steering angle in radians.  If link_brakes is 1.0 any landing gear
	// marked for linked braking will apply or release brake pressure to assist in
	// steering.
	void setSteering(double x, double link_brakes=1.0);

	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt);
	void computeForceAndMoment(double x);

protected:
	GearSet m_Gear;

	DataChannel<bool>::Ref b_GearExtendSelected;  // tracks intent, not actual state.
	DataChannel<bool>::Ref b_WOW;
	DataChannel<bool>::CRef b_NearGround;
	DataChannel<double>::CRef b_LeftBrakeInput;
	DataChannel<double>::CRef b_RightBrakeInput;
	DataChannel<double>::CRef b_SteeringInput;
	DataChannel<double>::CRef b_Density;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<simdata::Vector3>::CRef b_GroundN;
	DataChannel<simdata::Vector3>::CRef b_WindVelocity;

	simdata::Vector3 m_GroundNormalBody;
	simdata::Vector3 m_WindVelocityBody;
	double m_Height;
	bool m_FullyRetracted;

	virtual double onUpdate(double dt);
};

#endif // __LANDINGGEAR_H__

