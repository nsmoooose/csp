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

#include <Engine.h>
#include <csp/csplib/data/Real.h>
#include <FuelManagementSystem.h>  // FIXME only need FuelManagementInterface


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
	double getEffectiveThrottle();
	bool isFuelPressurized();
	bool isFuelDepressurized();
	double getAltitudeFactor();

	// state variables
	double m_Drive;
	DataChannel<double>::Ref b_RPM;   // TODO export
	double m_StartRPM;
	double m_CoreTemperature;
	double m_ExhaustTemperature;   // TODO export
	DataChannel<double>::Ref b_FanTurbineInletTemperature;   // TODO export
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
	double m_FuelConsumption;   // TODO export
	double m_HotStartTemperature;
	DataChannel<double>::Ref b_Nozzle;   // TODO export
	double m_FuelPressure;
	double m_StartElapsedTime;
	double m_HangTime;
	double m_Blend;
	bool m_NormalStart;

	DataChannel<FuelManagementSystem::Ref>::CRef b_FuelManagementSystem;
	DataChannel<double>::Ref b_FuelFlow;

	// TODO add Link<jfs>

	void regen();
	simdata::Real m_WindSpin;
	simdata::Real m_DragRate;
	simdata::Real m_FanTurbineInletHeatRate;
	simdata::Real m_FanTurbineInletCoolRate;
	simdata::Real m_FanTurbineInletTemperatureBase;
	simdata::Real m_FanTurbineInletTemperatureScale;
	simdata::Real m_CoreFanTurbineInletTemperatureRatio;
	simdata::Real m_CoreHeatRate;
	simdata::Real m_CoreCoolRate;
	simdata::Real m_ExhaustHeatRate;
	simdata::Real m_ExhaustCoolRate;
	simdata::Real m_Friction;
	simdata::Real m_SpinDownTau;
	simdata::Real m_SpoolRate;
	simdata::Real m_AbnormalStartChance;
	simdata::Real m_HotDelta;
	simdata::Real m_Overheat;
	simdata::Real m_IdleRPM;
	simdata::Real m_AfterburnerRPM;
	simdata::Real m_AfterburnerDelay;
	simdata::Real m_AfterburnerMinRPM;
	simdata::Real m_AfterburnerCutoffRPM;
	simdata::Real m_IdleFuelConsumption;
	simdata::Real m_MinFTITIgnition;
	simdata::Real m_NozzleIdleFactor;
	simdata::Real m_NozzleBase;
	simdata::Real m_JFSOffRPM;
	simdata::Real m_StartDrive;
	simdata::Real m_DriveSpeed;
	simdata::Real m_NormalHangTime;
	simdata::Real m_AbnormalHangTime;
	simdata::Real m_HotStartBranch;
	simdata::Real m_JFSDriveFactor;
	simdata::Real m_FuelPressurizationRate;
	simdata::Real m_FuelDepressurizationRate;
};


#endif // __CSPSIM_F16_F16ENGINE_H__

