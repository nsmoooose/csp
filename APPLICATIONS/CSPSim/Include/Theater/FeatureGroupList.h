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


#ifndef __THEATER_FEATUREGROUPLIST_H__
#define __THEATER_FEATUREGROUPLIST_H__


#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>

#include "Theater/FeatureGroup.h"

class Projection;


/**
 * class ObjectiveList (STATIC)
 *
 * A collection of all Objectives in a Theater.
 */
class FeatureGroupList: public simdata::Object {
	simdata::Link<FeatureGroup>::vector m_FeatureGroups;
public:

	SIMDATA_OBJECT(FeatureGroupList, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(FeatureGroupList)
		SIMDATA_XML("feature_groups", FeatureGroupList::m_FeatureGroups, true)
	END_SIMDATA_XML_INTERFACE

	FeatureGroupList();

	virtual ~FeatureGroupList();

	virtual void pack(simdata::Packer& p) const {
		Object::pack(p);
		p.pack(m_FeatureGroups);
	}

	virtual void unpack(simdata::UnPacker& p) {
		Object::unpack(p);
		p.unpack(m_FeatureGroups);
	}

	/**
	 * Get the list of FeatureGroups.
	 */
	inline simdata::Link<FeatureGroup>::vector & getFeatureGroups() { return m_FeatureGroups; }

	void projectFeatureGroups(Projection const &map);
};



#endif // __THEATER_FEATUREGROUPLIST_H__


