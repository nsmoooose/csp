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
#include <SimData/Object.h>
#include <SimData/Vector3.h>
#include <SimData/Quat.h>
#include <SimData/Real.h>
#include <SimData/Link.h>

#include "BaseDynamics.h"

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
	END_SIMDATA_XML_INTERFACE

	LandingGear();
	//LandingGear(LandingGear const &);
	//LandingGear const &operator=(LandingGear const &g);
	virtual void postCreate();
	const std::string& getName() const {return m_Name;}
	bool getWOW() const { return m_WOW; }
	double getSkidding() const { return m_Skidding; }
	double getBrakeTemperature() const { return m_BrakeTemperature; }
	bool getTouchdown() const { return m_Touchdown; }
	simdata::Vector3 const &getTouchdownPoint() const { return m_TouchdownPoint; }
	void resetTouchdown() { m_Touchdown = false; }
	double getDamage() const { return m_Damage; }
	bool getExtended() const { return m_Extended; }
	void setExtended(bool e) { m_Extended = e; }
	simdata::Vector3 const &getPosition() const { return m_Position; }
	simdata::Vector3 const &getMaxPosition() const { return m_MaxPosition; }

	double setSteering(double setting, double link_brakes);
	void setBraking(double setting);
	void setABS(bool antiskid) { m_ABS = antiskid; }
	double getSteeringAngle() const { return m_SteerAngle; }
	double getDragFactor() const;
	double getTireRotation() const { return m_TireRotation; }

	// dynamics interface
	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt,
                                    simdata::Vector3 const &origin,
                                    simdata::Vector3 const &vBody,
                                    simdata::Quat const &q,
                                    double const height,
                                    simdata::Vector3 const &normalGroundBody);
	simdata::Vector3 simulateSubStep(simdata::Vector3 const &origin,
                                     simdata::Vector3 const &vBody,
                                     simdata::Quat const &q,
                                     double height,
                                     simdata::Vector3 const &normalGroundBody);

	void residualUpdate(double dt, double airspeed);

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
	double m_RollingFriction;

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

	bool m_Extended;
	bool m_WOW;
	bool m_ABS;
	bool m_Skidding;
	bool m_SkidFlag;

	simdata::Vector3 m_Position;
	simdata::Vector3 m_NormalForce;
	simdata::Vector3 m_TangentForce;

	bool m_Touchdown;
	simdata::Vector3 m_TouchdownPoint;
	std::string m_Name;
};

class GearAnimation: public simdata::Object {
	// name of the animated gear 
	std::string m_GearName;

	// Channels' names.
	std::string m_Displacement;
	std::string m_Absorber02, m_Absorber03;

	// Channels to be exposed to drive animations.
	DataChannel<double>::Ref b_WheelRotation;
	DataChannel<double>::Ref b_Absorber02Angle, b_Absorber03Angle;

	// Internal parameters.
	float m_Absorber02Length, m_Absorber03Length, m_Offset;
	simdata::Vector3 m_DisplacementAxis;

