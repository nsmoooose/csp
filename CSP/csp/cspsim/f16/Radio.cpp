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

#include <csp/cspsim/f16/Radio.h>
#include <csp/csplib/data/ObjectInterface.h>

CSP_NAMESPACE

CSP_XML_BEGIN(F16Radio)
	// TODO presets
CSP_XML_END

Enumeration F16Radio::STATUS("OFF GUARD BACKUP ON");
Enumeration F16Radio::MODE("OFF MAIN BOTH");
Enumeration F16Radio::BAND("NARROW WIDE");
Enumeration F16Radio::POWER("OFF ON");


F16Radio::F16Radio():
	m_UHFPower("ON"),
	m_VHFPower("ON"),
	m_UHFBand("NARROW"),
	m_VHFBand("NARROW"),
	m_UHFStatus("GUARD"),
	m_VHFStatus("BACKUP"),
	m_Mode("BOTH")
{
	m_UHFPresets.push_back(22500);
	m_UHFPresets.push_back(13975);
	m_UHFPresets.push_back(19200);
	m_UHFPresets.push_back(18870);
	m_UHFPresets.push_back(20100);
	m_UHFPresets.push_back(21000);
	m_UHFPresets.push_back(19400);
	m_UHFPresets.push_back(19800);
	m_UHFPresets.push_back(22050);
	m_UHFPresets.push_back(15715);
	m_UHFPresets.push_back(17500);
	m_UHFPresets.push_back(19100);
	m_UHFPresets.push_back(20400);
	m_UHFPresets.push_back(20675);
	m_UHFPresets.push_back(21450);
	m_UHFPresets.push_back(16400);
	m_UHFPresets.push_back(16875);
	m_UHFPresets.push_back(18120);
	m_UHFPresets.push_back(14570);
	m_UHFPresets.push_back(15000);

	m_VHFPresets.push_back(3500);
	m_VHFPresets.push_back(3975);
	m_VHFPresets.push_back(6200);
	m_VHFPresets.push_back(10870);
	m_VHFPresets.push_back(3100);
	m_VHFPresets.push_back(8000);
	m_VHFPresets.push_back(6400);
	m_VHFPresets.push_back(11000);
	m_VHFPresets.push_back(3050);
	m_VHFPresets.push_back(5715);
	m_VHFPresets.push_back(7500);
	m_VHFPresets.push_back(11200);
	m_VHFPresets.push_back(4000);
	m_VHFPresets.push_back(3675);
	m_VHFPresets.push_back(5450);
	m_VHFPresets.push_back(11300);
	m_VHFPresets.push_back(6875);
	m_VHFPresets.push_back(8120);
	m_VHFPresets.push_back(4570);
	m_VHFPresets.push_back(11400);
}

CSP_NAMESPACE_END

