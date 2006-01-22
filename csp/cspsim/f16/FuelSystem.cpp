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
 * @file F16/FuelSystem.cpp
 *
 **/

// todos
// forward fuel transfer system
// air source
// indicator animations

#include <csp/cspsim/CockpitInterface.h>
#include <csp/cspsim/FuelManagementSystem.h>
#include <csp/cspsim/stores/StoresManagementSystem.h>

#include <csp/csplib/data/Enum.h>
#include <csp/csplib/util/Conversions.h>

CSP_NAMESPACE

namespace f16 {

/** Specialized FuelManagemantSystem for F-16C/D aircraft.
 */
class FuelSystem: public FuelManagementSystem {
public:
	CSP_DECLARE_OBJECT(f16::FuelSystem)

	FuelSystem():
		m_Pressurize(true),
		m_AFFTActive(false),
		m_FuelMotion(0.0f),
		m_AFFTMaxFwdFuel(850.0f),  // 1500 lb
		m_AFFTMinForwardImbalance(-510.0f),  // -900 lb
		m_AFFTMaxForwardImbalance(-425.0f),  // -750 lb
		m_AftFuelLowLevel(140.0f),  // 250 lb
		m_FwdFuelLowLevel(225.0f),  // 400 lb
		m_WingFirst(false),
		m_TestMode(false),
		m_FwdReservoir(0),
		m_AftReservoir(0),
		m_FwdFuselage(0),
		m_AftFuselage(0),
		m_LeftWing(0),
		m_RightWing(0),
		m_LeftExternal(0),
		m_RightExternal(0),
		m_CenterExternal(0)
	{
		b_MasterFuelSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&MASTER_FUEL, "F16.MasterFuelSwitch", "MASTER_FUEL", "MASTER"));
		b_TankInertingSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&TANK_INERTING, "F16.TankInertingSwitch", "TANK_INERTING", "OFF"));
		b_EngineFeedSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&ENGINE_FEED, "F16.EngineFeedSwitch", "ENGINE_FEED", "NORM"));
		b_FuelQtySelSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&FUEL_QTY_SEL, "F16.FuelQtySelSwitch", "FUEL_QTY_SEL", "NORM"));
		b_ExtFuelTransSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&EXT_FUEL_TRANS, "F16.ExtFuelTransSwitch", "EXT_FUEL_TRANS", "NORM"));
		b_AirRefuelSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&AIR_REFUEL, "F16.AirRefuelSwitch", "AIR_REFUEL", "CLOSE"));
		b_AirSourceSwitch = m_CockpitInterface.addElement(new CockpitSwitch(&AIR_SOURCE, "F16.AirSourceSwitch", "AIR_SOURCE", "NORM"));
		m_CockpitInterface.bindEvents(this);
	}

	virtual double onUpdate(double dt) {
		static const EnumLink norm(EXT_FUEL_TRANS, "NORM");
		static const EnumLink air_refuel_open(AIR_REFUEL, "OPEN");
		static const EnumLink air_source_norm(AIR_SOURCE, "NORM");
		static const EnumLink air_source_dump(AIR_SOURCE, "DUMP");

		if ((b_ExtFuelTransSwitch->state() == norm) && (m_CenterExternal->quantity() > 0.1)) {
			m_CenterExternal->unblock();
			m_LeftExternal->block();
			m_RightExternal->block();
		} else {
			if (m_LeftExternal->quantity() + m_RightExternal->quantity() > 0.1) {
				m_CenterExternal->block();
			} else {
				m_CenterExternal->unblock();
			}
			m_LeftExternal->unblock();
			m_RightExternal->unblock();
		}

		m_Pressurize = (b_AirRefuelSwitch->state() != air_refuel_open && (b_AirSourceSwitch->state() == air_source_norm || b_AirSourceSwitch->state() == air_source_dump));

		// FIXME updates the center tank twice!
		m_FwdReservoir->update(dt, m_Pressurize);
		m_AftReservoir->update(dt, m_Pressurize);
		updateFuelReadout();
		if (m_FuelMotion > 100.0f) {
			m_FuelMotion = 0.0f;
			StoresManagementSystem *sms = getModel()->getStoresManagementSystem().get(); // TODO should return a raw ptr
			if (sms) sms->setDirtyDynamics();
		}
		return 1.0;  // once per second is enough
	}

	virtual double drawFuel(double dt, double amount) {
		static const EnumLink master(MASTER_FUEL, "MASTER");
		static const EnumLink norm(ENGINE_FEED, "NORM");

		double quantity = 0.0;
		const double aft_fuel = m_AftReservoir->quantity() + m_AftFuselage->quantity();
		const double fwd_fuel = m_FwdReservoir->quantity() + m_FwdFuselage->quantity();
		const double fwd_heavy = fwd_fuel - aft_fuel;

		//if (m_AFFTActive) { static double XXX = 0.0;  XXX += dt; if (XXX > 5) { XXX = 0; std::cout << "*** " << b_EngineFeedSwitch->state().getToken() << "; fwd heavy: " << fwd_heavy << " " << m_AFFTActive << "\n"; } }

		if (b_MasterFuelSwitch->state() == master) {
			switch (b_EngineFeedSwitch->state().getValue()) {
				case 0: // off
					// TODO in off presumably the maximum transfer rate is lower (no boost pumps)
					// and may be affected by aircraft attitude.
				case 1: // norm
					// equal draw from fwd and aft reservoirs (ffp only); AFFT for norm setting
					// is handled separately below.
					quantity += m_FwdReservoir->drawFuel(0.5 * dt, 0.5 * amount - quantity);
					quantity += m_AftReservoir->drawFuel(dt, amount - quantity);
					quantity += m_FwdReservoir->drawFuel(0.5 * dt, amount - quantity);
					break;
				case 2: // aft
					// todo transfer from aft to fwd
					quantity += m_AftReservoir->drawFuel(dt, amount - quantity);
					break;
				case 3: // fwd
					// todo transfer from fwd to aft
					quantity += m_FwdReservoir->drawFuel(dt, amount - quantity);
					break;
				default:
					assert(0);
			}
			// automatic forward fuel transfer
			if (b_EngineFeedSwitch->state() == norm && fwd_fuel < m_AFFTMaxFwdFuel) {
				if (fwd_heavy > m_AFFTMaxForwardImbalance) {
					m_AFFTActive = false;
				} else if (fwd_heavy < m_AFFTMinForwardImbalance) {
					m_AFFTActive = true;
				}
				if (m_AFFTActive) {
					// assumes crossfeed pump limits are independent of ffp limits.
					// TODO 20 l/s needs to be externalized; no idea what the proper rate is.
					// FIXME fuel tank rate limits are circumvented by calling drawFuel and refuel
					// multiple times per timestep.  in addition, the rate depends on the type of
					// transfer (e.g., gravity feed or pump)!
					double transfer = m_AftReservoir->drawFuel(dt, 20.0 * dt);
					m_FuelMotion += transfer;
					transfer -= m_FwdReservoir->refuel(dt, transfer);
					// FIXME: this is dumb.
					if (transfer > 0.0) {
						m_FuelMotion -= transfer;
						m_AftReservoir->refuel(dt, transfer);
					}
				}
			} else {
				// reset the trigger when system is disabled; not sure if this is the correct behavoir.
				m_AFFTActive = false;
			}
		}
		// TODO jfs draws fuel regardless of master fuel switch setting.  probably need
		// an extra parameter to indicate jfs vs engine fuel draw.
		m_FuelMotion += quantity;

		// ensure that the totalizer output updates continuously.
		if (b_FuelQtySelSwitch->state().getValue() != 0 /*TEST*/ && !m_TestMode) {
			b_Totalizer->value() -= convert::kg_lb(quantity * m_NominalFuelDensity);
		}

		return quantity;
	}

	virtual double refuel(double /*dt*/, double /*amount*/, bool /*ground*/) {
		return 0.0; // TODO implement me!
	}