	// Channel to import.
	DataChannel<simdata::Vector3>::CRef b_GearDisplacement;
	DataChannel<double>::CRef b_GearTireRotation;

public:
	SIMDATA_OBJECT(GearAnimation, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(GearAnimation)
		SIMDATA_XML("displacement_channel", GearAnimation::m_Displacement, true)
		SIMDATA_XML("displacement_axis", GearAnimation::m_DisplacementAxis, false)
		SIMDATA_XML("absorber02_channel", GearAnimation::m_Absorber02, false)
		SIMDATA_XML("absorber03_channel", GearAnimation::m_Absorber03, false)
		SIMDATA_XML("absorber02_length", GearAnimation::m_Absorber02Length, false)
		SIMDATA_XML("absorber03_length", GearAnimation::m_Absorber03Length, false)
		SIMDATA_XML("offset", GearAnimation::m_Offset, false)
	END_SIMDATA_XML_INTERFACE

	GearAnimation():
		m_Displacement(""),
		m_DisplacementAxis(simdata::Vector3::ZAXIS),
		m_Absorber02Length(1.0f),
		m_Absorber03Length(1.0f),
		m_Offset(0.0f) {
	}
		virtual double onUpdate(double dt) {
			double vertical_displacement = b_GearDisplacement->value()*m_DisplacementAxis;
			//XXX special hack for Lologramme ;)
			b_WheelRotation->value() = (1.0-dt)*b_WheelRotation->value() + dt*b_GearTireRotation->value();
			b_Absorber02Angle->value() = asin((vertical_displacement-m_Offset)/(2*m_Absorber02Length));
			b_Absorber03Angle->value() = asin((vertical_displacement-m_Offset)/(2*m_Absorber03Length));
			return 0.016;
		}
		virtual void registerChannels(Bus::Ref bus) {
			bus->registerChannel(b_WheelRotation.get());
			bus->registerChannel(b_Absorber02Angle.get());
			bus->registerChannel(b_Absorber03Angle.get());
		}
		virtual void bindChannels(Bus::Ref bus) {
			b_GearDisplacement = bus->getChannel(m_Displacement);
			if (!b_GearDisplacement.valid()) {
				CSP_LOG(OBJECT, WARNING, "GearAnimation: input channel '" << m_Displacement << "' unavailable."); 
			}
			b_GearTireRotation = bus->getChannel(m_GearName + "TireRotation");
			if (!b_GearTireRotation.valid()) {
				CSP_LOG(OBJECT, WARNING, "GearAnimation: input channel '" << m_GearName + "TireRotation" << "' unavailable."); 
			}
		}

		// Helper classes
		class BindChannels {
			Bus::Ref m_Bus;
		public:
			BindChannels(Bus::Ref bus):m_Bus(bus){ }
			void operator()(simdata::Link<GearAnimation>& ga) const {
				ga->bindChannels(m_Bus);
			}
		};
		class RegisterChannels {
			Bus::Ref m_Bus;
		public:
			RegisterChannels(Bus::Ref bus):m_Bus(bus){ }
			void operator()(simdata::Link<GearAnimation>& ga) const {
				ga->registerChannels(m_Bus);
			}
		};
		class OnUpdate {
			double m_dt;
		public:
			OnUpdate(double dt):m_dt(dt){ }
			void operator()(simdata::Link<GearAnimation>& ga) const {
				ga->onUpdate(m_dt);
			}
		};
protected:
	virtual void postCreate() {
		m_DisplacementAxis.normalized();
		m_GearName = m_Displacement.substr(0,m_Displacement.length()-std::string("Displacement").length());
		CSP_LOG(OBJECT, DEBUG, "GearAnimmation: m_GearName = " << m_GearName);
		std::string wheel_rotation = m_GearName + "WheelRotation";
		b_WheelRotation = DataChannel<double>::newLocal(wheel_rotation, 0.0);
		b_Absorber02Angle = DataChannel<double>::newLocal(m_Absorber02, 0.0);
		b_Absorber03Angle = DataChannel<double>::newLocal(m_Absorber03, 0.0);
	}
};

class GearDynamics: public BaseDynamics {

	typedef simdata::Link<LandingGear>::vector GearSet;
	
	void doComplexPhysics(double x);
	void doSimplePhysics(double x);

	void setGearExtension(double on);

	typedef simdata::Link<GearAnimation>::vector GearSetAnimation;
	GearSetAnimation m_GearSetAnimation;

public:
	SIMDATA_OBJECT(GearDynamics, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(GearDynamics, BaseDynamics)
		SIMDATA_XML("gear_set", GearDynamics::m_Gear, true)
		SIMDATA_XML("gear_set_animation", GearDynamics::m_GearSetAnimation, false)
	END_SIMDATA_XML_INTERFACE

	DECLARE_INPUT_INTERFACE(GearDynamics, BaseDynamics)
		BIND_ACTION("GEAR_UP", GearUp);
		BIND_ACTION("GEAR_DOWN", GearDown);
		BIND_ACTION("GEAR_TOGGLE", GearToggle);
	END_INPUT_INTERFACE

	// input event handlers
	void GearUp();
	void GearDown();
	void GearToggle();

	GearDynamics();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual void getInfo(InfoList &info) const;

	bool isGearExtended() const;
	bool getWOW() const;

	void setBraking(double left, double right);
	void setSteering(double x, double link_brakes=1.0);

	LandingGear const *getGear(size_t i);
	size_t getGearNumber() const;
	simdata::Vector3 getGearDisplacement(size_t i) const;

	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt);
	void computeForceAndMoment(double x);

protected:
	GearSet m_Gear;

	// gear animation
	DataChannel<double>::Ref b_GearExtension;
	std::vector<DataChannel<simdata::Vector3>::Ref> b_GearDisplacement;
	std::vector<DataChannel<double>::Ref> b_TireRotation;

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

	virtual double onUpdate(double dt);
};


#endif // __LANDINGGEAR_H__

