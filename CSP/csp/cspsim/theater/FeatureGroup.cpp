// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file FeatureGroup.cpp
 *
 **/


#include "Theater/FeatureGroup.h"
#include "Theater/FeatureObjectModel.h"
#include "Theater/FeatureSceneGroup.h"
#include "Theater/LayoutTransform.h"
#include "Theater/ElevationCorrection.h"
#include "TerrainObject.h"
#include "Projection.h"

#include <SimCore/Util/Log.h>
#include <csp/lib/data/ObjectInterface.h>

#include <osg/Vec3>
#include <osg/Quat>


SIMDATA_XML_BEGIN(FeatureGroup)
	SIMDATA_DEF("model", m_Model, true)
	SIMDATA_DEF("position", m_Position, true)
	SIMDATA_DEF("orientation", m_Orientation, true)
	/*SIMDATA_DEF("x", m_X, true)*/
	/*SIMDATA_DEF("y", m_Y, true)*/
SIMDATA_XML_END


simdata::Vector3 FeatureGroup::getGlobalPosition() const {
	return simdata::Vector3(m_X, m_Y, 0.0);
}

FeatureSceneGroup* FeatureGroup::getSceneGroup() {
	return m_SceneGroup.get();
}

void FeatureGroup::onEnterScene() {
	CSP_LOG(APP, DEBUG, "FeatureGroup @ " << getGlobalPosition() << ": adding " << m_Model->getFeatureCount() << " feature(s) to the scene");
}

void FeatureGroup::onLeaveScene() {
	CSP_LOG(APP, DEBUG, "FeatureGroup @ " << getGlobalPosition() << ": removing " << m_Model->getFeatureCount() << " feature(s) from the scene");
	// our scene graph is no longer needed
	m_SceneGroup = NULL;
}

FeatureSceneGroup* FeatureGroup::makeSceneGroup(simdata::Vector3 const &origin, TerrainObject *terrain) {
	assert(!m_SceneGroup);
	m_SceneGroup = new FeatureSceneGroup();
	m_Model->addSceneModel(m_SceneGroup.get(), LayoutTransform(), ElevationCorrection(terrain, m_X, m_Y, m_Orientation));
	m_SceneGroup->setPosition(osg::Vec3(m_X-origin.x(), m_Y-origin.y(), -origin.z()));
	m_SceneGroup->setAttitude(osg::Quat(m_Orientation, osg::Z_AXIS));
	return m_SceneGroup.get();
}

void FeatureGroup::onAggregate() {
}

void FeatureGroup::onDeaggregate() {
}

FeatureGroup::FeatureGroup(): SimObject(TYPE_STATIC) {
	m_X = 0.0;
	m_Y = 0.0;
	m_Orientation = 0.0;
	m_SceneGroup = NULL;
}

FeatureGroup::~FeatureGroup() {
}

void FeatureGroup::postCreate() {
	m_Features.reserve(m_Model->getFeatureCount());
	assert(m_Model.valid());
	m_Model->makeFeatures(m_Features, 0);
}

void FeatureGroup::project(Projection const &map) {
	//std::cout << "feature group @ " << m_Position << std::endl;
	simdata::Vector3 place = map.convert(m_Position);
	//std::cout << "             -> " << place << std::endl;
	//std::cout << "             -> " << map.getCenter() << std::endl;
	m_X = place.x();
	m_Y = place.y();
}


