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


#include <SimData/InterfaceRegistry.h>
#include <SimData/Object.h>
#include <SimData/Types.h>


class LandingGear: public simdata::Object {
public:
	SIMDATA_OBJECT(LandingGear, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(LandingGear)
		SIMDATA_XML("max_position", LandingGear::m_MaxPosition, true)
		SIMDATA_XML("motion", LandingGear::m_Motion, true)
		SIMDATA_XML("damage_limit", LandingGear::m_DamageLimit, true)
		SIMDATA_XML("K", LandingGear::m_K, true)
		SIMDATA_XML("beta", LandingGear::m_Beta, true)
		SIMDATA_XML("chain", LandingGear::m_Chained, false)
		SIMDATA_XML("brake_limit", LandingGear::m_BrakeLimit, false)
		SIMDATA_XML("static_friction", LandingGear::m_StaticFriction, false)
		SIMDATA_XML("dynamic_friction", LandingGear::m_DynamicFriction, false)
		SIMDATA_XML("compression_limit", LandingGear::m_CompressionLimit, true)
		SIMDATA_XML("steering_limit", LandingGear::m_SteeringLimit, false)
		SIMDATA_XML("tire_K", LandingGear::m_TireK, false)
		SIMDATA_XML("tire_beta", LandingGear::m_TireBeta, false)
	END_SIMDATA_XML_INTERFACE

	LandingGear();
	//LandingGear(LandingGear const &);
	//LandingGear const &operator=(LandingGear const &g);
	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void postCreate();

	bool getWOW() const { return m_WOW; }
	float getSkidding() const { return m_Skidding; }
	bool getTouchdown() const { return m_Touchdown; }
	void resetTouchdown() { m_Touchdown = false; }
	double getCompression() const { return m_Compression; }
	float getDamage() const { return m_Damage; }
	bool getExtended() const { return m_Extended; }
	void setExtended(bool e) { m_Extended = e; }
	simdata::Vector3 const &getPosition() const { return m_Position; }
	simdata::Vector3 const &getMaxPosition() const { return m_MaxPosition; }

	float setSteering(float setting);
	void setBraking(float setting) { m_BrakeSetting = setting; }
	void setABS(bool antiskid) { m_ABS = antiskid; }
	float getSteeringAngle() const { return m_SteerAngle; }

	simdata::Vector3 simulate(simdata::Quaternion const &, simdata::Vector3 const &v, float h, simdata::Vector3 const &normal, float dt);

protected:
	simdata::Vector3 m_MaxPosition;
	simdata::Vector3 m_Motion;
	simdata::Spread m_DamageLimit;
	float m_K;
	float m_Beta;
	bool m_Chained;
	float m_BrakeLimit;
	float m_StaticFriction;
	float m_DynamicFriction;
	float m_CompressionLimit;
	float m_SteeringLimit;
	float m_TireK;
	float m_TireBeta;

	float m_Brake;
	float m_BrakeSetting;
	simdata::Vector3 m_Steer;
	float m_SteerAngle;
	float m_Damage;
	bool m_Extended;
	bool m_WOW;
	bool m_Touchdown;
	float m_Skidding;
	float m_TireShiftX;
	float m_TireShiftY;
	double m_Compression;
	bool m_ABS;
	simdata::Vector3 m_Position;
};


class LandingGearSet: public simdata::Object {

	typedef simdata::Pointer<LandingGear>::vector GearSet;
	
public:
	SIMDATA_OBJECT(LandingGearSet, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(LandingGearSet)
		SIMDATA_XML("gear_set", LandingGearSet::m_Gear, true)
	END_SIMDATA_XML_INTERFACE

	LandingGearSet() { m_WOW = false; }

	void doComplexPhysics(simdata::Quaternion const &orientation, 
			 simdata::Vector3 const &velocity, 
			 simdata::Vector3 const &angular_velocity, 
			 float height, 
			 simdata::Vector3 const &normal, 
			 float dt,
			 simdata::Vector3 &force,
			 simdata::Vector3 &moment) {
		m_WOW = false;
		for (unsigned i = 0; i < m_Gear.size(); i++) {
			LandingGear &gear = *(m_Gear[i]);
			simdata::Vector3 R = gear.getPosition();
			simdata::Vector3 V = velocity + (angular_velocity ^ R);
			simdata::Vector3 F = gear.simulate(orientation, V, height, normal, dt);
			if (gear.getWOW()) m_WOW = true;
			force += F;
			moment += R ^ F;
		}
	}

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_Gear);
	}

	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_Gear);
	}

//	doSimplePhysics(float dt) {}

	//FIXME: just for some testing purpose
	void setStatus(bool on) {
		size_t n = m_Gear.size();
		for (unsigned short i = 0; i < n; ++i)
			m_Gear[i]->setExtended(on);
	}

	void Retract() { 
		setStatus(false);
	}

	void Extend() {
		setStatus(true);
	}

	bool getWOW() const { return m_WOW; }

	void setBraking(float x) {
		for (unsigned i = 0; i < m_Gear.size(); i++) {
			m_Gear[i]->setBraking(x);
		}
	}

	void setSteering(float x) {
		for (unsigned i = 0; i < m_Gear.size(); i++) {
			m_Gear[i]->setSteering(x);
		}
	}

	LandingGear const *getGear(unsigned i) {
		assert(i <= m_Gear.size());
		return m_Gear[i].get();
	}

protected:
	GearSet m_Gear;
	bool m_WOW;
};



#endif // __LANDINGGEAR_H__

