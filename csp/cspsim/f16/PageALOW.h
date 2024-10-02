#pragma once
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
 * @file PageALOW.h
 *
 **/

#include <csp/cspsim/f16/DataEntryPage.h>

namespace csp {

class PageALOW: public DataEntryForm {
	CycleCallback::RefT m_Steerpoint;

public:
	PageALOW() {
		m_Steerpoint = new ActiveSteerpointCycle("Navigation");
		addCycle(new DataCycle(23, 0, m_Steerpoint.get()));
		addEntry(new AltitudePad(16, 1, 5, new ChannelAccessor<double>("F16.CaraAlow")));  // cara alow
		addEntry(new AltitudePad(16, 2, 5, new ChannelAccessor<double>("F16.MslFloor")));  // msl floor
		addEntry(new AltitudePad(16, 3, 5, new ChannelAccessor<double>("F16.TfAdv")));  // tf adv
	}

	virtual void render(AlphaNumericDisplay& ded) {
		DataEntryForm::render(ded);
		ded.write(12, 0, "ALOW");
		ded.write(4, 1, "CARA ALOW");
		ded.write(4, 2, "MSL FLOOR");
		ded.write(4, 3, "TF ADV (MSL)");
		ded.write(21, 0, "%2d", m_Steerpoint->get());
	}
};

} // namespace csp
