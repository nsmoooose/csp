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
 * @file F16Engine.cpp
 *
 **/


#include <F16/F16Engine.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <FuelManagementSystem.h>

CSP_XML_BEGIN(F16Engine)
	CSP_DEF("wind_spin", m_WindSpin, false)
	CSP_DEF("drag_rate", m_DragRate, false)
	CSP_DEF("fan_turbine_inlet__heat_rate", m_FanTurbineInletHeatRate, false)
	CSP_DEF("fan_turbine_inlet_cool_rate", m_FanTurbineInletCoolRate, false)
	CSP_DEF("fan_turbine_inlet_temperature_base", m_FanTurbineInletTemperatureBase, false)
	CSP_DEF("fan_turbine_inlet_temperature_scale", m_FanTurbineInletTemperatureScale, false)
	CSP_DEF("core_fan_turbine_inlet_temperature_ratio", m_CoreFanTurbineInletTemperatureRatio, false)
	CSP_DEF("core_heat_rate", m_CoreHeatRate, false)
	CSP_DEF("core_cool_rate", m_CoreCoolRate, false)
	CSP_DEF("exhaust_heat_rate", m_ExhaustHeatRate, false)
	CSP_DEF("exhaust_cool_rate", m_ExhaustCoolRate, false)
	CSP_DEF("friction", m_Friction, false)
	CSP_DEF("spindown_tau", m_SpinDownTau, false)
	CSP_DEF("spool_rate", m_SpoolRate, false)
	CSP_DEF("abnormal_start_chance", m_AbnormalStartChance, false)
	CSP_DEF("hot_delta", m_HotDelta, false)
	CSP_DEF("overheat", m_Overheat, false)
	CSP_DEF("idle_rpm", m_IdleRPM, false)
	CSP_DEF("afterburner_rpm", m_AfterburnerRPM, false)
	CSP_DEF("afterburner_delay", m_AfterburnerDelay, false)
	CSP_DEF("afterburner_min_rpm", m_AfterburnerMinRPM, false)
	CSP_DEF("afterburner_cutoff_rpm", m_AfterburnerCutoffRPM, false)
	CSP_DEF("idle_fuel_consumption", m_IdleFuelConsumption, false)
	CSP_DEF("min_ftit_ignition", m_MinFTITIgnition, false)
	CSP_DEF("nozzle_idle_factor", m_NozzleIdleFactor, false)
	CSP_DEF("nozzle_base", m_NozzleBase, false)
	CSP_DEF("jfs_off_rpm", m_JFSOffRPM, false)
	CSP_DEF("start_drive", m_StartDrive, false)
	CSP_DEF("normal_hang_time", m_NormalHangTime, false)
	CSP_DEF("abnormal_hang_time", m_AbnormalHangTime, false)
	CSP_DEF("hot_start_branch", m_HotStartBranch, false)
	CSP_DEF("jfs_drive_factor", m_JFSDriveFactor, false)
	CSP_DEF("fuel_pressurization_rate", m_FuelPressurizationRate, false)
	CSP_DEF("fuel_depressurization_rate", m_FuelDepressurizationRate, false)
CSP_XML_END


