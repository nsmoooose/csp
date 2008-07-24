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
 * @file Radio.h
 *
 */


#ifndef __CSPSIM_F16_RADIO_H__
#define __CSPSIM_F16_RADIO_H__

#include <csp/cspsim/System.h>
#include <csp/csplib/data/Enum.h>

#include <vector>

namespace csp {

class F16Radio: public System {
public:
	CSP_DECLARE_OBJECT(F16Radio)

	static Enumeration STATUS;
	static Enumeration MODE;
	static Enumeration BAND;
	static Enumeration POWER;

	typedef Enum<STATUS> Status;
	typedef Enum<MODE> Mode;
	typedef Enum<BAND> Band;
	typedef Enum<POWER> Power;

	virtual void importChannels(Bus*) {
	}

	virtual void registerChannels(Bus* bus) {
		bus->registerSharedDataChannel<EnumLink>("Radio.UHF.Status", Status("GUARD"));
		bus->registerSharedDataChannel<EnumLink>("Radio.VHF.Status", Status("BACKUP"));
	}

	F16Radio();

protected:
	Power m_UHFPower;
	Power m_VHFPower;
	Band m_UHFBand;
	Band m_VHFBand;
	Status m_UHFStatus;
	Status m_VHFStatus;
	Mode m_Mode;
	std::vector<int> m_UHFPresets;
	std::vector<int> m_VHFPresets;
};

} // namespace csp

#endif // __CSPSIM_F16_RADIO_H__

