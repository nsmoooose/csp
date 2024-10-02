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
 * @file PageCNI.h
 *
 **/

#include <csp/cspsim/f16/DataEntryPage.h>
#include <csp/csplib/data/Enum.h>
#include <csp/csplib/data/Vector3.h>

namespace csp {

class PageCNI: public DataEntryForm {
public:
	PageCNI();

	virtual std::string ICP_UP();
	virtual std::string ICP_DN();

protected:
	virtual double update();
	virtual void render(AlphaNumericDisplay& ded);
	virtual std::string onNumber(int x);

private:
	SimpleCycle::RefT m_Steerpoint;
	SimpleCycle::RefT m_UHFPreset;
	SimpleCycle::RefT m_VHFPreset;
};

} // namespace csp
