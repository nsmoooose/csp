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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SimData/InterfaceRegistry.h>
#include <SimData/LUT.h>
#include <SimData/Object.h>
#include <SimData/Vector3.h>

#include <BaseDynamics.h>


class ThrustData: public simdata::Object {
	simdata::Table2 m_idle_thrust, m_mil_thrust, m_ab_thrust;
public:
	SIMDATA_STATIC_OBJECT(ThrustData, 3, 0)
	
	BEGIN_SIMDATA_XML_INTERFACE(ThrustData)
		SIMDATA_XML("idle_thrust", ThrustData::m_idle_thrust, true)
		SIMDATA_XML("mil_thrust", ThrustData::m_mil_thrust, true)
		SIMDATA_XML("ab_thrust", ThrustData::m_ab_thrust, true)
	END_SIMDATA_XML_INTERFACE

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
	float m_Throttle;
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

	Engine(simdata::Vector3 const &thrustDirection = simdata::Vector3::YAXIS);

	virtual ~Engine();

	void setThrustDirection(simdata::Vector3 const& thrustDirection);

	void setThrottle(float throttle) { m_Throttle = throttle; }
	void setMach(float mach) { m_Mach = mach; }
	void setAltitude(float altitude) { m_Altitude = altitude; }

	simdata::Vector3 getThrust() const;
	simdata::Vector3 const &Engine::getSmokeEmitterLocation() const;
};


class EngineDynamics: public BaseDynamics {
	typedef simdata::Link<Engine>::vector EngineSet;
	EngineSet m_Engine;
	DataChannel<double>::CRef b_ThrottleInput;
	DataChannel<double>::CRef b_Mach;

protected:
	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

public:

	SIMDATA_OBJECT(EngineDynamics, 3, 0)
	
	EXTEND_SIMDATA_XML_INTERFACE(EngineDynamics, BaseDynamics)
		SIMDATA_XML("engine_set", EngineDynamics::m_Engine, true)
	END_SIMDATA_XML_INTERFACE

	virtual void postCreate();

	virtual void getInfo(InfoList &info) const;

	EngineDynamics();

	virtual void preSimulationStep(double dt);
	virtual void computeForceAndMoment(double x);

	std::vector<simdata::Vector3> getSmokeEmitterLocation() const;

	virtual std::string getName() const { return "EngineDynamics"; }
};

#endif // __ENGINE_H__