protected:
	virtual void registerChannels(Bus *bus) {
		FuelManagementSystem::registerChannels(bus);
		m_CockpitInterface.registerChannels(bus);
		b_Totalizer = bus->registerLocalDataChannel<double>("F16.Totalizer", 0.0f);
		b_AftLeftPointer = bus->registerLocalDataChannel<double>("F16.AftLeftPointer", 0.0f);
		b_FwdRightPointer = bus->registerLocalDataChannel<double>("F16.FwdRightPointer", 0.0f);
		b_AftFuelLowLight = bus->registerLocalDataChannel<bool>("F16.AftFuelLowLight", false);
		b_FwdFuelLowLight = bus->registerLocalDataChannel<bool>("F16.FwdFuelLowLight", false);
	}

	virtual void importChannels(Bus *bus) {
		FuelManagementSystem::importChannels(bus);
		// TODO get pressurize setting from bus
	}

	virtual void postCreate() {
		FuelManagementSystem::postCreate();
		m_FwdReservoir = getNode(Key("FwdReservoir")); assert(m_FwdReservoir);
		m_AftReservoir = getNode(Key("AftReservoir")); assert(m_AftReservoir);
		m_FwdFuselage = getNode(Key("FwdFuselage")); assert(m_FwdFuselage);
		m_AftFuselage = getNode(Key("AftFuselage")); assert(m_AftFuselage);
		m_LeftWing = getNode(Key("LeftWing")); assert(m_LeftWing);
		m_RightWing = getNode(Key("RightWing")); assert(m_RightWing);
		m_LeftExternal = getNode(Key("LeftExternal")); assert(m_LeftExternal);
		m_RightExternal = getNode(Key("RightExternal")); assert(m_RightExternal);
		m_CenterExternal = getNode(Key("CenterExternal")); assert(m_CenterExternal);
	}

