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
 * @file ResourceBundle.cpp
 *
 * @brief Static object for indexing external resources (e.g. for sound samples).
 *
 **/

#include <csp/cspsim/ResourceBundle.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/sound/Sample.h>

CSP_NAMESPACE


CSP_XML_BEGIN(ResourceBundle)
	CSP_DEF("sounds", m_SoundSamples, false)
CSP_XML_END


ResourceBundle::ResourceBundle() { }
ResourceBundle::~ResourceBundle() { }

void ResourceBundle::postCreate() {
	Object::postCreate();
	for (unsigned i = 0; i < m_SoundSamples.size(); ++i) {
		Ref<SoundSample> sample = m_SoundSamples[i];
		if (!sample) {
			CSPLOG(ERROR, APP) << "Null sound sample in resource bundle " << i;
			continue;
		}
		if (!m_SoundSampleMap.insert(std::make_pair(sample->getName(), sample)).second) {
			CSPLOG(ERROR, APP) << "Duplicate sound sample name in resource bundle " << getPath() << " (" << sample->getName() << ")";
		}
	}
}

SoundSample const *ResourceBundle::getSoundSample(std::string const &name) const {
	SoundSampleMap::const_iterator iter = m_SoundSampleMap.find(name);
	if (iter == m_SoundSampleMap.end()) {
		CSPLOG(WARNING, APP) << "Could not find sound sample " << name << " in resource bundle " << getPath();
		return 0;
	}
	return iter->second.get();
}


CSP_NAMESPACE_END

