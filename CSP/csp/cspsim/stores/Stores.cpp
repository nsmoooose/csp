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
 * @file Stores.cpp
 *
 **/


#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/DataManager.h>

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

CSP_XML_BEGIN(MissileData)
CSP_XML_END

CSP_XML_BEGIN(FuelTankData)
	CSP_DEF("capacity", m_Capacity, true)
CSP_XML_END

CSP_XML_BEGIN(RackData)
	CSP_DEF("mounts", m_Mounts, true)
	CSP_DEF("default_compatibility", m_DefaultCompatibility, false)
CSP_XML_END

CSP_XML_BEGIN(RackMount)
	CSP_DEF("offset", m_Offset, true)
	CSP_DEF("attitude", m_Attitude, false)
	CSP_DEF("compatibility", m_Compatibility, false)
	CSP_DEF("fixed", m_Fixed, false)
	CSP_DEF("eject_velocity", m_EjectVelocity, false)
	CSP_DEF("eject_angular_velocity", m_EjectAngularVelocity, false)
CSP_XML_END

CSP_XML_BEGIN(StoreData)
	CSP_DEF("name", m_Name, true)
	CSP_DEF("mass", m_Mass, true)
	CSP_DEF("unit_inertia", m_UnitInertia, true)
	CSP_DEF("cg_offset", m_CgOffset, true)
	CSP_DEF("drag_factor", m_DragFactor, true)
	CSP_DEF("model", m_Model, false)
	CSP_DEF("object", m_Object, false)
CSP_XML_END

CSP_XML_BEGIN(StoreCompatibilityTable)
	CSP_DEF("stores", m_StoreVector, true)
CSP_XML_END


const StoreIndex StoreIndex::Invalid;

double Store::mass() const {
	return data()->mass();
}

Matrix3 const &Store::unitInertia() const {
	return data()->unitInertia();
}

Vector3 const &Store::cgOffset() const {
	return data()->cgOffset();
}

void Store::sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, bool no_drag) const {
	data()->sumDynamics(dynamics, offset, attitude, no_drag);
}

void Store::addModel(osg::Group *group) const {
	m_ParentGroup = group;
	if (group) group->addChild(data()->makeModel());
}

StoreData::StoreData(Type type): m_Type(type) {
}

StoreData::~StoreData() {
}

Ref<DynamicObject> StoreData::createObject() const {
	CSPSim *sim = CSPSim::theSim;
	if (!sim || m_Object.isNone()) return 0;
	DataManager &manager = sim->getDataManager();
	return manager.getObject(m_Object);
}

osg::Node *StoreData::makeModel() const {
	// FIXME should/can ObjectModel provide const access to the base model?
	ObjectModel *model = const_cast<ObjectModel*>(m_Model.get());
	return !model ? 0 : model->getModel().get();
}

Ref<ObjectModel> StoreData::getModel() const {
	return m_Model;
}

Store *FuelTankData::createStore() const {
	return new FuelTank(this);
}

Store *MissileData::createStore() const {
	return new Missile(this);
}

Hardpoint *HardpointData::createHardpoint(unsigned index) const {
	return new Hardpoint(this, index);
}


Store *RackData::createStore() const {
	return new Rack(this);
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

CSP_NAMESPACE_END

