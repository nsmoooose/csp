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
 * @file F16/FuelSystem.h
 *
 **/

#ifndef __CSPSIM_F16_FUELSYSTEM_H__
#define __CSPSIM_F16_FUELSYSTEM_H__

#include <csp/cspsim/FuelManagementSystem.h>

namespace csp {
namespace f16 {

/** Specialized FuelManagemantSystem for F-16C/D aircraft.
 */
class FuelSystem: public FuelManagementSystem {
public:
	CSP_DECLARE_OBJECT(f16::FuelSystem)

	FuelSystem();
	virtual double onUpdate(double dt);

	virtual double drawFuel(double dt, double amount);

	virtual double refuel(double /*dt*/, double /*amount*/, bool /*ground*/);

protected:
	virtual void registerChannels(Bus *bus);

	virtual void importChannels(Bus *bus);

	virtual void postCreate();

private:
	static const double m_NominalFuelDensity; // kg/l

	void updateFuelReadout();

	bool m_Pressurize;
	bool m_AFFTActive;
	double m_FuelMotion;

	double m_AFFTMaxFwdFuel;
	double m_AFFTMinForwardImbalance;
	double m_AFFTMaxForwardImbalance;

	double m_AftFuelLowLevel;
	double m_FwdFuelLowLevel;

	bool m_WingFirst;
	bool m_TestMode;

	FuelNode *m_FwdReservoir;
	FuelNode *m_AftReservoir;
	FuelNode *m_FwdFuselage;
	FuelNode *m_AftFuselage;
	FuelNode *m_LeftWing;
	FuelNode *m_RightWing;
	FuelNode *m_LeftExternal;
	FuelNode *m_RightExternal;
	FuelNode *m_CenterExternal;

	static const Enumeration MASTER_FUEL;
	static const Enumeration TANK_INERTING;
	static const Enumeration ENGINE_FEED;
	static const Enumeration FUEL_QTY_SEL;
	static const Enumeration EXT_FUEL_TRANS;
	static const Enumeration AIR_REFUEL;
	static const Enumeration AIR_SOURCE;

	CockpitSwitch::RefT b_MasterFuelSwitch;
	CockpitSwitch::RefT b_TankInertingSwitch;
	CockpitSwitch::RefT b_EngineFeedSwitch;
	CockpitSwitch::RefT b_FuelQtySelSwitch;
	CockpitSwitch::RefT b_ExtFuelTransSwitch;
	CockpitSwitch::RefT b_AirRefuelSwitch;
	CockpitSwitch::RefT b_AirSourceSwitch;
	CockpitInterface m_CockpitInterface;

	DataChannel<double>::RefT b_Totalizer;
	DataChannel<double>::RefT b_AftLeftPointer;
	DataChannel<double>::RefT b_FwdRightPointer;
	DataChannel<bool>::RefT b_FwdFuelLowLight;
	DataChannel<bool>::RefT b_AftFuelLowLight;

};

} // namespace f16
} // namespace csp

#endif // __CSPSIM_F16_FUELSYSTEM_H__
