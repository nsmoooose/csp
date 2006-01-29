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
 * @file SoundModel.cpp
 *
 **/


#include <csp/cspsim/sound/SoundModel.h>
#include <csp/cspsim/sound/Sample.h>
#include <csp/cspsim/sound/SoundEffect.h>
#include <csp/cspsim/SceneModel.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/osg.h>

#include <osg/MatrixTransform>
#include <osgAL/SoundNode>
#include <osgAL/SoundState>
#include <csp/csplib/util/undef.h>

#include <cassert>

CSP_NAMESPACE

SoundModel::SoundModel(): m_SoundGroup(new osg::Group), m_InternalView(false) {
}

SoundModel::~SoundModel() { }

void SoundModel::bind(SceneModel *scene_model) {
	if (scene_model) {
		CSPLOG(INFO, AUDIO) << "bind sound model";
		osg::Group *group = scene_model->getDynamicGroup();
		if (group) group->addChild(m_SoundGroup.get());
	}
}

void SoundModel::unbind(SceneModel *scene_model) {
	if (scene_model) {
		CSPLOG(INFO, AUDIO) << "unbind sound model";
		osg::Group *group = scene_model->getDynamicGroup();
		if (group) group->removeChild(m_SoundGroup.get());
	}
}

void SoundModel::addExternalSound(osgAL::SoundState *sound, Vector3 const &offset, Vector3 const &direction) {
	assert(sound && !sound->getName().empty());
	sound->setOccludeDampingFactor(0.9);
	sound->setOccludeScale(m_InternalView ? 0.0f : 1.0f);
	sound->setOccluded(true);
	SoundData data;
	data.sound_node = new osgAL::SoundNode(sound);
	data.scene_node = placeNode(data.sound_node, offset, direction);
	data.mode = EXTERNAL;
	const bool duplicate = !m_SoundIndex.insert(std::make_pair(sound->getName(), data)).second;
	if (duplicate) CSPLOG(FATAL, AUDIO) << "Duplicate sound " << sound->getName();
	m_SoundGroup->addChild(data.scene_node.get());
}

void SoundModel::addInternalSound(osgAL::SoundState *sound, Vector3 const &offset, Vector3 const &direction) {
	assert(sound && !sound->getName().empty());
	sound->setOccludeDampingFactor(0.9);
	sound->setOccludeScale(m_InternalView ? 1.0f : 0.0f);
	sound->setOccluded(true);
	SoundData data;
	data.sound_node = new osgAL::SoundNode(sound);
	data.scene_node = placeNode(data.sound_node, offset, direction);
	data.mode = INTERNAL;
	const bool duplicate = !m_SoundIndex.insert(std::make_pair(sound->getName(), data)).second;
	if (duplicate) CSPLOG(FATAL, AUDIO) << "Duplicate sound " << sound->getName();
	m_SoundGroup->addChild(data.scene_node.get());
}

void SoundModel::addHeadsetSound(osgAL::SoundState *sound) {
	assert(sound && !sound->getName().empty());
	sound->setOccludeDampingFactor(1.0);
	sound->setOccludeScale(m_InternalView ? 1.0f : 0.0f);
	sound->setOccluded(true);
	sound->setRelative(true);
	SoundData data;
	data.sound_node = new osgAL::SoundNode(sound);
	data.scene_node = data.sound_node.get();
	data.mode = HEADSET;
	const bool duplicate = !m_SoundIndex.insert(std::make_pair(sound->getName(), data)).second;
	if (duplicate) CSPLOG(FATAL, AUDIO) << "Duplicate sound " << sound->getName();
	m_SoundGroup->addChild(data.scene_node.get());
}

bool SoundModel::removeSound(osgAL::SoundState *sound) {
	assert(sound && !sound->getName().empty());
	SoundIndex::iterator iter = m_SoundIndex.find(sound->getName());
	if (iter == m_SoundIndex.end()) {
		CSPLOG(WARNING, AUDIO) << "Sound node '" << sound->getName() << "' not found";
		return false;
	}
	Node node = iter->second.scene_node;
	m_SoundIndex.erase(iter);
	if (!m_SoundGroup->removeChild(node.get())) {
		CSPLOG(ERROR, AUDIO) << "Sound node '" << sound->getName() << "' not found in scene graph";
	}
	return true;
}

bool SoundModel::removeSound(Ref<SoundEffect> const &effect) {
	return !effect ? false : removeSound(effect->state());
}

bool SoundModel::removeAndDeleteSound(Ref<SoundEffect> &effect) {
	const bool ok = removeSound(effect);
	effect = 0;
	return ok;
}

osg::Node* SoundModel::placeNode(SoundNode &sound_node, Vector3 const &offset, Vector3 const &direction) const {
	osg::Node *node = sound_node.get();
	if (offset != Vector3::ZERO || direction != Vector3::YAXIS) {
		osg::Vec3 pos = toOSG(offset);
		osg::Vec3 dir = toOSG(direction);
		osg::Vec3 up = dir ^ osg::Vec3(0,1,0);
		if (up.length() == 0.0) up = dir ^ osg::Vec3(0,0,1);
		osg::MatrixTransform *mat = new osg::MatrixTransform(osg::Matrix::lookAt(pos, pos + dir, up));
		mat->addChild(node);
		node = mat;
	}
	return node;
}

void SoundModel::setInternalView(bool internal_view) {
	if (internal_view == m_InternalView) return;
	m_InternalView = internal_view;
	for (SoundIndex::iterator iter = m_SoundIndex.begin(); iter != m_SoundIndex.end(); ++iter) {
		SoundData &data = iter->second;
		if (data.sound_node.valid()) {
			osgAL::SoundState *state = data.sound_node->getSoundState();
			bool is_internal_sound = (data.mode != EXTERNAL);
			if (state) {
				state->setOccludeScale(is_internal_sound == internal_view ? 1.0 : 0.0);
				state->setOccluded(true);  // necessary to apply the new occlude_scale value
			}
		}
	}
}

CSP_NAMESPACE_END

