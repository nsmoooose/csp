
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
 * @file Hardpoint.cpp
 *
 **/


#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/ObjectModel.h>

#include <csp/cspsim/stores/Hardpoint.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/stores/StoresDynamics.h>
#include <csp/cspsim/stores/Rack.h>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/DataManager.h>

#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/osg.h>

#include <osg/PositionAttitudeTransform>

#include <algorithm>
#include <set>


CSP_NAMESPACE

CSP_XML_BEGIN(HardpointData)
	CSP_DEF("name", m_Name, true)
	CSP_DEF("external", m_External, true)
	CSP_DEF("fixed_store", m_FixedStore, false)
	CSP_DEF("compatibility", m_Compatibility, false)
	CSP_DEF("mount_compatibility", m_MountCompatibility, false)
	CSP_DEF("offset", m_Offset, false)
	CSP_DEF("attitude", m_Attitude, false)
	CSP_DEF("rack_preference", m_RackPreference, false)
CSP_XML_END

Hardpoint *HardpointData::createHardpoint(unsigned index) const {
	return new Hardpoint(this, index);
}

Hardpoint::Hardpoint(HardpointData const *data, unsigned index): m_Data(data), m_Fixed(false), m_Index(index) {
	assert(data);
	if (data->hasFixedStore()) {
		m_Store = data->createFixedStore();
		assert(m_Store.valid());
		m_Fixed = true;
	}
}

bool Hardpoint::mountStore(Store *store) {
	CSPLOG(INFO, OBJECT) << "mounting store";
	assert(store);
	CSPLOG(INFO, OBJECT) << "mounting store " << store->name();
	assert(m_Data.valid());
	if (!m_Data->isMountCompatible(store->key())) {
		CSPLOG(INFO, OBJECT) << "could not mount incompatible store " << store->name() << " on hardpoint " << name();
		return false;
	}
	if (m_Store.valid()) {
		CSPLOG(INFO, OBJECT) << "could not mount store " << store->name() << "; hardpoint " << name() << " already has a mounted store";
		return false;
	}
	CSPLOG(INFO, OBJECT) << "mounting " << store->name() << " on hardpoint " << name();
	store->m_Index = StoreIndex::Hardpoint(m_Index);  // FIXME kludge
	m_Store = store;
	return true;
}

bool Hardpoint::removeStore(StoreIndex const &idx, Store::RefT &store, Vector3 &offset, Quat &attitude, Vector3 &velocity, Vector3 &angular_velocity) {
	if (!m_Store) return false;
	StoreIndex hpindex = StoreIndex::Hardpoint(m_Index);
	offset = m_Data->offset();
	attitude = m_Data->attitude();
	if (idx == hpindex) {
		store = m_Store;
		m_Store = 0;
	} else {
		Store *parent = m_Store->findStore(idx.parent());
		if (!parent || !parent->asRack()) return false;
		store = parent->findStore(idx);
		if (!store) return false;
		parent->asRack()->setChild(idx, 0);

		unsigned i = hpindex.relativeChild(idx);
		RackMount const *mount = m_Store->asRack()->mount(i);
		mount->applyGeometry(offset, attitude);
		if (parent != m_Store.get()) {
			mount = parent->asRack()->mount(idx.index());
			mount->applyGeometry(offset, attitude);
		}
		velocity = mount->ejectVelocity();
		angular_velocity = mount->ejectAngularVelocity();
	}
	return true;
}


void Hardpoint::addModel(osg::Group *group) const {
	assert(group);
	if (!m_Store) return;
	osg::PositionAttitudeTransform *hp = new osg::PositionAttitudeTransform;
	hp->setPosition(toOSG(m_Data->offset()));
	hp->setAttitude(toOSG(m_Data->attitude()));
	m_Store->addModel(hp);
	group->addChild(hp);
}

CSP_NAMESPACE_END

