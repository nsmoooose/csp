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

class LandingGear: public simdata::Object {
public:
	SIMDATA_OBJECT(LandingGear, 2, 0)

	BEGIN_SIMDATA_XML_INTERFACE(LandingGear)
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
	virtual void serialize(simdata::Archive&);
	virtual void postCreate();

	bool getWOW() const { return m_WOW; }
	double getSkidding() const { return m_Skidding; }
	bool getTouchdown() const { return m_Touchdown; }
	simdata::Vector3 const &getTouchdownPoint() const { return m_TouchdownPoint; }
	void resetTouchdown() { m_Touchdown = false; }
	double getCompression() const { return m_Compression; }
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

protected:
	void resetForces();
	void updateWOW(simdata::Vector3 const &origin, simdata::Quat const &q);
	void updateBraking(double dt);
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
};


#endif // __LANDINGGEAR_H__

