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
#include "SmokeEffects.h"
#include "VirtualBattlefield.h"
#include "Log.h"

#include <osg/NodeVisitor>
#include <osg/Quat>

#include <SimData/InterfaceRegistry.h>
#include <SimData/Math.h>

#include <sstream>

SIMDATA_REGISTER_INTERFACE(SimObject)

unsigned int SimObject::localObjectInstance = 0;

SimObject::SimObject(TypeId type): m_ID(0), m_Type(type), m_Flags(0)
{
	CSP_LOG(APP, DEBUG, "SimObject::SimObject()" );

	m_ID = ++SimObject::localObjectInstance;

	setAggregateFlag(true);
}


SimObject::~SimObject() {
	CSP_LOG(APP, INFO, "SimObject::~SimObject()..." );
}


void SimObject::aggregate() {
	CSP_LOG(BATTLEFIELD, INFO, "SimObject aggregate " << int(this));
}

void SimObject::deaggregate() {
	CSP_LOG(BATTLEFIELD, INFO, "SimObject deaggregate " << int(this));
}

void SimObject::enterScene() {
	CSP_LOG(SCENE, INFO, "SimObject enterScene " << int(this));
	assert(!getVisibleFlag());
	setVisibleFlag(true);
}

void SimObject::leaveScene() {
	CSP_LOG(SCENE, INFO, "SimObject leaveScene " << int(this));
	assert(getVisibleFlag());
	setVisibleFlag(false);
}


std::string SimObject::_debugId() const {
	std::stringstream ss;
	ss << "<Object " << m_ID << '>';
	return ss.str();
}
