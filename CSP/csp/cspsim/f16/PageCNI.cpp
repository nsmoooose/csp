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
 * @file PageCNI.cpp
 *
 **/


#include "PageCNI.h"
#include <ConditionsChannels.h>
#include <CSPSim.h>
#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>
#include <iostream>

// TODO replace m_RadioStatusLabel and radioStatusLabel with a widget class that
// maps enumlink to string and can be defined in xml.  then the page can be made
// generic:
//
// <Object class="DataEntryPage">
//   <List name="Widgets">
//     <Object class="DataEntryEnumWidget">
//       <Integer name="x">1</Integer>
//       <Integer name="y">1</Integer>
//       <Key name="channel">Radio.UHF.Status</Key>
//       <Vector name="labels">
//         <!-- TODO need a better map representation -->
//         <String>OFF</String>    <String>OFF</String>
//         <String>GUARD</String>  <String>GRD</String>
//         <String>BACKUP</String> <String>BUP</String>
//         <String>ON</String>     <String></String>
//       </Vector>
//     </Object>
//     ...
//   </List>
// </Object>
//
// Even if a generic page representation isn't possible, it is worth moving the
// label mappings to xml since they are both clunky and inflexible when hard
// coded.


PageCNI::PageCNI() {
	m_Steerpoint = new SimpleCycle(1, 1, 99, true);
	m_UHFPreset = new SimpleCycle(1, 1, 20, true);
	m_VHFPreset = new SimpleCycle(1, 1, 20, true);
	addCycle(new DataCycle(20, 0, m_Steerpoint.get()));
	addCycle(new DataCycle(5, 2, m_VHFPreset.get()));
	addCycle(new DataCycle(5, 0, m_UHFPreset.get()));
	addWidget(new SpeedDirectionWidget(18, 1, bus::Conditions::WindVelocity));
	addWidget(new EnumDisplayWidget(1, 1, "Radio.UHF.Status", "OFF:OFF,GUARD:GRD,BACKUP:BUP,ON:"));
	addWidget(new EnumDisplayWidget(1, 3, "Radio.VHF.Status", "OFF:OFF,GUARD:GRD,BACKUP:BUP,ON:"));
}

void PageCNI::render(AlphaNumericDisplay &ded) {
	DataEntryForm::render(ded);
	ded.write(1, 0, "UHF");
	ded.write(6, 0, "295.00");
	ded.write(1, 2, "VHF");
	ded.write(6, 2, "10");
	ded.write(15, 0, "STPT");
	ded.write(21, 0, "%2d", m_Steerpoint->get());
	simdata::SimDate now = CSPSim::theSim->getCurrentTime();
	std::string label = now.formatString("%H:%M:%S");
	if (label[0] == '0') label[0] = ' ';
	ded.write(17, 2, label);
	ded.write(1, 4, "M1234C");
	ded.write(8, 4, "7500");
	ded.write(16, 4, "MAN");
	ded.write(21, 4, "T123X");
}

double PageCNI::update() {
	simdata::SimDate now = CSPSim::theSim->getCurrentTime();
	double time = now.getTime();
	return 1.0 - (time - static_cast<int>(time));
}

std::string PageCNI::ICP_UP() { prevCycle(); return ""; }
std::string PageCNI::ICP_DN() { nextCycle(); return ""; }

std::string PageCNI::onNumber(int x) {
	switch (x) {
		case 0:
			return "MSEL";
		case 1:
			return "TILS";
		case 2:
			return "ALOW";
		case 4:
			return "STPT";
		case 5:
			return "CRUS";
		case 6:
			return "TIME";
		case 7:
			return "MARK";
		case 8:
			return "FIX";
		case 9:
			return "ACAL";
		default:
			return "";
	};
	return "";
}
