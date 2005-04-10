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
 * @file PageLIST.cpp
 *
 **/


#include "PageLIST.h"


const char *const PageLIST::LIST[] = {"DEST", "BINGO", "VIP", "INTG", "NAV", "MAN", "INS", "DLNK", "", "MODE", "VRP", "MISC"};
const char *const PageLIST::MISC[] = {"CORR", "MAGV", "OFP", "", "INSM", "LASR", "GPS", "", "DRNG", "BULL", "", ""};
const int PageLIST::COLS[] = {1, 7, 14, 19};

void PageLIST::render(AlphaNumericDisplay& ded) {
	DataEntryForm::render(ded);
	char label[2]; label[1] = 0;
	for (int i = 0; i < 12; i++) {
		label[0] = "123R456E7890"[i];
		ded.write(COLS[i % 4], i/4 + 1, label, AlphaNumericDisplay::INVERSE);
		ded.write(COLS[i % 4] + 1, i/4 + 1, m_Misc ? MISC[i] : LIST[i]);
	}
	ded.write(12, 0, m_Misc ? "MISC" : "LIST");
	ded.write(21, 0, "%2d", m_Steerpoint->get());
}

