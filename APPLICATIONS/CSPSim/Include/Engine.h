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
 * @file Engine.h
 *
 **/

#ifndef __THRUST_H__
#define __THRUST_H__

#include <SimData/InterfaceRegistry.h>
#include <SimData/Interpolate.h>
#include <SimData/Object.h>
#include <SimData/Pack.h>
#include <SimData/Vector3.h>

#include "BaseDynamics.h"


class ThrustData: public simdata::Object {
	simdata::Table m_idle_thrust, m_mil_thrust, m_ab_thrust;
public:
	SIMDATA_OBJECT(ThrustData, 3, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(ThrustData)
		SIMDATA_XML("idle_thrust", ThrustData::m_idle_thrust, true)
		SIMDATA_XML("mil_thrust", ThrustData::m_mil_thrust, true)
		SIMDATA_XML("ab_thrust", ThrustData::m_ab_thrust, true)
	END_SIMDATA_XML_INTERFACE

	virtual void pack(simdata::Packer& p) const; 
    virtual void unpack(simdata::UnPacker &p); 

	float getMil(float mach, float altitude) const;
	float getIdle(float mach, float altitude) const;
	float getAb(float mach, float altitude) const;

	ThrustData();
	virtual ~ThrustData();
};


class EngineDynamics;

class Engine:public simdata::Object {
	simdata::Link<ThrustData> m_ThrustData;
	simdata::Vector3 m_ThrustDirection, m_EngineOffset;
	float const *m_Throttle;
	float m_Mach;
	float m_Altitude;
	float m_EngineIdleRpm, m_EngineAbRpm;
	simdata::Vector3 m_SmokeEmitterLocation;
	friend class EngineDynamics;
public:
	SIMDATA_OBJECT(Engine, 3, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(Engine)
		SIMDATA_XML("thrust_data", Engine::m_ThrustData, true)
		SIMDATA_XML("engine_idle_rpm", Engine::m_EngineIdleRpm, true)
		SIMDATA_XML("engine_ab_thrust", Engine::m_EngineAbRpm, true)
		SIMDATA_XML("thrust_direction", Engine::m_ThrustDirection, true)
		SIMDATA_XML("engine_offset", Engine::m_EngineOffset, true)
		SIMDATA_XML("smoke_emitter_location", Engine::m_SmokeEmitterLocation, true)
	END_SIMDATA_XML_INTERFACE

	virtual void pack(simdata::Packer& p) const; 
    virtual void unpack(simdata::UnPacker &p); 

	Engine(simdata::Vector3 const &thrustDirection = simdata::Vector3::YAXIS);

	virtual ~Engine();

	void setThrustDirection(simdata::Vector3 const& thrustDirection);
	void bindThrottle(float const &throttle);
	void setMach(float mach);
	void setAltitude(float altitude);

	simdata::Vector3 getThrust() const;
	//float getThrust(float rpm, float mach, float altitude) const;
	simdata::Vector3 const &Engine::getSmokeEmitterLocation() const;
};


class EngineDynamics:public simdata::Object, public BaseDynamics {
	typedef simdata::Link<Engine>::vector ESet;
	ESet m_Engine;
	float m_Throttle;
public:

	SIMDATA_OBJECT(EngineDynamics, 3, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(EngineDynamics)
		SIMDATA_XML("engine_set", EngineDynamics::m_Engine, true)
	END_SIMDATA_XML_INTERFACE

	virtual void pack(simdata::Packer& p) const; 
    virtual void unpack(simdata::UnPacker &p); 
	virtual void postCreate();

	EngineDynamics();
	void setThrottle(double const throttle);
	virtual void preSimulationStep(double dt);
	virtual void computeForceAndMoment(double x);
	std::vector<simdata::Vector3> getSmokeEmitterLocation() const;
};

#endif // __THRUST_H__

