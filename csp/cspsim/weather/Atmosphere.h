// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file Atmosphere.h
 *
 **/

#ifndef __CSPSIM_ATMOSPHERE_H__
#define __CSPSIM_ATMOSPHERE_H__

#include <csp/csplib/data/Date.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/Vector3.h>

#include <vector>

CSP_NAMESPACE

namespace weather {

/**
 * class Atmosphere
 *
 * This class implements a primitive atmospheric model based on the 1976
 * Standard Atmosphere.  Dynamic variables such as temperature and pressure
 * are modelled using a combination of physical effects (e.g. season and sun
 * elevation) and hand-tailored noise functions.  The latter are also used to
 * generate winds, gusts, and turbulence.  A number of utility functions such
 * as mach number and calibrated airspeed are also implemented.
 *
 * Wind is currently modelled as an average prevailing wind plus a 2D random
 * fluctuation that cycles roughly every 15 minutes.  To this wind we add a
 * a 2D noise function that varies with altitude.  Finally, the wind vector
 * is modulated by a factor that depends on the rate of change of the sea
 * level pressure (as a crude approximation of storm fronts) and a gust
 * factor, which is yet another noise function that cycles every 5 minutes.
 *
 * Turbulence is handled separately and must be added to the wind value
 * described above.  Turbulence combines altitude dependent noise functions
 * that morph over time with a 1D noise function indexed by distance travelled.
 */
class Atmosphere { //: public Object {

public:
	Atmosphere();

	/** Get the gravitational acceleration (m/s^2) as a function of altitude (m).
	 */
	double getGravity(double /*alt*/) const { return 9.806; /* sufficient for now */ }

	/** Air temperature (K) as a function of altitude (m).
	 *  From 1976 Standard Atmosphere, below 32000 m
	 */
	float getTemperature(double alt) const;

	/** Air pressure (Pa) as a function of altitude (m).
	 *  From 1976 Standard Atmosphere, below 32000 m
	 */
	double getPressure(double alt) const;

	/** Air density (kg/m^3) as a function of altitude (m).
	 *  From 1976 Standard Atmosphere, below 32000 m
	 */
	double getDensity(double alt) const;

	/** Get the current wind velocity at the specified position (in global
	 *  coordinates).
	 */
	Vector3 getWind(Vector3 const &position) const;

	/** Get the turbulence at the specified position (altitude).  Distance
	 *  is the linear position in the turbulence field.  The caller should
	 *  update this value as the vehicle moves according to dist = dist +
	 *  |v| * dt.  Turbulence should be added to the value returned by
	 *  getWind() to compute the instantaneous local airflow.
	 */
	Vector3 getTurbulence(Vector3 const &position, double dist) const;

	/** Set the position in latitude, longitude coordinates, which together
	 *  with the date determines sun elevation.
	 */
	void setPosition(double lat, double lon);

	/** Set the average wind speed (m/s) and direction.
	 */
	void setPrevailingWind(Vector3 const &);
	
	/** Set the date, which is used to compute seasonal temperature effects.
	 */
	void setDate(SimDate const &);

	void reset();

	/** Update the atmospheric conditions (pressure, winds, etc).  Should be
	 *  called at least once per second on average (3 Hz is about optimal).
	 */
	void update(double dt);

	/** Get the mach number (multiple of the speed of sound) for the given
	 *  speed (m/s) and altitude (m).  Uses the 1976 Standard Atmosphere.
	 */
	inline double getMach(double speed, double altitude) const;

	/** Get the speed of sound in m/s at the specified altitude (m).  Uses
	 *  the 1976 Standard Atmosphere.
	 */
	float getSpeedOfSound(double altitude) const;

	/** Get the calibrated airspeed for the specified mach number and altitude
	 *  (m) in m/s.
	 */
	float getPreciseCAS(double mach, double altitude) const;

	/** Get an approximate value for the calibrated airspeed using a lookup
	 *  table.  This is considerably faster than getPreciseCAS, although the
	 *  difference is probably not significant in most cases.
	 */
	float getCAS(double mach, double altitude) const;
	
protected:
	bool fastUpdate(double &dt);
	void slowUpdate();

	void generateWinds();
	void tabulateCAS();

	double m_Latitude;
	double m_Longitude;
	double m_TargetTemperature;
	double m_GroundTemperature;
	double m_TargetPressure;
	double m_GroundPressure;
	double m_GroundDensity;
	double m_TimeScale;
	double m_WindIndex;
	double m_WindScale;
	double m_FastUpdate;
	double m_UpdateTime;
	int m_UpdateIndex;
	SimDate m_Date;
	Vector3 m_PrevailingWind;
	Vector3 m_AverageWind;
	Vector3 m_TargetWind;
	std::vector<float> m_PressureTime;
	std::vector<float> m_WindTimeX;
	std::vector<float> m_WindTimeY;
	std::vector<float> m_WindAltX;
	std::vector<float> m_WindAltY;
	std::vector<float> m_GustTime;
	std::vector<float> m_TurbulenceX;
	std::vector<float> m_TurbulenceY;
	std::vector<float> m_TurbulenceZ;
	std::vector<float> m_TurbulenceAltA;
	std::vector<float> m_TurbulenceAltB;
	Table2 m_CAS;
	double m_TurbulenceBlend;
	bool m_TurbulenceBlendUp;
	double m_GustModulation;
	double m_GustIndex;
};

inline double Atmosphere::getMach(double speed, double alt) const {
	return speed / getSpeedOfSound(alt);
}

inline float Atmosphere::getCAS(double mach, double altitude) const {
	return m_CAS[static_cast<float>(mach)][static_cast<float>(altitude)];
}

} // end namespace weather

CSP_NAMESPACE_END

#endif // __CSPSIM_ATMOSPHERE_H__

