// Combat Simulator Project
// Copyright (C) 2003 The Combat Simulator Project
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
 * @file System.cpp
 *
 * Vehicle systems base classes and infrastructure.
 *
 **/


#include <csp/cspsim/System.h>
#include <csp/cspsim/Bus.h>
#include <csp/cspsim/ResourceBundle.h>
#include <csp/cspsim/SoundModel.h>
#include <csp/cspsim/SystemsModel.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/sound/Sample.h>


CSP_NAMESPACE

CSP_XML_BEGIN(System)
	CSP_DEF("subsystems", m_Subsystems, false)
	CSP_DEF("resources", m_ResourceBundle, false)
CSP_XML_END


System::System(): m_Model(0) {
}

System::~System() {
}

void System::setModel(SystemsModel *model) {
	assert(m_Model == 0);
	assert(model != 0);
	m_Model = model;
	Ref<Bus> bus = model->getBus();
	registerChannels(bus.get());
}

void System::postCreate() {
	// This method is called automatically after the system is instantiated
	// and deserialized from static data.  All the subsystems defined in XML
	// have been instantiated in the m_Subsystems vector, so now we add them
	// as child nodes and clear m_Subsystems to eliminate the extra set
	// of references.
	CSPLOG(DEBUG, OBJECT) << "System::postCreate() " << getClassName() << ", adding " << m_Subsystems.size() << " subsystems.";
	Link<System>::vector::iterator iter = m_Subsystems.begin();
	for (; iter != m_Subsystems.end(); ++iter) {
		CSPLOG(DEBUG, OBJECT) << "System::addChild() " << (*iter)->getClassName();
		addChild(iter->get());
	}
	m_Subsystems.clear();
}

bool System::addChild(SystemNode *node) {
	if (!SystemNode::addChild(node)) {
		CSPLOG(ERROR, OBJECT) << "SystemNode::addChild() failed.";
		return false;
	}
	return true;
}

void System::onInternalView(bool) { }
void System::onAttachSceneModel(SceneModel*) { }
void System::onDetachSceneModel(SceneModel*) { }

SoundSample const *System::getSoundSample(std::string const &name) const {
	if (!m_ResourceBundle) {
		CSPLOG(ERROR, OBJECT) << "No resource bundle in system " << getClassName() << " for sound " << name;
		return 0;
	}
	return m_ResourceBundle->getSoundSample(name);
}

SoundEffect *System::addSoundEffect(std::string const &name, SoundEffect::Mode mode) {
	assert(m_Model);
	Ref<const SoundSample> sample(getSoundSample(name));
	Ref<SoundModel> model = m_Model->getSoundModel();
	if (!sample || !model) return 0;
	switch (mode) {
		case SoundEffect::HEADSET:
			return SoundEffect::HeadsetSound(sample, model);
		case SoundEffect::INTERNAL:
			return SoundEffect::InternalSound(sample, model);
		case SoundEffect::EXTERNAL:
			return SoundEffect::ExternalSound(sample, model);
		default:
			CSPLOG(ERROR, AUDIO) << "Unknown sound effect mode " << mode;
	}
	return 0;
}

void System::removeSoundEffect(Ref<SoundEffect> const &sound) {
	Ref<SoundModel> model = m_Model->getSoundModel();
	if (model.valid()) {
		model->removeSound(sound);
	}
}

void System::removeAndDeleteSoundEffect(Ref<SoundEffect> &sound) {
	removeSoundEffect(sound);
	sound = 0;
}

void System::BindVisitor::apply(System &s) {
	s.importChannels(m_Bus);
	traverse(s);
}

void System::InitVisitor::apply(System &s) {
	s.setModel(m_Model);
	traverse(s);
}

CSP_NAMESPACE_END

