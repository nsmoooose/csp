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
 * @file PageLIST.h
 *
 **/

#include <csp/cspsim/f16/DataEntryPage.h>

namespace csp {

class PageLIST: public DataEntryForm {
	static const char *const LIST[];
	static const char *const MISC[];
	static const int COLS[];

	SimpleCycle::RefT m_Steerpoint;
	bool m_Misc;

public:
	PageLIST(): m_Misc(false) {
		m_Steerpoint = new SimpleCycle(1, 1, 99, true);
		addCycle(new DataCycle(24, 0, m_Steerpoint.get()));
	}

	virtual void importChannels(Bus*) { }

	virtual void render(AlphaNumericDisplay& ded);

	virtual std::string onNumber(int n) {
		if (n == 0 && !m_Misc) m_Misc = true;
		return "";
	}

	virtual void reset() {
		DataEntryForm::reset();
		m_Misc = false;
	}
};

} // namespace csp