private:
	static const double m_NominalFuelDensity; // kg/l

	void updateFuelReadout() {
		double totalizer = 0;
		double al = 0;
		double fr = 0;
		totalizer =
			m_AftReservoir->quantity() + m_FwdReservoir->quantity() +
			m_AftFuselage->quantity() + m_FwdFuselage->quantity() +
			m_LeftWing->quantity() + m_RightWing->quantity() +
			m_LeftExternal->quantity() + m_RightExternal->quantity() + m_CenterExternal->quantity();

		bool aft_warning_light = (m_AftReservoir->quantity() < m_AftFuelLowLevel);
		bool fwd_warning_light = (m_FwdReservoir->quantity() < m_FwdFuelLowLevel);
		m_TestMode = false;

		switch (b_FuelQtySelSwitch->state().getValue()) {
			case 0: // test
				totalizer = convert::lb_kg(6000.0) / m_NominalFuelDensity;
				al = fr = convert::lb_kg(2000.0) / m_NominalFuelDensity;
				fwd_warning_light = aft_warning_light = true;
				m_TestMode = true;
				break;
			case 1: // norm
				al = m_AftReservoir->quantity() + m_AftFuselage->quantity();
				fr = m_FwdReservoir->quantity() + m_FwdFuselage->quantity();
				break;
			case 2: // rsvr
				al = m_AftReservoir->quantity();
				fr = m_FwdReservoir->quantity();
				break;
			case 3: // int_wing
				al = m_LeftWing->quantity();
				fr = m_RightWing->quantity();
				break;
			case 4: // ext_wing
				al = m_LeftExternal->quantity();
				fr = m_RightExternal->quantity();
				break;
			case 5: // ext_ctr
				al = 0;
				fr = m_CenterExternal->quantity();
				break;
			default:
				assert(0);
		}

		b_AftFuelLowLight->value() = aft_warning_light;
		b_FwdFuelLowLight->value() = fwd_warning_light;

		// TODO make these targets and low pass filter the channel values (need a
		// higher update rate, with an internal timer for slower fuel management
		// updates).
		b_Totalizer->value() = convert::kg_lb(totalizer * m_NominalFuelDensity);
		b_AftLeftPointer->value() = convert::kg_lb(al * m_NominalFuelDensity);
		b_FwdRightPointer->value() = convert::kg_lb(fr * m_NominalFuelDensity);
	}

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

CSP_XML_BEGIN(f16::FuelSystem)
	CSP_DEF("aft_fuel_low_level", m_AftFuelLowLevel, false)
	CSP_DEF("fwd_fuel_low_level", m_FwdFuelLowLevel, false)
CSP_XML_END

const Enumeration FuelSystem::MASTER_FUEL("OFF MASTER");
const Enumeration FuelSystem::TANK_INERTING("OFF INERTING");
const Enumeration FuelSystem::ENGINE_FEED("OFF NORM AFT FWD");
const Enumeration FuelSystem::FUEL_QTY_SEL("TEST NORM RSVR INT_WING EXT_WING EXT_CTR");
const Enumeration FuelSystem::EXT_FUEL_TRANS("NORM WING_FIRST");
const Enumeration FuelSystem::AIR_REFUEL("CLOSE OPEN");
const Enumeration FuelSystem::AIR_SOURCE("OFF NORM DUMP RAM");
const double FuelSystem::m_NominalFuelDensity = 0.8; // kg/l

} // namespace f16

CSP_NAMESPACE_END

