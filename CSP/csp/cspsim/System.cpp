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


#include <csp/cspsim/System.h>
#include <csp/cspsim/Bus.h>
#include <csp/cspsim/SystemsModel.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/ObjectInterface.h>


CSP_NAMESPACE

CSP_XML_BEGIN(System)
	CSP_DEF("subsystems", m_Subsystems, false)
CSP_XML_END


System::System(): m_Model(0) {
}

System::~System() {
}

void System::setModel(SystemsModel *model) {
	assert(m_Model == 0);
	assert(model != 0);
	m_Model = model;
	Bus::Ref bus = model->getBus();
	registerChannels(bus.get());
}

void System::postCreate() {
	// This method is called automatically after the system is instantiated
	// and deserialized from static data.  All the subsystems defined in XML
	// have been instantiated in the m_Subsystems vector, so now we add them
	// as child nodes and clear m_Subsystems to eliminate the extra set
	// of references.
	CSP_LOG(OBJECT, DEBUG, "System::postCreate() " << getClassName() << ", adding " << m_Subsystems.size() << " subsystems.");
	Link<System>::vector::iterator iter = m_Subsystems.begin();
	for (; iter != m_Subsystems.end(); ++iter) {
		CSP_LOG(OBJECT, DEBUG, "System::addChild() " << (*iter)->getClassName());
		addChild(iter->get());
	}
	m_Subsystems.clear();
}

bool System::addChild(SystemNode *node) {
	if (!SystemNode::addChild(node)) {
		CSP_LOG(OBJECT, ERROR, "SystemNode::addChild() failed.");
		return false;
	}
	return true;
}


CSP_NAMESPACE_END

