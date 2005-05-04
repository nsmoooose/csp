// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file GearAnimation.h
 *
 **/


#ifndef __GEAR_ANIMATION_H__
#define __GEAR_ANIMATION_H__


#include "Bus.h"
#include "AnimationSequence.h"
#include <SimData/InterfaceRegistry.h>
#include <SimData/Link.h>
#include <SimData/Math.h>
#include <SimData/Object.h>
#include <SimData/Vector3.h>


/** Base class for driving landing gear animations.  Controls retract/extend
 *  motion, shock absorber compression, tire rotation, and steering.
 */
class GearAnimation: public simdata::Object {
public:
	virtual void setGearName(const std::string &name) { m_GearName = name; m_Prefix = "LandingGear." + name; }
	std::string const &getGearName() const { return m_GearName; }

	// Called by LandingGear to update the animation.
	// @param compression The normalized motion of the wheel along the shock absorber compression axis
	//   relative to its unweighted position.
	virtual void setCompression(double compression) { compression = 0; /*avoid warning*/ }

	// Called by LandingGear to update the animation.
	// @param tire rotation The tire rotation angle in radians.
	virtual void setTireRotation(double tire_rotation) { tire_rotation = 0; /*avoid warning*/ }

	// Called by LandingGear to update the animation.
	// @param steering_angle The rotation of the wheel around the compression axis (positive
	//   for turning to the right).
	virtual void setSteeringAngle(double steering_angle) { steering_angle = 0; /*avoid warning*/ }

	// Called by LandingGear to update the animation.
	// @param dt The elapsed time since the last update in seconds.
	virtual void update(double dt)=0;

	// Called by LandingGear to allow subclasses to register output channels for driving animations.
	virtual void registerChannels(Bus*) { }

	// Called by LandingGear to allow subclasses to import channels.
	virtual void bindChannels(Bus*) { }

	virtual void setCompressionMotion(simdata::Vector3 const &axis, double limit) {
		m_CompressionAxis = axis.normalized();
		m_CompressionScale = limit;
	}

	simdata::Vector3 const &getCompressionAxis() const { return m_CompressionAxis; }
	double getCompressionScale() const { return m_CompressionScale; }

	virtual double getExtension() const = 0;
	virtual bool isFullyExtended() const = 0;
	virtual bool isFullyRetracted() const = 0;
	virtual void extend() = 0;
	virtual void retract() = 0;
	virtual void forceExtend() = 0;

protected:
	virtual std::string const &prefix() const { return m_Prefix; }

private:
	std::string m_GearName;
	std::string m_Prefix;
	simdata::Vector3 m_CompressionAxis;
	double m_CompressionScale;
};


/** A GearAnimation subclass that uses animation paths (via TimedSequence)
 *  to raise and lower the gear and to drive the landing gear compression.
 *  This allows complex animation sequences to be defined in the 3D model
 *  and driven easily by the simulation.  Tire rotation and steering animations
 *  are handled by simple driven rotations.
 *
 *  Exported channels:
 *    LandingGear.<gearname>.RetractCompress
 *    LandingGear.<gearname>.TireRotation
 *    LandingGear.<gearname>.SteeringAngle
 */
class GearSequenceAnimation: public GearAnimation {
public:
	SIMDATA_OBJECT(GearSequenceAnimation, 0, 0)
	BEGIN_SIMDATA_XML_INTERFACE(GearSequenceAnimation)
		SIMDATA_XML("retract_sequence", GearSequenceAnimation::m_RetractSequence, false)
		SIMDATA_XML("compression_sequence", GearSequenceAnimation::m_CompressionSequence, false)
	END_SIMDATA_XML_INTERFACE

	virtual void setGearName(std::string const &name);

	virtual void update(double dt) {
		if (m_RetractSequence.valid()) m_RetractSequence->update(dt);
	}

	virtual void setCompression(double compression) {
		if (m_CompressionSequence.valid()) m_CompressionSequence->setValue(compression);
	}

	virtual void setTireRotation(double tire_rotation) { b_TireRotation->value() = tire_rotation; }
	virtual void setSteeringAngle(double steering_angle) { b_SteeringAngle->value() = steering_angle; }

	virtual double getExtension() const { return !m_RetractSequence ? 1.0 : m_RetractSequence->normalizedKey(); }
	virtual bool isFullyExtended() const { return !m_RetractSequence ? true : m_RetractSequence->atEnd(); }
	virtual bool isFullyRetracted() const { return !m_RetractSequence ? false : m_RetractSequence->atStart(); }
	virtual void extend() { if (m_RetractSequence.valid()) m_RetractSequence->playForward(); }
	virtual void retract() { if (m_RetractSequence.valid()) m_RetractSequence->playBackward(); }
	virtual void forceExtend();

	// Called by LandingGear to allow subclasses to register output channels for driving animations.
	virtual void registerChannels(Bus* bus);

private:
	simdata::Link<TimedSequence> m_RetractSequence;
	simdata::Link<DrivenSequence> m_CompressionSequence;
	DataChannel<double>::Ref b_TireRotation;
	DataChannel<double>::Ref b_SteeringAngle;
};


/** A specialization of GearSequenceAnimation that manually drives the animations of the
 *  various moving parts involved in shock absorber compression and tire rotation.  The
 *  parts are fairly specific to the M2k.  This approach is deprecated for new models,
 *  which should define animation paths to synchonize the motion of the various parts.
 *
 *  Exported channels:
 *    LandingGear.<gearname>.Absorber02
 *    LandingGear.<gearname>.Absorber03
 *    LandingGear.<gearname>.Compression
 */
class M2kGearAnimation: public GearSequenceAnimation {
	// Internal parameters
	float m_Absorber02Length, m_Absorber03Length, m_Offset;

	// Channels to be exposed to drive animations.
	DataChannel<double>::Ref b_Absorber02Angle;
	DataChannel<double>::Ref b_Absorber03Angle;
	DataChannel<double>::Ref b_Compression;

public:
	SIMDATA_OBJECT(M2kGearAnimation, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(M2kGearAnimation, GearSequenceAnimation)
		SIMDATA_XML("absorber02_length", M2kGearAnimation::m_Absorber02Length, false)
		SIMDATA_XML("absorber03_length", M2kGearAnimation::m_Absorber03Length, false)
		SIMDATA_XML("offset", M2kGearAnimation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	M2kGearAnimation(): m_Absorber02Length(1.0f), m_Absorber03Length(1.0f), m_Offset(0.0f) { }

	virtual void setCompression(double compression);

	// Register the various channels that drive the animated parts.
	virtual void registerChannels(Bus* bus);
};


#endif // __GEAR_ANIMATION_H__