F16Engine::F16Engine():
	m_Status(STATUS_OFF),
	m_Drive(0.0),
	m_StartRPM(0.0),
	m_CoreTemperature(20.0),
	m_ExhaustTemperature(20.0),
	m_CoreTemperatureTarget(20.0),
	m_ExhaustTemperatureTarget(20.0),
	m_FanTurbineInletTemperatureTarget(20.0),
	m_BurnTime(0.0),
	m_Leanness(0.0),
	m_NoStartTime(0.0),
	m_AfterburnerTime(-1.0),
	m_Afterburner(false),
	m_AfterburnerFailure(false),
	m_ThrustSpecificFuelConsumption(1.0),
	m_FuelConsumption(0.0),
	m_HotStartTemperature(0.0),
	m_FuelPressure(0.0),
	m_StartElapsedTime(0.0),
	m_HangTime(0.0),
	m_Blend(0.0),
	m_NormalStart(true)
{
	// TODO set defaults
	m_WindSpin.set(0.45e-5f, 0.01e-5f);
	m_DragRate.set(0.03f, 0.001f);
	m_FanTurbineInletHeatRate.set(0.30f, 0.001f);
	m_FanTurbineInletCoolRate.set(0.20f, 0.001f);
	m_FanTurbineInletTemperatureBase.set(560.0f, 10.0f);
	m_FanTurbineInletTemperatureScale.set(800.0f, 80.0f);
	m_CoreFanTurbineInletTemperatureRatio.set(0.70f, 0.01f);
	m_CoreHeatRate.set(0.03f, 0.001f);
	m_CoreCoolRate.set(0.004f, 0.001f);
	m_ExhaustHeatRate.set(0.50f, 0.01f);
	m_ExhaustCoolRate.set(0.20f, 0.01f);
	m_Friction.set(0.0001f, 0.000005f);
	m_SpinDownTau.set(6.0f, 0.01f);
	m_SpoolRate.set(4.0f, 0.01f);
	m_AbnormalStartChance.set(0.01f, 0.001f);
	m_HotDelta.set(500.0f, 10.0f);
	m_Overheat.set(1000.0f, 20.0f);
	m_IdleRPM.set(0.68f, 0.01f);
	m_AfterburnerRPM.set(1.05f, 0.001f);
	m_AfterburnerDelay.set(0.25f, 0.01f);
	m_AfterburnerMinRPM.set(0.95f, 0.005f);
	m_AfterburnerCutoffRPM.set(1.005f, 0.0001f);
	m_IdleFuelConsumption.set(318.0f, 5.0f);
	m_MinFTITIgnition.set(220.0f, 5.0f);
	m_NozzleIdleFactor.set(0.2f, 0.005f);
	m_NozzleBase.set(0.45f, 0.01f);
	m_JFSOffRPM.set(0.50f, 0.01f);
	m_StartDrive.set(0.2100f, 0.0001f);
	m_DriveSpeed.set(1.4f, 0.01f);
	m_NormalHangTime.set(120.0f, 10.0f);
	m_AbnormalHangTime.set(70.0f, 30.0f);
	m_HotStartBranch.set(0.70f, 0.05f);
	m_JFSDriveFactor.set(0.365f, 0.01f);
	m_FuelPressurizationRate.set(0.15f, 0.005f);
	m_FuelDepressurizationRate.set(0.5f, 0.01f);
}

F16Engine::~F16Engine() { }

void F16Engine::postCreate() {
	Object::postCreate();
	regen();
}

void F16Engine::regen() {
	m_WindSpin.regen();
	m_DragRate.regen();
	m_FanTurbineInletHeatRate.regen();
	m_FanTurbineInletCoolRate.regen();
	m_FanTurbineInletTemperatureBase.regen();
	m_FanTurbineInletTemperatureScale.regen();
	m_CoreFanTurbineInletTemperatureRatio.regen();
	m_CoreHeatRate.regen();
	m_CoreCoolRate.regen();
	m_ExhaustHeatRate.regen();
	m_ExhaustCoolRate.regen();
	m_Friction.regen();
	m_SpinDownTau.regen();
	m_SpoolRate.regen();
	m_AbnormalStartChance.regen();
	m_HotDelta.regen();
	m_Overheat.regen();
	m_IdleRPM.regen();
	m_AfterburnerRPM.regen();
	m_AfterburnerDelay.regen();
	m_AfterburnerMinRPM.regen();
	m_AfterburnerCutoffRPM.regen();
	m_IdleFuelConsumption.regen();
	m_MinFTITIgnition.regen();
	m_NozzleIdleFactor.regen();
	m_NozzleBase.regen();
	m_JFSOffRPM.regen();
	m_StartDrive.regen();
	m_NormalHangTime.regen();
	m_AbnormalHangTime.regen();
	m_HotStartBranch.regen();
	m_JFSDriveFactor.regen();
	m_FuelPressurizationRate.regen();
	m_FuelDepressurizationRate.regen();
}

