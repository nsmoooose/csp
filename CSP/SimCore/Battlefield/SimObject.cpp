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
 * @file SimObject.cpp
 *
 **/


#include "SimObject.h"
#include "Log.h"

#include <sstream>


SimObject::SimObject(TypeId type):
	m_Id(0),
	m_Type(type),
	m_Flags(0),
	m_Name("?"),
	m_AirBubble(0),
	m_GroundBubble(0)
{
	// XXX battlefield should set this!
	CSP_LOG(BATTLEFIELD, DEBUG, "SimObject::SimObject(): " << _debugId());
	setAggregateFlag(true);
	switch (type) {
		case TYPE_AIR_UNIT:
			setFlags(F_AIR, true);
			setFlags(F_STATIC, false);
			break;
		case TYPE_MUD_UNIT:
		case TYPE_SEA_UNIT:
			setFlags(F_AIR, false);
			setFlags(F_STATIC, false);
			break;
		case TYPE_STATIC:
			setFlags(F_AIR, false);
			setFlags(F_STATIC, true);
			break;
		default:
			CSP_LOG(BATTLEFIELD, ERROR, "Invalid object type: " << type);
			assert(0);
	};
}

SimObject::~SimObject() {
	CSP_LOG(BATTLEFIELD, DEBUG, "SimObject::~SimObject()" );
}

std::string SimObject::_debugId() const {
	std::stringstream ss;
	ss << "<SimObject " << id() << ", " << name() << '>';
	return ss.str();
}


