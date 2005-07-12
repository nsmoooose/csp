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
	SIMDATA_DECLARE_STATIC_OBJECT(ThrustData)

	float getMil(float mach, float altitude) const;
	float getIdle(float mach, float altitude) const;
	float getAb(float mach, float altitude) const;

	ThrustData();
	virtual ~ThrustData();
};


class EngineDynamics;

class Engine: public simdata::Object {
	simdata::Link<ThrustData> m_ThrustData;
	simdata::Vector3 m_ThrustDirection, m_EngineOffset;
	float m_Throttle;
	float m_Mach;
	float m_Altitude;
	float m_EngineIdleRpm, m_EngineAbRpm;
	simdata::Vector3 m_SmokeEmitterLocation;
	friend class EngineDynamics;
public:
	SIMDATA_DECLARE_OBJECT(Engine)

	Engine(simdata::Vector3 const &thrustDirection = simdata::Vector3::YAXIS);

	virtual ~Engine();

	void setThrustDirection(simdata::Vector3 const& thrustDirection);

	void setThrottle(float throttle) { m_Throttle = throttle; }
	void setMach(float mach) { m_Mach = mach; }
	void setAltitude(float altitude) { m_Altitude = altitude; }

	simdata::Vector3 getThrust() const;
	simdata::Vector3 const &getSmokeEmitterLocation() const;
};


class EngineDynamics: public BaseDynamics {
	typedef simdata::Link<Engine>::vector EngineSet;
	EngineSet m_Engine;
	DataChannel<double>::CRef b_ThrottleInput;
	DataChannel<double>::CRef b_Mach;
	DataChannel<double>::CRef b_Alpha;

	// Depending on the AOA (and mach) the engine misses
	// air flow lowering the thrust.
	void cut();
	double flatten(double x);

	double m_A,m_B,m_C;

protected:
	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

public:
	SIMDATA_DECLARE_OBJECT(EngineDynamics)

	virtual void postCreate();

	virtual void getInfo(InfoList &info) const;

	EngineDynamics();

	virtual void preSimulationStep(double dt);
	virtual void computeForceAndMoment(double x);

	std::vector<simdata::Vector3> getSmokeEmitterLocation() const;

	virtual std::string getName() const { return "EngineDynamics"; }
};

#endif // __ENGINE_H__

