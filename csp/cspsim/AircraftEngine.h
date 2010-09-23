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
 * @file AircraftEngine.h
 *
 **/

#ifndef __CSPSIM_AIRCRAFTENGINE_H__
#define __CSPSIM_AIRCRAFTENGINE_H__

#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/Vector3.h>

#include <csp/cspsim/BaseDynamics.h>
#include <csp/cspsim/Engine.h>

namespace csp {

class EngineDynamics;
class ThrustData;


/** A generic aircraft engine class.
 */
class AircraftEngine: public Engine {
	Link<ThrustData> m_ThrustData;
	Vector3 m_ThrustDirection, m_EngineOffset;
	double m_LastIdleThrust;
	double m_LastMilitaryThrust;
	double m_LastAfterburnerThrust;
	double m_LastThrust;
	double m_Mach;
	double m_CAS;
	double m_Altitude;
	double m_Alpha;
	double m_Density;
	Vector3 m_SmokeEmitterLocation;
	friend class AircraftEngineDynamics;

	/** Depending on the AOA (and mach) the engine misses air flow lowering the thrust.  The
	 * following parameters approximate this in an ad-hoc way.
	 */
	double m_A, m_B, m_C;
	double flatten(double x) const;

public:
	CSP_DECLARE_OBJECT(AircraftEngine)

	AircraftEngine(Vector3 const &thrustDirection = Vector3::YAXIS);

	virtual ~AircraftEngine();

	void setThrustDirection(Vector3 const& thrustDirection);

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
	double getIdleThrust() const { return m_LastIdleThrust; }
	double getMilitaryThrust() const { return m_LastMilitaryThrust; }
	double getAfterburnerThrust() const { return m_LastAfterburnerThrust; }

	virtual double getThrust() const { return m_LastThrust; };
	virtual Vector3 getThrustVector() const { return m_LastThrust * m_ThrustDirection; }

	Ref<SoundEffect> getEngineSound() const { return m_EngineSound; }
	Ref<SoundEffect> getAfterburnerSound () const { return m_AfterburnerSound; }

	Vector3 const &getSmokeEmitterLocation() const;

	virtual void update(double) { updateThrust(); }

protected:
	void updateThrust();

	/** Returns a value in the range 0-1 to scale linearly from zero to idle thrust,
	 * 1-2 scale linearly from idle to military thrust, and 2-3 to scale linearly from
	 * military to afterburner thrust.
	 */
	virtual double getBlend() const;
	/** An arbitary scale factor for the blended thrust. */
	virtual double getThrustScale() const;

	/** Add engine sounds to the specified sound model.  The bundle argument, if not
	 *  null, can be used to retrieve shared sound sources defined in the parent
	 *  object (typically EngineDynamics).
	 */
	virtual void bindSounds(SoundModel *model, ResourceBundle *bundle=0);

	// Engine sounds
	Ref<SoundEffect> m_EngineSound;
	Ref<SoundEffect> m_AfterburnerSound;
};


/** A dynamics class that computes the force and moment of one or more engines.
 */
class AircraftEngineDynamics: public BaseDynamics {
	typedef Link<AircraftEngine>::vector EngineSet;
	EngineSet m_Engine;
	DataChannel<double>::CRefT b_ThrottleInput;
	DataChannel<double>::CRefT b_Mach;
	DataChannel<double>::CRefT b_Alpha;
	DataChannel<double>::CRefT b_CAS;
	DataChannel<double>::CRefT b_Density;

protected:
	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

public:
	CSP_DECLARE_OBJECT(AircraftEngineDynamics)

	virtual void getInfo(InfoList &info) const;

	AircraftEngineDynamics();

	virtual void preSimulationStep(double dt);
	virtual void computeForceAndMoment(double x);

	std::vector<Vector3> getSmokeEmitterLocation() const;
};

} // namespace csp

#endif // __CSPSIM_AIRCRAFTENGINE_H__

