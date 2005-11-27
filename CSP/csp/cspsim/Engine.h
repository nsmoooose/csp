// Combat Simulator Project
// Copyright (C) 2002, 2005 The Combat Simulator Project
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

#ifndef __CSPSIM_ENGINE_H__
#define __CSPSIM_ENGINE_H__

#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector3.h>

#include <BaseDynamics.h>

class EngineDynamics;
class ThrustData;


/** A generic engine class.
 */
class Engine: public simdata::Object {
	simdata::Link<ThrustData> m_ThrustData;
	simdata::Vector3 m_ThrustDirection, m_EngineOffset;
	double m_LastIdleThrust;
	double m_LastMilitaryThrust;
	double m_LastAfterburnerThrust;
	double m_LastThrust;
	double m_Throttle;
	double m_Mach;
	double m_CAS;
	double m_Altitude;
	double m_Alpha;
	double m_Density;
	simdata::Vector3 m_SmokeEmitterLocation;
	friend class EngineDynamics;

	// Depending on the AOA (and mach) the engine misses air flow lowering the thrust.  The
	// following parameters approximate this in an ad-hoc way.
	double m_A, m_B, m_C;
	double flatten(double x) const;

public:
	SIMDATA_DECLARE_OBJECT(Engine)

	Engine(simdata::Vector3 const &thrustDirection = simdata::Vector3::YAXIS);

	virtual ~Engine();

	virtual void registerChannels(Bus*) { }
	virtual void importChannels(Bus*) { }

	void setThrustDirection(simdata::Vector3 const& thrustDirection);

	void setThrottle(double throttle) { m_Throttle = throttle; }

	void setConditions(double cas, double mach, double altitude, double density, double alpha) {
		m_CAS = cas;
		m_Mach = mach;
		m_Altitude = altitude;
		m_Density = density;
		m_Alpha = alpha;
	}

	double getMach() const { return m_Mach; }
	double getCAS() const { return m_CAS; }
	double getAlpha() const { return m_Alpha; }
	double getAltitude() const { return m_Altitude; }
	double getDensity() const { return m_Density; }
	double getThrottle() const { return m_Throttle; }
	double getIdleThrust() const { return m_LastIdleThrust; }
	double getMilitaryThrust() const { return m_LastMilitaryThrust; }
	double getAfterburnerThrust() const { return m_LastAfterburnerThrust; }

	virtual double getThrust() const { return m_LastThrust; };
	virtual simdata::Vector3 getThrustVector() const { return m_LastThrust * m_ThrustDirection; }

	simdata::Vector3 const &getSmokeEmitterLocation() const;

	virtual void update(double) { updateThrust(); }

protected:
	void updateThrust();

	// Returns a value in the range 0-1 to scale linearly from zero to idle thrust,
	// 1-2 scale linearly from idle to military thrust, and 2-3 to scale linearly from
	// military to afterburner thrust.
	virtual double getBlend() const;
	// An arbitary scale factor for the blended thrust.
	virtual double getThrustScale() const;
};


/** A dynamics class that computes the force and moment of one or more engines.
 */
class EngineDynamics: public BaseDynamics {
	typedef simdata::Link<Engine>::vector EngineSet;
	EngineSet m_Engine;
	DataChannel<double>::CRef b_ThrottleInput;
	DataChannel<double>::CRef b_Mach;
	DataChannel<double>::CRef b_Alpha;
	DataChannel<double>::CRef b_CAS;
	DataChannel<double>::CRef b_Density;

protected:
	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

public:
	SIMDATA_DECLARE_OBJECT(EngineDynamics)

	virtual void getInfo(InfoList &info) const;

	EngineDynamics();

	virtual void preSimulationStep(double dt);
	virtual void computeForceAndMoment(double x);

	std::vector<simdata::Vector3> getSmokeEmitterLocation() const;
};

#endif // __CSPSIM_ENGINE_H__

