// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file FeatureGroupList.h
 *
 **/


#ifndef __CSPSIM_THEATER_FEATUREGROUPLIST_H__
#define __CSPSIM_THEATER_FEATUREGROUPLIST_H__


#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Object.h>

#include <csp/cspsim/theater/FeatureGroup.h>

CSP_NAMESPACE

class Projection;


/**
 * class ObjectiveList (STATIC)
 *
 * A collection of all Objectives in a Theater.
 */
class FeatureGroupList: public Object {
	Link<FeatureGroup>::vector m_FeatureGroups;

public:
	CSP_DECLARE_STATIC_OBJECT(FeatureGroupList)

	FeatureGroupList();

	virtual ~FeatureGroupList();

	/**
	 * Get the list of FeatureGroups.
	 */
	inline Link<FeatureGroup>::vector & getFeatureGroups() { return m_FeatureGroups; }

	void projectFeatureGroups(Projection const &map);
};

CSP_NAMESPACE_END

#endif // __CSPSIM_THEATER_FEATUREGROUPLIST_H__


