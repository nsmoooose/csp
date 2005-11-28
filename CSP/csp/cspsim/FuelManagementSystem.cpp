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
 * @file FuelManagementSystem.cpp
 *
 **/

#include <csp/cspsim/FuelManagementSystem.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/stores/StoresManagementSystem.h>

CSP_NAMESPACE

CSP_XML_BEGIN(FuelNode)
	CSP_DEF("key", m_Key, true)
	CSP_DEF("children", m_ChildKeys, false)
CSP_XML_END

CSP_XML_BEGIN(FuelTankNode)
	CSP_DEF("tank_id", m_TankId, false)
	CSP_DEF("pressurization_rate", m_PressurizationRate, false)
	CSP_DEF("depressurization_rate", m_DepressurizationRate, false)
	CSP_DEF("inflow_rate", m_InflowRate, false)
	CSP_DEF("outflow_rate", m_OutflowRate, false)
	CSP_DEF("needs_pressure", m_NeedsPressure, false)
CSP_XML_END

CSP_XML_BEGIN(FuelManagementSystem)
	CSP_DEF("fuel_nodes", m_FuelNodes, true)
CSP_XML_END


void FuelNode::bindChildren(NodeMap const &node_map) {
	assert(m_Children.empty());
	m_Children.reserve(m_ChildKeys.size());
	for (unsigned i = 0; i < m_ChildKeys.size(); ++i) {
		NodeMap::const_iterator iter = node_map.find(m_ChildKeys[i]);
		assert(iter != node_map.end());
		if (iter != node_map.end()) {
			m_Children.push_back(iter->second);
			iter->second->addParent(this);
		}
	}
}

FuelTankNode::FuelTankNode():
	m_Pressure(0.0),
	m_NeedsPressure(false), m_PressurizationRate(0.05), m_DepressurizationRate(0.1),
	m_InflowRate(10.0) /*lb/sec*/, m_OutflowRate(27.0) /*lb/sec*/ { }  // FIXME metric units

FuelTankNode::~FuelTankNode() { }

void FuelTankNode::bindStores(StoresManagementSystem* sms) {
	bool add = !m_Tank;
	m_Tank = 0;
	StoreIndex hp = sms->getHardpointByName(m_TankId);
	if (hp.valid()) {
		std::vector<Ref<Store> > stores;
		sms->getAllStores(hp, stores);
		for (unsigned i = 0; i < stores.size(); ++i) {
			m_Tank = !stores[i] ? 0 : stores[i]->asFuelTank();
			if (m_Tank.valid()) break;
		}
	}
	if (add && m_Tank.valid()) {
		CSPLOG(INFO, OBJECT) << "adding fuel to " << m_TankId;
		m_Tank->setQuantity(m_Tank->capacity());  // TODO SMS should handle this!
		sms->setDirtyDynamics();
	}
	if (m_Tank.valid()) {
		CSPLOG(INFO, OBJECT) << "FuelTankNode::bindStores: tank " << m_TankId << " found";
	} else {
		CSPLOG(INFO, OBJECT) << "FuelTankNode::bindStores: tank " << m_TankId << " not found";
	}
}

double FuelTankNode::capacity() const {
	return !m_Tank ? 0.0 : m_Tank->capacity();
}

double FuelTankNode::quantity() const {
	return !m_Tank ? 0.0 : m_Tank->quantity();
}

double FuelTankNode::fuelMass() const {
	return !m_Tank ? 0.0 : m_Tank->quantity() * m_Tank->fuelDensity();
}

void FuelTankNode::redistribute(double dt) {
	if (!m_Tank || blocked()) return;
	if (!m_Parents.empty()) {
		for (unsigned i = 0; i < m_Parents.size(); ++i) { // FIXME preferential order
			double limit = limitOut(dt);
			double transfer = std::min(limit, m_Parents[i]->limitIn(dt));
			addFuel(-transfer);
			m_Parents[i]->addFuel(transfer);
		}
	}
}

void FuelTankNode::addFuel(double amount) {
	if (m_Tank.valid()) m_Tank->setQuantity(m_Tank->quantity() + amount);
}


const char *FuelManagementSystem::Channel = "FuelManagementSystem";

// TODO temporary!
#include <sstream>
void FuelManagementSystem::getInfo(InfoList &info) const {
	std::ostringstream os;
	for (unsigned i = 0; i < m_FuelNodes.size(); ++i) {
		os << m_FuelNodes[i]->name().substr(0,5) << ":" << static_cast<int>(m_FuelNodes[i]->quantity()) << ":" << static_cast<int>(m_FuelNodes[i]->capacity()) << "; ";
	}
	info.push_back(os.str());
}

void FuelManagementSystem::postCreate() {
	System::postCreate();
	initialize();
}

void FuelManagementSystem::registerChannels(Bus *bus) {
	bus->registerLocalDataChannel<Ref<FuelManagementSystem> >(Channel, this);
}

void FuelManagementSystem::importChannels(Bus *) {
	StoresManagementSystem *sms = getModel()->getStoresManagementSystem().get();
	if (sms) {
		sms->addConfigurationHandler(sigc::mem_fun(this, &FuelManagementSystem::rebind));
		rebind();
	}
}

void FuelManagementSystem::initialize() {
	for (unsigned i = 0; i < m_FuelNodes.size(); ++i) {
		Key key = m_FuelNodes[i]->key();
		assert(m_NodeMap.find(key) == m_NodeMap.end());
		m_NodeMap[key] = m_FuelNodes[i].get();
	}
	for (unsigned i = 0; i < m_FuelNodes.size(); ++i) {
		m_FuelNodes[i]->bindChildren(m_NodeMap);
	}
}

void FuelManagementSystem::rebind() {
	StoresManagementSystem *sms = getModel()->getStoresManagementSystem().get();
	if (sms) {
		for (unsigned i = 0; i < m_FuelNodes.size(); ++i) {
			m_FuelNodes[i]->bindStores(sms);
		}
	}
}

CSP_NAMESPACE_END

