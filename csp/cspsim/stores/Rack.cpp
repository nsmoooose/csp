// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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
 * @file Rack.cpp
 *
 **/

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/stores/Rack.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/DataManager.h>

namespace csp {

CSP_XML_BEGIN(RackData)
	CSP_DEF("mounts", m_Mounts, true)
	CSP_DEF("default_compatibility", m_DefaultCompatibility, false)
//	CSP_DEF("compatibility", m_Compatibility, false)
//	CSP_DEF("mount_compatibility", m_MountCompatibility, false)
CSP_XML_END

CSP_XML_BEGIN(RackMount)
	CSP_DEF("offset", m_Offset, true)
	CSP_DEF("attitude", m_Attitude, false)
	CSP_DEF("compatibility", m_Compatibility, false)
	CSP_DEF("fixed", m_Fixed, false)
	CSP_DEF("eject_velocity", m_EjectVelocity, false)
	CSP_DEF("eject_angular_velocity", m_EjectAngularVelocity, false)
CSP_XML_END

Store* RackData::createStore() const {
	return new Rack(this);
}

unsigned RackData::capacity(Key const &store_key) const {
	if (m_DefaultCompatibility.valid() && m_DefaultCompatibility->isCompatible(store_key)) return capacity();
	unsigned count = 0;
	for (unsigned idx = 0; idx < m_Mounts.size(); ++idx) {
		if (m_Mounts[idx]->isCompatible(store_key)) count++;
	}
	return count;
}

void RackData::getCompatibleStores(StoreSet &stores) const {
	stores.clear();
	if (m_DefaultCompatibility.valid()) m_DefaultCompatibility->mergeStoreSet(stores);
	for (unsigned idx = 0; idx < m_Mounts.size(); ++idx) m_Mounts[idx]->mergeStoreSet(stores);
}

Store* Rack::findStore(StoreIndex const &idx) {
	if (idx == index()) return this;
	if (!index().isParent(idx)) return 0;
	unsigned child = index().relativeChild(idx);
	if (child >= m_Data->capacity() || !m_Children[child]) return 0;
	return m_Children[child]->findStore(idx);
}

unsigned Rack::availableSpace(Key const &store_key) const {
	const bool default_compatible = m_Data->isDefaultCompatible(store_key);
	unsigned count = 0;
	for (unsigned idx = 0; idx < getNumChildren(); ++idx) {
		if (!m_Children[idx] && (default_compatible || m_Data->mount(idx)->isCompatible(store_key))) count++;
	}
	return count;
}

bool Rack::getChildren(std::vector<Store::RefT> &children, bool recurse) const {
	bool any = false;
	for (unsigned i = 0; i < getNumChildren(); ++i) {
		if (m_Children[i].valid()) {
			any = true;
			children.push_back(m_Children[i]);
			if (recurse && m_Children[i]->asRack()) m_Children[i]->asRack()->getChildren(children, recurse);
		}
	}
	return any;
}

void Rack::sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, bool no_drag) const {
	Store::sumDynamics(dynamics, offset, attitude, no_drag);
	for (unsigned i = 0; i < m_Data->capacity(); ++i) {
		if (m_Children[i].valid()) {
			Vector3 child_offset = offset + attitude.rotate(m_Data->mount(i)->offset());
			Quat child_attitude = attitude * m_Data->mount(i)->attitude();
			m_Children[i]->sumDynamics(dynamics, child_offset, child_attitude, no_drag);
		}
	}
}

void Rack::addModel(osg::Group *group) const {
	Store::addModel(group);
	for (unsigned i = 0; i < m_Data->capacity(); ++i) {
		if (m_Children[i].valid()) {
			if (group) {
				osg::PositionAttitudeTransform *mount = new osg::PositionAttitudeTransform;
				mount->setPosition(toOSG(m_Data->mount(i)->offset()));
				mount->setAttitude(toOSG(m_Data->mount(i)->attitude()));
				m_Children[i]->addModel(mount);
				group->addChild(mount);
			} else {
				m_Children[i]->addModel(group);
			}
		}
	}
}

unsigned Rack::mountStores(StoreData const *data, unsigned count) {
	assert(data);
	unsigned added = 0;
	for (unsigned i = 0; i < m_Data->capacity(); ++i) {
		if (isAvailable(i, data->key())) {
			_setChild(i, data->createStore());
			assert(m_Children[i].valid());
			if (++added >= count) break;
		}
	}
	return added;
}

} // namespace csp

