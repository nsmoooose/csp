// Combat Simulator Project - FlightSim Demo
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


#include "SimpleSceneManager.h"
#include "VirtualScene.h"
#include "DynamicObject.h"
#include "Theater/FeatureGroup.h"
#include "ObjectModel.h"
#include "Animation.h"

#include <SimData/Vector3.h>


// random optimization note.  iterating through all elements of a map is
// about 10 times slower than using a vector, when compiled with g++ -O2.
// without optimization, the iteration times are comparable.  for example,
// summing integer elements in a map takes about 30 ns per element at
// -O2 on a 2.5 GHz Athlon.  this is fast enough for the iterations the
// scene manager needs to perform each frame, and offers significant
// advantages when removing elements (about 2 orders of magnitude faster
// for 1000 elements).


void SimpleSceneManager::scheduleHideFeature(simdata::Ref<FeatureGroup> feature) {
	m_Scene->removeFeature(feature);
	// TODO feature->discardSceneGroup() ??
}

void SimpleSceneManager::scheduleShowFeature(simdata::Ref<FeatureGroup> feature) {
	CSP_LOG(SCENE, DEBUG, "scheduling show feature " << *feature);
	FeatureSceneGroup *scene_group = feature->getSceneGroup();
	if (scene_group == 0) {
		simdata::Vector3 origin = m_Scene->getFeatureOrigin(feature);
		TerrainObject *terrain = m_Scene->getTerrain().get();
		CSP_LOG(SCENE, DEBUG, "constructing feature scene group @ " << origin);
		scene_group = feature->makeSceneGroup(origin, terrain);
	}
	CSP_LOG(SCENE, DEBUG, "adding feature to scene");
	m_Scene->addFeature(feature);
}

void SimpleSceneManager::scheduleHideDynamic(simdata::Ref<DynamicObject> dynamic) {
	m_Scene->removeObject(dynamic);
}

void SimpleSceneManager::scheduleShowDynamic(simdata::Ref<DynamicObject> dynamic) {
	m_Scene->addObject(dynamic);
}

void SimpleSceneManager::scheduleVisibilityChange(ObjectRef const& object, bool hide) {
	CSP_LOG(SCENE, DEBUG, "setting visibility for " << *object << " to " << !hide);
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
		simdata::Ref<FeatureGroup> feature = static_cast<FeatureGroup*>(object.get());
		if (hide) {
			scheduleHideFeature(feature);
		} else {
			scheduleShowFeature(feature);
		}
	} else {
		// non-static objects will be DynamicObject subclasses, so do a
		// static cast rather than using Ref's dynamic_cast.
		simdata::Ref<DynamicObject> dynamic = static_cast<DynamicObject*>(object.get());
		if (hide) {
			scheduleHideDynamic(dynamic);
		} else {
			scheduleShowDynamic(dynamic);
		}
	}
}


SimpleSceneManager::SimpleSceneManager(simdata::Ref<VirtualScene> scene, double visibility_range):
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

void SimpleSceneManager::setCamera(simdata::Vector3 const &eye_point, simdata::Vector3 const &look_pos, simdata::Vector3 const &up_vec) {
	m_Scene->_setLookAt(eye_point, look_pos, up_vec);
}


