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
 * @file NavigationSystem.h
 *
 **/

#include <csp/cspsim/Projection.h>
#include <csp/cspsim/System.h>
#include <csp/cspsim/Steerpoint.h>

#include <vector>

namespace csp {

class NavigationSystem: public Referenced {
public:
	typedef Ref<NavigationSystem> RefT;

	NavigationSystem();

	virtual LLA fromWorld(Vector3 const &);
	virtual Vector3 toWorld(LLA const &);

	virtual void nextSteerpoint();
	virtual void prevSteerpoint();

	virtual Steerpoint::RefT activeSteerpoint();
	virtual void setActiveSteerpoint(unsigned index);

protected:
	void addTestSteerpoints();
	int m_SteerpointIndex;
	std::vector<Steerpoint::RefT> m_Steerpoints;
	Ref<const Projection> m_Projection;
};

} // namespace csp
