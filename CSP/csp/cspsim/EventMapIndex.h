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
 * @file EventMapIndex.h
 *
 **/

#ifndef __CSPSIM_EVENTMAPINDEX_H__
#define __CSPSIM_EVENTMAPINDEX_H__


#include <csp/cspsim/HID.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/EventMapping.h>

#include <csp/csplib/util/HashUtility.h>

CSP_NAMESPACE

class EventMapIndex: public Referenced {
public:
	EventMapping::RefT getMap(const hasht &key);
	EventMapping::RefT getMap(const std::string &id);
	void load(std::string const &path);
	void loadAllMaps();

protected:
	typedef HashMap<hasht, EventMapping::RefT>::Type MapHash;
	typedef std::vector<EventMapping::RefT> MapVector;
	MapHash m_Index;
	MapVector m_Maps;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_EVENTMAPINDEX_H__

