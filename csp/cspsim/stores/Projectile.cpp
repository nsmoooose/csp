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
 * @file Projectile.cpp
 *
 **/

#include <csp/cspsim/stores/Projectile.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/cspsim/SceneModel.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/StringTools.h>

namespace csp {

CSP_XML_BEGIN(Projectile)
CSP_XML_END

// TODO SimObject probably needs a specialized type for Projectiles.
Projectile::Projectile(): DynamicObject(TYPE_AIR_UNIT), m_DetachedModel(false) {
}

Projectile::~Projectile() {
}

void Projectile::prepareRelease(Ref<DynamicObject> const &/*parent*/, Ref<Store> const &store) {
	assert(store.valid() && !m_Store);
	m_Store = store;
	setReferenceMass(store->mass());
	setReferenceInertia(store->mass() * store->unitInertia());
	setReferenceCgOffset(store->cgOffset());
}

void Projectile::onEnterScene() {
	DynamicObject::onEnterScene();
	if (m_Store.valid() && !m_DetachedModel) {
		// remove store 3d model from parent, as it will be replaced by this
		// object's 3d model.
		osg::Group *group = m_Store->getParentGroup();
		if (group) {
			assert(group->getNumParents() == 1);
			group->getParent(0)->asGroup()->removeChild(group);
			m_DetachedModel = true;
		}
	}
}

void Projectile::createSceneModel() {
	DynamicObject::createSceneModel();
	if (m_SceneModel.valid()) {
		std::string label = m_Store->name();
		ConvertStringToUpper(label);  // label font currently doesn't support lowercase
		m_SceneModel->setLabel(label);
	}
}

} // namespace csp