// Initialize to running.
void F16Engine::jumpStart() {
	m_Status = STATUS_RUNNING;
	b_RPM->value() = m_IdleRPM;
	m_ExhaustTemperature = 600.0;
	b_FanTurbineInletTemperature->value() = 500.0;
	m_CoreTemperature = 400.0;
	b_Nozzle->value() = 1.0;
	m_FuelPressure = 1.0;
	m_Blend = 1.0;
	setAfterburner(false);
}

void F16Engine::registerChannels(Bus* bus) {
	Engine::registerChannels(bus);
	b_FuelFlow = bus->registerLocalDataChannel<double>("Engine.FuelFlow", 0.0);
	b_RPM = bus->registerLocalDataChannel<double>("Engine.RPM", 0.0);
	b_FanTurbineInletTemperature = bus->registerLocalDataChannel<double>("Engine.FTIT", 20.0);
	b_Nozzle = bus->registerLocalDataChannel<double>("Engine.Nozzle", 1.0);
}

void F16Engine::importChannels(Bus* bus) {
	Engine::importChannels(bus);
	b_FuelManagementSystem = bus->getChannel("FuelManagementSystem");
	jumpStart();
}

void F16Engine::update(double dt) {
	double fdt = static_cast<double>(dt);
	// jfs->update(fdt);
	updateTemperature(fdt);
	switch (m_Status) {
		case STATUS_OFF:
			updateOff(fdt);
			break;
		case STATUS_PRESTART:
			updatePrestart(fdt);
			break;
		case STATUS_RUNNING:
			updateRunning(fdt);
			break;
		case STATUS_STALL:
			//updateStall(fdt);
			break;
		case STATUS_FIRE:
			//updateFire(fdt);
			break;
		case STATUS_FAIL:
			//updateFail(fdt);
			break;
	}
	updateNozzle(fdt);
	updateFuel(fdt);
	updateThrust();
}

void F16Engine::updateTemperature(double dt) {
	{
		double dT = m_CoreTemperatureTarget - m_CoreTemperature;
		double rate = (dT >= 0.0) ? m_CoreHeatRate : m_CoreCoolRate;
		m_CoreTemperature += dT * rate * dt;
	}
	{
		double cas = getCAS();
		double target = m_FanTurbineInletTemperatureTarget;
		if (target > 600.0) target = 600.0 + (target - 600.0) * 1000.0 / (1000.0 + cas);
		target = std::max(target, m_CoreTemperatureTarget);
		double dT = target - b_FanTurbineInletTemperature->value();
		double rate = (dT >= 0.0) ? m_FanTurbineInletHeatRate.value() : m_FanTurbineInletCoolRate.value() * (1.0 + cas * 0.005);
		b_FanTurbineInletTemperature->value() += dT * rate * dt;
	}
	checkOverheat(dt);
}


void F16Engine::checkOverheat(double) {
	//double dT = b_FanTurbineInletTemperature->value() - m_Overheat;
	// TODO check fail/fire
}

void F16Engine::updateTemperatureTargets() {
	const double base = m_FanTurbineInletTemperatureBase + m_HotStartTemperature;
	m_FanTurbineInletTemperatureTarget = std::max(20.0, base + (b_RPM->value() - m_IdleRPM) * m_FanTurbineInletTemperatureScale);
	m_CoreTemperatureTarget = m_FanTurbineInletTemperatureTarget * m_CoreFanTurbineInletTemperatureRatio;
}

// TODO fix simdata random interface to provide obvious functions like this!
double gauss(double mean, double sigma) {
	return static_cast<double>(simdata::rd::BoxMueller(simdata::g_Random, mean, sigma));
}
double randomUnit() {
	return static_cast<double>(simdata::g_Random.unit());
}

void F16Engine::updateOff(double dt) {
	driveEngine(0.0, dt);
	const double throttle = getEffectiveThrottle();
	updateFuelConsumption((throttle >= 0.0) ? 0.1 : 0.0, dt);
	if (throttle >= 0.0 && isFuelPressurized()) {
		m_HotStartTemperature = 0.0;
		m_Status = STATUS_PRESTART;
		m_IdleRPM.regen();
		m_StartRPM = m_IdleRPM * std::min(0.98, gauss(0.95, 0.05));
		m_Drive = m_StartDrive;
		m_StartElapsedTime = 0.0;
		m_NormalStart = true;
		if (randomUnit() - m_AbnormalStartChance < 0.0) {
			m_HangTime = m_AbnormalHangTime.regen();
		} else {
			m_HangTime = m_NormalHangTime.regen();
		}
	}
}

