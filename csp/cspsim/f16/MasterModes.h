#pragma once
// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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
 * @file MasterMode.h
 *
 **/

#include <csp/csplib/data/Enum.h>
#include <csp/cspsim/Bus.h>

namespace csp {

namespace f16 {

extern const Enumeration MasterModes;
typedef Enum<MasterModes> MasterMode;

extern const MasterMode NAV;
extern const MasterMode AA;
extern const MasterMode AG;
extern const MasterMode MSL;
extern const MasterMode DGFT;
extern const MasterMode SJ;
extern const MasterMode EJ;

class MasterModeSelection: public ChannelBase {
	typedef sigc::signal<void, MasterMode const&> CallbackSignal;

public:
	typedef Ref<MasterModeSelection> RefT;
	typedef Ref<const MasterModeSelection> CRefT;

	MasterModeSelection(): ChannelBase("MasterMode", ACCESS_SHARED), m_Base(NAV), m_Mode(NAV), m_SelectiveJettison(false), m_EmergencyJettison(false), m_Dogfight(false), m_Missile(false) { }

	MasterMode const &mode() const { return m_Mode; }

	void setBaseMode(MasterMode const &mode) {
		if (mode == NAV || mode == AG || mode == AA) {
			m_Base = mode;
			updateMode();
		}
	}

	void setNAV() {
		m_Base = NAV;
		updateMode();
	}

	void setAA() {
		m_Base = AA;
		updateMode();
	}

	void setAG() {
		m_Base = AG;
		updateMode();
	}

	void setDogfight(bool on) {
		m_Dogfight = on;
		m_Missile = false;
		updateMode();
	}

	void setSelectiveJettison(bool on) {
		m_SelectiveJettison = on;
		updateMode();
	}

	bool getSelectiveJettision() const {
		return m_SelectiveJettison;
	}

	bool getEmegencyJettision() const {
		return m_EmergencyJettison;
	}

	void setEmergencyJettison(bool on) {
		m_EmergencyJettison = on;
		updateMode();
	}

	void reset() {
		m_SelectiveJettison = false;
		m_EmergencyJettison = false;
		m_Dogfight = false;
		m_Missile = false;
		updateMode();
	}

	sigc::connection registerCallback(CallbackSignal::slot_type const &slot) const {
		return m_ChangeSignal.connect(slot);
	}

private:
	void updateMode();

	MasterMode m_Base;
	MasterMode m_Mode;
	bool m_SelectiveJettison;
	bool m_EmergencyJettison;
	bool m_Dogfight;
	bool m_Missile;
	mutable CallbackSignal m_ChangeSignal;
};

} // namespace f16

} // namespace csp
