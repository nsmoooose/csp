// Combat Simulator Project - FlightSim Demo
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
 * @file EventMapIndex.h
 *
 **/

#ifndef __EVENTMAPINDEX_H__
#define __EVENTMAPINDEX_H__


#include "HID.h"
#include "Config.h"
#include "EventMapping.h"

#include <SimData/HashUtility.h>


class EventMapIndex 
{	
public:
	~EventMapIndex();
	EventMapping *getMap(const simdata::hasht &key);
	EventMapping *getMap(const std::string &id);
	void load(std::string const &path);
	void loadAllMaps();

protected:
	typedef simdata::HASHT_MAP<EventMapping*>::Type MapHash;
	typedef std::vector<EventMapping*> MapVector;
	MapHash m_Index;
	MapVector m_Maps;
};


#endif // __EVENTMAPINDEX_H__

