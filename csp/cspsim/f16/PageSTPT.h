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
 * @file PageSTPT.h
 *
 **/


#ifndef __CSPSIM_F16_PAGE_STPT_H__
#define __CSPSIM_F16_PAGE_STPT_H__

#include <csp/cspsim/f16/DataEntryPage.h>
#include <csp/csplib/util/Math.h>

CSP_NAMESPACE

class PageSTPT: public DataEntryForm {
public:
	PageSTPT() {
		addCycle(new DataCycle(17, 0, new ActiveSteerpointCycle("Navigation")));
		addEntry(new IntegerPad(13, 0, 2, 1, 99, new SteerpointIndexAccessor("Navigation")));  // stpt
		addEntry(new CoordinatePad(11, 1, CoordinatePad::LATITUDE, new SteerpointCoordinateAccessor(SteerpointCoordinateAccessor::LATITUDE, "Navigation")));
		addEntry(new CoordinatePad(11, 2, CoordinatePad::LONGITUDE, new SteerpointCoordinateAccessor(SteerpointCoordinateAccessor::LONGITUDE, "Navigation")));
		addEntry(new AltitudePad(11, 3, 6, new SteerpointCoordinateAccessor(SteerpointCoordinateAccessor::ELEVATION, "Navigation")));
	}

	virtual void render(AlphaNumericDisplay& ded) {
		DataEntryForm::render(ded);
		ded.write(8, 0, "STPT");
		ded.write(5, 1, "LAT  ");
		ded.write(5, 2, "LNG  ");
		ded.write(4, 3, "ELEV");
		ded.write(5, 4, "TOS");
		// TODO hook into man/auto mode
		ded.write(21, 0, "MAN");
		ded.write(11, 4, "12:58:34");
	}
};

CSP_NAMESPACE_END

#endif // __CSPSIM_F16_PAGE_STPT_H__

