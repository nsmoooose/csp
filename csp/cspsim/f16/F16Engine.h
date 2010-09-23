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
 * @file F16Engine.h
 *
 **/


#ifndef __CSPSIM_F16_F16ENGINE_H__
#define __CSPSIM_F16_F16ENGINE_H__

#include <csp/cspsim/Engine.h>
#include <csp/cspsim/FuelManagementSystem.h>  /** @todo only need FuelManagementInterface */

#include <csp/csplib/data/Real.h>

namespace csp {

class F16Engine: public Engine {
public:
	CSP_DECLARE_OBJECT(F16Engine)

	F16Engine();
	virtual ~F16Engine();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

protected:
	virtual void update(double dt);
	virtual void postCreate();
	virtual double getBlend() const { return m_Blend; }

private:
	typedef enum { STATUS_OFF, STATUS_PRESTART, STATUS_RUNNING, STATUS_STALL, STATUS_FIRE, STATUS_FAIL } EngineStatus;
	EngineStatus m_Status;

	void jumpStart();
	void updateTemperature(double dt);
	void updateTemperatureTargets();
	void checkOverheat(double dt);
	void updateOff(double dt);
	void updatePrestart(double dt);
	void updateRunning(double  dt);
	void driveEngine(double drive, double dt);
	void updateFuelConsumption(double target, double dt);
	void shutdown();
	void setAfterburner(bool on);
	void updateFuel(double dt);
	void updateNozzle(double dt);
	void spoolEngine(double dt);
	double getEffectiveThrottle();
	bool isFuelPressurized();
	bool isFuelDepressurized();
	double getAltitudeFactor();

	// state variables
	double m_Drive;
	DataChannel<double>::RefT b_RPM;   /** @todo export @code DataChannel<double>::RefT b_RPM; @endcode */
	double m_StartRPM;
	double m_CoreTemperature;
	double m_ExhaustTemperature;   /** @todo export @code double m_ExhaustTemperature; @endcode */
	DataChannel<double>::RefT b_FanTurbineInletTemperature;   // TODO export
	double m_CoreTemperatureTarget;
	double m_ExhaustTemperatureTarget;
	double m_FanTurbineInletTemperatureTarget;
	double m_BurnTime;
	double m_Leanness;
	double m_NoStartTime;
	double m_AfterburnerTime;
	bool m_Afterburner;
	bool m_AfterburnerFailure;
	double m_ThrustSpecificFuelConsumption;
	double m_FuelConsumption;   /** @todo export @code double m_FuelConsumption; @endcode */
	double m_HotStartTemperature;
	DataChannel<double>::RefT b_Nozzle;   /** @todo export @code DataChannel<double>::RefT b_Nozzle; @endcode */
	double m_FuelPressure;
	double m_StartElapsedTime;
	double m_HangTime;
	double m_Blend;
	double m_Power;
	bool m_NormalStart;

	DataChannel< Ref<FuelManagementSystem> >::CRefT b_FuelManagementSystem;
	DataChannel<double>::RefT b_FuelFlow;

	void regen();
	Real m_WindSpin;
	Real m_DragRate;
	Real m_FanTurbineInletHeatRate;
	Real m_FanTurbineInletCoolRate;
	Real m_FanTurbineInletTemperatureBase;
	Real m_FanTurbineInletTemperatureScale;
	Real m_CoreFanTurbineInletTemperatureRatio;
	Real m_CoreHeatRate;
	Real m_CoreCoolRate;
	Real m_ExhaustHeatRate;
	Real m_ExhaustCoolRate;
	Real m_Friction;
	Real m_SpinDownTau;
	Real m_SpoolRate;
	Real m_AbnormalStartChance;
	Real m_HotDelta;
	Real m_Overheat;
	Real m_IdleRPM;
	Real m_AfterburnerRPM;
	Real m_AfterburnerDelay;
	Real m_AfterburnerMinRPM;
	Real m_AfterburnerCutoffRPM;
	Real m_IdleFuelConsumption;
	Real m_MinFTITIgnition;
	Real m_NozzleIdleFactor;
	Real m_NozzleBase;
	Real m_JFSOffRPM;
	Real m_StartDrive;
	Real m_DriveSpeed;
	Real m_NormalHangTime;
	Real m_AbnormalHangTime;
	Real m_HotStartBranch;
	Real m_JFSDriveFactor;
	Real m_FuelPressurizationRate;
	Real m_FuelDepressurizationRate;
};

} // namespace csp

#endif // __CSPSIM_F16_F16ENGINE_H__

