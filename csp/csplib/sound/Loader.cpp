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
 * @file csplib/sound/Loader.cpp
 * @brief Base class and registry for loading sound samples.
 **/

#include <csp/csplib/sound/Loader.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/FileUtility.h>

#include <openalpp/sample.h>

#include <map>
#include <cassert>

#include <csp/csplib/util/undef.h>

CSP_NAMESPACE

namespace {

	// Global registry of loaders for various sound formats.
	class SoundRegistry {
	public:
		typedef std::map<std::string, SoundFileLoader*> LoaderMap;
		static LoaderMap &map() {
			if (!_instance) _instance = new SoundRegistry;
			return _instance->_map;
		}
	private:
		static SoundRegistry *_instance;
		LoaderMap _map;
	};
	
	SoundRegistry * SoundRegistry::_instance = 0;

	// Force the SoundRegistry singleton to be initialized during static construction, before any
	// threads start.  This is a bit of overkill, since normally any sound loaders that register
	// during static construction will initialize the registry.
	struct InitializeRegistry { InitializeRegistry() { SoundRegistry::map(); } } _sound_registry_init;

} // namespace


void SoundFileLoader::registerExtension(SoundFileLoader *loader, std::string const &extension) {
	assert(loader);
	CSPLOG(ERROR, AUDIO) << "Registering sound sample loader for extension " << extension;
	SoundRegistry::map().insert(SoundRegistry::LoaderMap::value_type(extension, loader));
}

void SoundFileLoader::unregisterExtension(SoundFileLoader *loader, std::string const &extension) {
	SoundRegistry::LoaderMap::iterator iter = SoundRegistry::map().find(extension);
	if (iter != SoundRegistry::map().end() && iter->second == loader)  SoundRegistry::map().erase(iter);
}

SoundFileLoader::SoundFileLoader() { }
SoundFileLoader::~SoundFileLoader() { }


openalpp::Sample *SoundFileLoader::load(std::string const &filename) {
	if (!initialized) {
		CSPLOG(ERROR, AUDIO) << "SoundFileLoader::init() not called.";
		return 0;
	}
	std::string ext = ospath::ext(filename);
	SoundRegistry::LoaderMap::const_iterator iter = SoundRegistry::map().find(ext);
	if (iter == SoundRegistry::map().end()) {
		CSPLOG(ERROR, AUDIO) << "No loader found for extension '" << ext << "' when loading " << filename;
		return 0;
	}
	return iter->second->_load(filename);
}

bool SoundFileLoader::initialized = false;

// Since simcore is currently build as a static library we need to explicitly
// reference the individual loaders to avoid dead-code elimination by the
// linker.  Ultimately we probably want to switch to a plugin mechanism using
// shared libraries and use osgDB::DynamicLibrary to manage runtime loading.
void SoundFileLoader::init() {
	__registerOgg();
	initialized = true;
}


CSP_NAMESPACE_END


/* test code:

#include <openalpp/alpp.h>

int main() {
	openalpp::ref_ptr<openalpp::Sample> sample = SoundFileLoader::load("/home/mrose/tmp/test.ogg");
	if (sample.valid()) {
		CSPLOG(ERROR, AUDIO) << "loaded!";
		openalpp::Source *source = new openalpp::Source;
		openalpp::Listener *listener = new openalpp::Listener(0,0,0);
		listener->select();
		source->setSound(sample.get());
		source->setLooping();
		source->play();
		sleep(5);
	}
}
*/
