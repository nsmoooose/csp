// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file SimpleSceneManager.cpp
 *
 **/

#include <csp/cspsim/SimpleSceneManager.h>
#include <csp/cspsim/DynamicObject.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/VirtualScene.h>
#include <csp/cspsim/theater/FeatureGroup.h>

#include <csp/csplib/data/Vector3.h>

namespace csp {

// random optimization note.  iterating through all elements of a map is
// about 10 times slower than using a vector, when compiled with g++ -O2.
// without optimization, the iteration times are comparable.  for example,
// summing integer elements in a map takes about 30 ns per element at
// -O2 on a 2.5 GHz Athlon.  this is fast enough for the iterations the
// scene manager needs to perform each frame, and offers significant
// advantages when removing elements (about 2 orders of magnitude faster
// for 1000 elements).


void SimpleSceneManager::scheduleHideFeature(Ref<FeatureGroup> feature) {
	m_Scene->removeFeature(feature);
	// TODO feature->discardSceneGroup() ??
}

void SimpleSceneManager::scheduleShowFeature(Ref<FeatureGroup> feature) {
	CSPLOG(DEBUG, SCENE) << "scheduling show feature " << *feature;
	FeatureSceneGroup *scene_group = feature->getSceneGroup();
	if (scene_group == 0) {
		Vector3 origin = m_Scene->getFeatureOrigin(feature);
		TerrainObject *terrain = m_Scene->getTerrain().get();
		CSPLOG(DEBUG, SCENE) << "constructing feature scene group @ " << origin;
		scene_group = feature->makeSceneGroup(origin, terrain);
	}
	CSPLOG(DEBUG, SCENE) << "adding feature to scene";
	m_Scene->addFeature(feature);
}

void SimpleSceneManager::scheduleHideDynamic(Ref<DynamicObject> dynamic) {
	m_Scene->removeObject(dynamic);
}

void SimpleSceneManager::scheduleShowDynamic(Ref<DynamicObject> dynamic) {
	m_Scene->addObject(dynamic);
}

void SimpleSceneManager::scheduleVisibilityChange(ObjectRef const& object, bool hide) {
	CSPLOG(DEBUG, SCENE) << "setting visibility for " << *object << " to " << !hide;
	if (hide) {
		assert(object->isVisible());
		if (!object->isVisible()) return;
	} else {
		assert(!object->isVisible());
		if (object->isVisible()) return;
	}

	setVisible(object, !hide);

	if (object->isStatic()) {
		// static objects will be FeatureGroup subclasses, so do a
		// static cast rather than using Ref's dynamic_cast.
		Ref<FeatureGroup> feature = static_cast<FeatureGroup*>(object.get());
		if (hide) {
			scheduleHideFeature(feature);
		} else {
			scheduleShowFeature(feature);
		}
	} else {
		// non-static objects will be DynamicObject subclasses, so do a
		// static cast rather than using Ref's dynamic_cast.
		Ref<DynamicObject> dynamic = static_cast<DynamicObject*>(object.get());
		if (hide) {
			scheduleHideDynamic(dynamic);
		} else {
			scheduleShowDynamic(dynamic);
		}
	}
}

SimpleSceneManager::SimpleSceneManager(Ref<VirtualScene> scene, double visibility_range):
	SceneManager(visibility_range),
	m_Scene(scene)
{
	assert(scene.valid());
}

SimpleSceneManager::~SimpleSceneManager() {
}

void SimpleSceneManager::scheduleHide(ObjectRef const& object) {
	scheduleVisibilityChange(object, true /* hide */);
}

void SimpleSceneManager::scheduleShow(ObjectRef const& object) {
	scheduleVisibilityChange(object, false /* hide */);
}

void SimpleSceneManager::setCamera(Vector3 const &eye_point, Vector3 const &look_pos, Vector3 const &up_vec) {
	m_Scene->_setLookAt(eye_point, look_pos, up_vec);
}

} // namespace csp