void F16Engine::updatePrestart(double dt) {
	if (isFuelDepressurized()) {
		shutdown();
		return;
	}
	const double rpm = b_RPM->value();
	const double rpm_factor = powf(rpm, 2.1);
	const double drive = m_Drive * rpm_factor * getAltitudeFactor();
	driveEngine(drive, dt);
	// TODO jfs
	if (m_StartElapsedTime > m_HangTime && m_NormalStart) {
		m_NormalStart = false;
		m_Drive /= (1.0 + 0.40 * std::max(0.1, rpm));  // ad-hoc drive reduction to prevent rpm increase
		if (randomUnit() < m_HotStartBranch) {
			m_HotStartTemperature = m_HotDelta.regen();
		} // else hung start
	}
	m_StartElapsedTime += dt;
	if (rpm >= m_IdleRPM || rpm >= m_StartRPM && getThrottle() > 0.0) {
		if (b_FanTurbineInletTemperature->value() >= m_MinFTITIgnition) {
			m_Status = STATUS_RUNNING;
		}
	}
	updateFuelConsumption(m_IdleFuelConsumption * std::max(0.05, rpm / m_IdleRPM), dt);
	updateTemperatureTargets();
}

void F16Engine::updateRunning(double dt) {
	if (isFuelDepressurized()) {
		shutdown();
		return;
	}
	const double rpm = b_RPM->value();
	const double thrust = getThrust();
	if (rpm < m_IdleRPM) {
		m_FuelConsumption = m_IdleFuelConsumption * (rpm / m_IdleRPM);
		setAfterburner(false);
	} else if (m_Afterburner) {
		updateFuelConsumption(m_IdleFuelConsumption + std::max(0.0, thrust * m_ThrustSpecificFuelConsumption), dt);
	} else {
		const double dthrust = getMilitaryThrust() - getIdleThrust();
		const double thrust_scale = (dthrust > 0) ? getMilitaryThrust() / dthrust : 0.0;
		const double effective_thrust = (thrust - getIdleThrust()) * thrust_scale;
		updateFuelConsumption(m_IdleFuelConsumption + std::max(0.0, effective_thrust * m_ThrustSpecificFuelConsumption), dt);
	}
	const double throttle = getEffectiveThrottle();
	/*
	double target = 1.0;
	if (throttle <= 0.95) {
		const double f = throttle / 0.95;
		target = m_IdleRPM * (1.0 - f) + f;
		if (rpm <= m_AfterburnerCutoffRPM) setAfterburner(false);
	} else if (!m_AfterburnerFailure) {
		const double f = (throttle - 0.95) * 20.0;
		if (m_Afterburner) {
			target = (1.0 - f) + f * m_AfterburnerRPM;
		} else {
			if (rpm >= m_AfterburnerMinRPM) {
				if (m_AfterburnerTime <= 0.0) m_AfterburnerTime = m_AfterburnerDelay;
				m_AfterburnerTime -= dt;
				if (m_AfterburnerTime <= 0.0) setAfterburner(true);
			}
		}
	} else {
		setAfterburner(false);
	}
	updateTemperatureTargets();
	b_RPM->value() = rpm + (target - rpm) * m_SpoolRate * dt;
	*/
	double p1 = throttle * (0.625) + std::max(0.0, (throttle - 0.8) * 1.875);
	static double p3 = 0.0;  // XXX
	double r = 0.0;
	if (p1 < 0.5) {
		if (p3 < 0.5) {
			r = (p1 - p3) * simdata::clampTo(2.0 - (p1-p3)*4.0, 0.1, 1.0);
		} else {
			r = 5.0 * (0.4 - p3);
		}
	} else {
		if (p3 < 0.5) {
			r = (0.6 - p3) * simdata::clampTo(2.0 - (0.6-p3)*4.0, 0.1, 1.0);
		} else {
			r = 5.0 * (p1 - p3);
		}
	}
	p3 += r * dt;
	setAfterburner(p3 >= 0.5);
	updateTemperatureTargets();
	b_RPM->value() = (p3 < 0.5) ? m_IdleRPM * (1.0 - 2*p3) + 2*p3 : (1.0 - p3)*2 + m_AfterburnerRPM * (2*p3 - 1.0);
}

