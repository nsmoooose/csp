// Combat Simulator Project
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
 * @file JetFuelStarter.cpp
 *
 **/


#include <csp/cspsim/f16/JetFuelStarter.h>
#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/Random.h>
#include <csp/csplib/data/ObjectInterface.h>

namespace csp {

CSP_XML_BEGIN(JetFuelStarter)
	CSP_DEF("power", m_Power, true)
	CSP_DEF("drive_rate", m_DriveRate, true)
	CSP_DEF("start_chance", m_StartChance, true)
	CSP_DEF("heat_rate", m_HeatRate, true)
	CSP_DEF("cool_rate", m_CoolRate, true)
	CSP_DEF("shutdown_temperature", m_ShutdownTemp, true)
	CSP_DEF("spinup_rate", m_SpinupRate, true)
	CSP_DEF("spindown_rate", m_SpindownRate, true)
	CSP_DEF("max_fuel_flow", m_MaxFuelFlow, true)
CSP_XML_END


JetFuelStarter::JetFuelStarter():
	m_Enabled(false),
	m_Temperature(0.0),
	m_Drive(0.0),
	m_Power(0.0294),
	m_DriveRate(0.03),
	m_StartChance(0.999),
	m_HeatRate(0.01),
	m_CoolRate(0.01),
	m_ShutdownTemp(0.85),
	m_SpinupRate(0.2),
	m_SpindownRate(0.8),
	m_MaxFuelFlow(0.0184) { // l/s (JFS 100-13 burns 117 lb/hr at max hp)
}

double JetFuelStarter::getFuelDemand(double dt) const {
	return m_Enabled ? std::max(0.0, m_MaxFuelFlow * m_Drive * dt) : 0.0;
}

void JetFuelStarter::update(double dt, double fuel_received) {
	if (m_Enabled) {
		const double fuel_needed = getFuelDemand(dt);
		if (fuel_received < fuel_needed) {
			deactivate();
			return;
		}
		double spool_rate = (1.0 - m_Drive) * m_SpinupRate;
		m_Drive = std::min(1.0, m_Drive + spool_rate * dt);
		double heat_rate = (1.0 - m_Temperature) * m_HeatRate;
		m_Temperature = std::min(1.0, m_Temperature + heat_rate * dt);
		if (m_Temperature > m_ShutdownTemp) {
			deactivate();
		}
	} else {
		if (m_Temperature > 0.0) {
			double heat_rate = -m_Temperature * m_CoolRate;
			m_Temperature = std::max(0.0, m_Temperature + heat_rate * dt);
		}
		if (m_Drive > 0.0) {
			double spool_rate = -m_Drive * m_SpindownRate;
			m_Drive = std::max(0.0, m_DriveRate + spool_rate * dt);
		}
	}
}

/*  TODO accumulators should be managed elsewhere (e.g. by EngineDynamics) since
 *  they have to be available to multiple systems.
updateAccumulators(dt, engine_rpm);
void JetFuelStarter::updateAccumulators(double dt, double engine_rpm) {
	if (m_Accumulator1 + m_Accumulator2 < 2.0) {
		double factor = clampTo((engine_rpm - m_AccumulatorRechargeRpm) * m_AccumulatorRechargeFactor, 0.0, 1.0);
		double recharge = factor * m_AccumulatorRechargeRate * dt;
		if (m_Accumulator1 < 1.0 && m_Accumulator2 < 1.0) recharge *= 0.5;
		m_Accumulator1 = std::min(1.0, m_Accumulator1 + recharge);
		m_Accumulator2 = std::min(1.0, m_Accumulator2 + recharge);
	}
}
*/

// Note that the caller is responsible for draining the accumulator.
void JetFuelStarter::start(double accumulator, double altitude, double airspeed) {
	// ignite doesn't check for available fuel, but if fuel isn't available the first
	// update will shut the jfs down.
	if (ignite(accumulator, altitude, airspeed)) {
		enable(true);
		// init drive to a negative value to simulate startup time
		m_Drive = accumulator * 0.5 - 1.0;
	}
}

static double decay(double x, double scale) { return 1.0 / (1.0 + x * x / (scale * scale)); }

bool JetFuelStarter::ignite(double accumulator, double altitude, double airspeed) {
	// ad-hoc dependence on accumulator pressure.  best start chance at 95% charge
	// or above, with steep drop off to no chance below 63% charge.
	double sq = accumulator * accumulator;
	double start_chance = clampTo(1.5 * (sq * sq * sq - 0.0625), 0.0, 1.0);
	start_chance *= m_StartChance;

	// rapid drop above T = 0.4 (0.5 @ T = 0.43)
	double temperature_factor = std::max(0.0, m_Temperature - 0.4);
	start_chance *= decay(temperature_factor, 0.03);

	// quasi ad-hoc altitude and airspeed dependence based on recommended JFS start
	// conditions in MCI 11-F16, vol. 3, 21 April 1995.

	// reduced chance above 6000m (~ 20000 ft).
	double altitude_factor = std::max(0.0, altitude - 6000.0);
	start_chance *= decay(altitude_factor, 2000.0);

	// reduced chance above 400 kias.
	double airspeed_kias = convert::mps_kts(airspeed);
	double airspeed_factor = std::max(0.0, airspeed_kias - 400.0);
	start_chance *= decay(airspeed_factor, 50.0);

	double dice = g_Random.unit();  // FIXME use thread-specific rng
	return (dice < start_chance);
}

void JetFuelStarter::deactivate() {
	enable(false);
}

double JetFuelStarter::getDrive() const {
	return std::max(0.0, m_Drive) * m_Power;
}

bool JetFuelStarter::isActive() const {
	return m_Enabled && m_Drive > 0.0;
}

bool JetFuelStarter::isEnabled() const {
	return m_Enabled;
}

void JetFuelStarter::enable(bool on) {
	m_Enabled = on;
}

} // namespace csp

