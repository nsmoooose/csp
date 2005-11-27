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
 * @file StoresDefinition.h
 *
 **/


#ifndef __CSPSIM_STORESDEFINITION_H__
#define __CSPSIM_STORESDEFINITION_H__


#include <SimData/InterfaceRegistry.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Link.h>

#include <map>
#include <string>

class HardpointData;


/** StoresDefinition is a container for a set of hardpoint definitions.  Hardpoints
 *  are indexed by number (in order of definition) and canonical name (e.g., "HP5").
 *  Defines both internal and external hardpoints.
 */
class StoresDefinition: public simdata::Object {
public:
	SIMDATA_DECLARE_STATIC_OBJECT(StoresDefinition)

	/** Get the total number of hardpoints (internal and external).
	 */
	unsigned getNumHardpoints() const { return m_Hardpoints.size(); }

	/** Get a hardpoint by index number.
	 */
	HardpointData const * getHardpoint(unsigned i) {
		assert(i < m_Hardpoints.size());
		return m_Hardpoints[i].get();
	}

	/** Get the index number of a named hardpoint.  Returns false if the named
	 *  hardpoint is not found.
	 */
	bool getHardpointIndex(std::string const &name, unsigned &index) const;

protected:
	void postCreate();

private:
	simdata::Link<HardpointData>::vector m_Hardpoints;
	typedef std::map<std::string, unsigned> HardpointMap;
	HardpointMap m_HardpointMap;
};


#endif // __CSPSIM_STORESDEFINITION_H__

