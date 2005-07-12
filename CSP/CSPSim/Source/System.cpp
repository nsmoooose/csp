// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2003 The Combat Simulator Project
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
 * @file System.cpp
 *
 * Vehicle systems base classes and infrastructure.
 *
 **/


#include "System.h"
#include "SystemsModel.h"


SIMDATA_XML_BEGIN(System)
	SIMDATA_DEF("subsystems", m_Subsystems, false)
SIMDATA_XML_END


void System::setModel(SystemsModel *model) {
	assert(m_Model == 0);
	assert(model != 0);
	m_Model = model;
	Bus::Ref bus = model->getBus();
	registerChannels(bus.get());
}


