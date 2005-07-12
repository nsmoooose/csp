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


#ifndef __F16_SYSTEM_H__
#define __F16_SYSTEM_H__

#include <System.h>
#include <Steerpoint.h>
#include <F16/NavigationSystem.h>

#include <SimData/Enum.h>
#include <vector>

class TimedSequence;

class F16System: public System {
	static const simdata::Enumeration MasterModes;

public:
	SIMDATA_DECLARE_OBJECT(F16System)

	DECLARE_INPUT_INTERFACE(F16System, System)
		BIND_ACTION("CATI", setCatI);
		BIND_ACTION("CATIII", setCatIII);
		BIND_ACTION("NEXT_STEERPOINT", nextSteerpoint);
		BIND_ACTION("PREV_STEERPOINT", prevSteerpoint);
		BIND_ACTION("FLAPS_DOWN", flapsDown);
		BIND_ACTION("FLAPS_UP", flapsUp);
		BIND_ACTION("FLAPS_TOGGLE", flapsToggle);
		BIND_ACTION("CANOPY_TOGGLE", canopyToggle);
		BIND_ACTION("FUEL_DOOR_TOGGLE",fuelDoorToggle);
	END_INPUT_INTERFACE  // protected:

public:
	F16System();

	virtual void flapsDown();
	virtual void flapsUp();
	virtual void flapsToggle();

	void canopyToggle();
	void fuelDoorToggle();

	virtual void setCatI();
	virtual void setCatIII();

	virtual void nextSteerpoint();
	virtual void prevSteerpoint();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

	typedef simdata::Enum<MasterModes> MasterMode;

protected:

	//void addTestSteerpoints();

	DataChannel<bool>::CRef b_GearExtendSelected;
	DataChannel<bool>::CRef b_GearFullyRetracted;
	DataChannel<double>::CRef b_Airspeed;
	DataChannel<double>::CRef b_FuelDoorSequence;
	DataChannel<bool>::CRef b_NoseLandingGearWOW;
	DataChannel<bool>::CRef b_LeftMainLandingGearWOW;
	DataChannel<bool>::CRef b_RightMainLandingGearWOW;
	DataChannel<bool>::CRef b_WheelSpin;
	DataChannel<double>::Ref b_TrailingEdgeFlapExtension;
	DataChannel<double>::Ref b_AirbrakeLimit;
	DataChannel<bool>::Ref b_AltFlaps;
	DataChannel<bool>::Ref b_CatIII;
	DataChannel<bool>::Ref b_StandbyGains;
	DataChannel<bool>::Ref b_TakeoffLandingGains;
	DataChannel<NavigationSystem::Ref>::Ref b_NavigationSystem;
	DataChannel<simdata::EnumLink>::Ref b_MasterMode;

	simdata::Link<TimedSequence> m_FuelDoorSequence;
	simdata::Link<TimedSequence> m_CanopySequence;

	virtual double onUpdate(double dt);

	// TODO should use push channels so we don't have to track state separately!
	// right now we poll state variables in the update handler to check if anything
	// has changed.  it would be more effecient to receive a callback then anyone
	// else changes the value of a shared channel.
	bool m_CatIII;

	//int m_SteerpointIndex;
	//std::vector<Steerpoint::Ref> m_Steerpoints;

};


#endif // __F16_SYSTEM_H__