void F16Engine::driveEngine(double drive, double dt) {
	drive += getCAS() * m_WindSpin;
	// TODO jfs drive
	const double rpm = b_RPM->value();
	const double spin_drag = (getAltitudeFactor() / m_SpinDownTau) * rpm * rpm + m_Friction;
	drive -= spin_drag;
	b_RPM->value() = std::max(0.0, rpm + m_DriveSpeed * drive * dt);
}

void F16Engine::updateFuelConsumption(double target, double dt) {
	m_FuelConsumption += (target - m_FuelConsumption) * 0.5 * dt;
	if (target > 1e+10) { char *x = 0; *x = 1; }
	b_FuelFlow->value() = m_FuelConsumption;
}

void F16Engine::shutdown() {
	m_Status = STATUS_OFF;
	m_NoStartTime = 0.0;
	m_FanTurbineInletTemperatureTarget = 20.0;
	m_CoreTemperatureTarget = 20.0;
	setAfterburner(false);
}

void F16Engine::setAfterburner(bool on) {
	m_Afterburner = on;
	// thrust is in newtons; scale so that fuel consumption is kg/hr.
	// TODO document units explicitly.
	m_ThrustSpecificFuelConsumption = (on ? 2.05 : 0.74) / 9.8;
}

void F16Engine::updateFuel(double dt) {
	// FIXME fuel consumption should be measured (internally) in kg/s, with appropriate conversions in other
	// places.
	static const double kg_per_hr_to_liters_per_sec = 3.47e-4;  // assumes 0.8 kg/l
	double need = m_FuelConsumption * kg_per_hr_to_liters_per_sec * dt;
	double fuel = !b_FuelManagementSystem ? 0.0 : b_FuelManagementSystem->value()->drawFuel(dt, need);
	if (getThrottle() < 0.0) {
		fuel = 0.0;
		need = 1.0;
	}
	if (fuel < need * 0.999) {
		double rate = (1.0 - fuel / need) * m_FuelDepressurizationRate;
		m_FuelPressure = std::max(0.0, m_FuelPressure - rate * dt);
	} else {
		const double rate = m_FuelPressurizationRate;
		m_FuelPressure = std::min(1.0, m_FuelPressure + rate * dt);
	}
}

void F16Engine::updateNozzle(double) {
	double nozzle_factor = 1.0;
	const double rpm = b_RPM->value();
	if (rpm < m_IdleRPM) {
		double factor = rpm / m_IdleRPM;
		m_Blend = factor;
		nozzle_factor = std::min(1.0, m_NozzleIdleFactor * factor + (1.0 - factor) * 2.0);
	} else if (rpm <= 1.0) {
		double power = (rpm - m_IdleRPM) / (1.0 - m_IdleRPM);
		m_Blend = 1.0 + power;
		nozzle_factor = m_NozzleIdleFactor * simdata::clampTo(1.0 - power, 0.0, 1.0);
	} else {
		double power = 1.0 + (rpm - 1.0) / (m_AfterburnerRPM - 1.0);
		m_Blend = 1.0 + power;
		nozzle_factor = simdata::clampTo(power - 1.0, 0.0, 1.0);
	}
	b_Nozzle->value() = m_NozzleBase + (1.0 - m_NozzleBase) * nozzle_factor;
}


double F16Engine::getEffectiveThrottle() {
	return getThrottle() * m_FuelPressure;
}

bool F16Engine::isFuelPressurized() {
	return m_FuelPressure > 0.95;
}

bool F16Engine::isFuelDepressurized() {
	return m_FuelPressure < 0.50;
}

double F16Engine::getAltitudeFactor() {
	return getDensity() * 0.78125;  // approx 1.0 at sea level
}
