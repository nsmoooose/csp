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


#ifndef __CSPSIM_NAVIGATION_SYSTEM_H__
#define __CSPSIM_NAVIGATION_SYSTEM_H__

#include <Projection.h>
#include <System.h>
#include <Steerpoint.h>

#include <vector>


class NavigationSystem: public simdata::Referenced {
public:
	typedef simdata::Ref<NavigationSystem> Ref;

	NavigationSystem();

	virtual simdata::LLA fromWorld(simdata::Vector3 const &);
	virtual simdata::Vector3 toWorld(simdata::LLA const &);

	virtual void nextSteerpoint();
	virtual void prevSteerpoint();

	virtual Steerpoint::Ref activeSteerpoint();
	virtual void setActiveSteerpoint(unsigned index);

protected:
	void addTestSteerpoints();
	int m_SteerpointIndex;
	std::vector<Steerpoint::Ref> m_Steerpoints;
	simdata::Ref<const Projection> m_Projection;
};


#endif // __CSPSIM_NAVIGATION_SYSTEM_H__

