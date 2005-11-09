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


#include <CSPSim.h>
#include <DynamicObject.h>
#include <Stores/Stores.h>
#include <SimData/ObjectInterface.h>
#include <SimData/DataManager.h>


SIMDATA_XML_BEGIN(HardpointData)
	SIMDATA_DEF("name", m_Name, true)
	SIMDATA_DEF("external", m_External, true)
	SIMDATA_DEF("fixed_store", m_FixedStore, false)
	SIMDATA_DEF("compatibility", m_Compatibility, false)
	SIMDATA_DEF("mount_compatibility", m_MountCompatibility, false)
	SIMDATA_DEF("offset", m_Offset, false)
	SIMDATA_DEF("attitude", m_Attitude, false)
	SIMDATA_DEF("rack_preference", m_RackPreference, false)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(MissileData)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(FuelTankData)
	SIMDATA_DEF("capacity", m_Capacity, true)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(RackData)
	SIMDATA_DEF("mounts", m_Mounts, true)
	SIMDATA_DEF("default_compatibility", m_DefaultCompatibility, false)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(RackMount)
	SIMDATA_DEF("offset", m_Offset, true)
	SIMDATA_DEF("attitude", m_Attitude, false)
	SIMDATA_DEF("compatibility", m_Compatibility, false)
	SIMDATA_DEF("fixed", m_Fixed, false)
	SIMDATA_DEF("eject_velocity", m_EjectVelocity, false)
	SIMDATA_DEF("eject_angular_velocity", m_EjectAngularVelocity, false)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(StoreData)
	SIMDATA_DEF("name", m_Name, true)
	SIMDATA_DEF("mass", m_Mass, true)
	SIMDATA_DEF("unit_inertia", m_UnitInertia, true)
	SIMDATA_DEF("cg_offset", m_CgOffset, true)
	SIMDATA_DEF("drag_factor", m_DragFactor, true)
	SIMDATA_DEF("model", m_Model, false)
	SIMDATA_DEF("object", m_Object, false)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(StoreCompatibilityTable)
	SIMDATA_DEF("stores", m_StoreVector, true)
SIMDATA_XML_END


const StoreIndex StoreIndex::Invalid;

double Store::mass() const {
	return data()->mass();
}

simdata::Matrix3 const &Store::unitInertia() const {
	return data()->unitInertia();
}

simdata::Vector3 const &Store::cgOffset() const {
	return data()->cgOffset();
}

void Store::sumDynamics(StoresDynamics &dynamics, simdata::Vector3 const &offset, simdata::Quat const &attitude, bool no_drag) const {
	data()->sumDynamics(dynamics, offset, attitude, no_drag);
}

void Store::addModel(osg::Group *group) const {
	m_ParentGroup = group;
	if (group) group->addChild(data()->makeModel());
}

simdata::Ref<DynamicObject> StoreData::createObject() const {
	CSPSim *sim = CSPSim::theSim;
	if (!sim || m_Object.isNone()) return 0;
	simdata::DataManager &manager = sim->getDataManager();
	return manager.getObject(m_Object);
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

void Rack::sumDynamics(StoresDynamics &dynamics, simdata::Vector3 const &offset, simdata::Quat const &attitude, bool no_drag) const {
	Store::sumDynamics(dynamics, offset, attitude, no_drag);
	for (unsigned i = 0; i < m_Data->capacity(); ++i) {
		if (m_Children[i].valid()) {
			simdata::Vector3 child_offset = offset + attitude.rotate(m_Data->mount(i)->offset());
			simdata::Quat child_attitude = attitude * m_Data->mount(i)->attitude();
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
				mount->setPosition(simdata::toOSG(m_Data->mount(i)->offset()));
				mount->setAttitude(simdata::toOSG(m_Data->mount(i)->attitude()));
				m_Children[i]->addModel(mount);
				group->addChild(mount);
			} else {
				m_Children[i]->addModel(group);
			}
		}
	}
}
