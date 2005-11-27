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
 **/


#ifndef __CSPSIM_F16_RADIO_H__
#define __CSPSIM_F16_RADIO_H__

#include <System.h>
#include <csp/lib/data/Enum.h>
#include <vector>


class F16Radio: public System {
public:
	SIMDATA_DECLARE_OBJECT(F16Radio)

	static simdata::Enumeration STATUS;
	static simdata::Enumeration MODE;
	static simdata::Enumeration BAND;
	static simdata::Enumeration POWER;

	typedef simdata::Enum<STATUS> Status;
	typedef simdata::Enum<MODE> Mode;
	typedef simdata::Enum<BAND> Band;
	typedef simdata::Enum<POWER> Power;

	virtual void importChannels(Bus*) {
	}

	virtual void registerChannels(Bus* bus) {
		bus->registerSharedDataChannel<simdata::EnumLink>("Radio.UHF.Status", Status("GUARD"));
		bus->registerSharedDataChannel<simdata::EnumLink>("Radio.VHF.Status", Status("BACKUP"));
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

#endif // __CSPSIM_F16_RADIO_H__

