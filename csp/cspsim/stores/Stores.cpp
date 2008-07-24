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
 * @file Stores.cpp
 *
 **/

#include <csp/cspsim/stores/Stores.h>

#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/ObjectModel.h>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/DataManager.h>

#include <csp/cspsim/stores/StoresDynamics.h>

#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Path.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Real.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/osg.h>

#include <osg/PositionAttitudeTransform>

namespace csp {

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

void StoreData::sumDynamics(StoresDynamics &dynamics, Vector3 const &offset, Quat const &attitude, double extra_mass, bool no_drag) const {
	const double mass = m_Mass + extra_mass;
	const Vector3 cg = attitude.rotate(m_CgOffset) + offset;  // center of gravity in body coordinates
	const Matrix3 i = attitude.getMatrix3() * m_UnitInertia;
	dynamics.addMassAndInertia(mass, i, cg);
	if (!no_drag) {
		dynamics.addDrag(m_DragFactor, cg);  // should be front of store, but cg is probably close enough
	}
}

} // namespace csp

