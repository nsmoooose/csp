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
 * @file F16System.h
 *
 **/

/**
 * @namespace csp::f16
 * @brief F16 systems implementation.
 */

#ifndef __CSPSIM_F16_F16SYSTEM_H__
#define __CSPSIM_F16_F16SYSTEM_H__

#include <csp/cspsim/f16/NavigationSystem.h>
#include <csp/cspsim/System.h>
#include <csp/cspsim/Steerpoint.h>

#include <csp/csplib/data/Enum.h>

#include <vector>

CSP_NAMESPACE

class TimedSequence;

class F16System: public System {
	static const Enumeration MasterModes;

public:
	CSP_DECLARE_OBJECT(F16System)

	DECLARE_INPUT_INTERFACE(F16System, System)
		BIND_ACTION("CATI", setCatI);
		BIND_ACTION("CATIII", setCatIII);
		BIND_ACTION("MPO_ON", engageMPO);
		BIND_ACTION("MPO_OFF", disengageMPO);
		BIND_ACTION("NEXT_STEERPOINT", nextSteerpoint);
		BIND_ACTION("PREV_STEERPOINT", prevSteerpoint);
		BIND_ACTION("FLAPS_DOWN", flapsDown);
		BIND_ACTION("FLAPS_UP", flapsUp);
		BIND_ACTION("FLAPS_TOGGLE", flapsToggle);
		BIND_ACTION("CANOPY_TOGGLE", canopyToggle);
		BIND_ACTION("EJECT_WING_TANKS", onEjectWingTanks);
		BIND_ACTION("EJECT_CENTER_TANK", onEjectCenterTank);
	END_INPUT_INTERFACE  // protected:

public:
	F16System();

	virtual void flapsDown();
	virtual void flapsUp();
	virtual void flapsToggle();

	void canopyToggle();

	virtual void setCatI();
	virtual void setCatIII();

	virtual void nextSteerpoint();
	virtual void prevSteerpoint();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

	typedef Enum<MasterModes> MasterMode;

protected:

	//void addTestSteerpoints();

	DataChannel<EnumLink>::CRefT b_AirRefuelSwitch;
	DataChannel<bool>::CRefT b_GearExtendSelected;
	DataChannel<bool>::CRefT b_GearFullyRetracted;
	DataChannel<double>::CRefT b_Airspeed;
	DataChannel<double>::CRefT b_FuelDoorSequence;
	DataChannel<bool>::CRefT b_NoseLandingGearWOW;
	DataChannel<bool>::CRefT b_LeftMainLandingGearWOW;
	DataChannel<bool>::CRefT b_RightMainLandingGearWOW;
	DataChannel<bool>::CRefT b_WheelSpin;
	DataChannel<double>::RefT b_TrailingEdgeFlapExtension;
	DataChannel<double>::RefT b_AirbrakeLimit;
	DataChannel<bool>::RefT b_AltFlaps;
	DataChannel<bool>::RefT b_CatIII;
	DataChannel<bool>::RefT b_ManualPitchOverride;
	DataChannel<bool>::RefT b_ManualPitchOverrideActive;
	DataChannel<bool>::RefT b_StandbyGains;
	DataChannel<bool>::RefT b_TakeoffLandingGains;
	DataChannel<Ref<NavigationSystem> >::RefT b_NavigationSystem;
	DataChannel<EnumLink>::RefT b_MasterMode;

	Link<TimedSequence> m_FuelDoorSequence;
	Link<TimedSequence> m_CanopySequence;

	virtual double onUpdate(double dt);

	// TODO should use push channels so we don't have to track state separately!
	// right now we poll state variables in the update handler to check if anything
	// has changed.  it would be more effecient to receive a callback then anyone
	// else changes the value of a shared channel.
	bool m_CatIII;

	//int m_SteerpointIndex;
	//std::vector<Steerpoint::RefT> m_Steerpoints;
	void onAirRefuelSwitch();

	// for testing only
	void onEjectWingTanks();
	void onEjectCenterTank();

	void engageMPO();
	void disengageMPO();
};

CSP_NAMESPACE_END

#endif // __CSPSIM_F16_F16SYSTEM_H__

