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
 * @file cspsim/sound/Loader.h
 * @brief Base class and registry for loading sound samples.
 **/


#ifndef __CSPSIM_SOUND_LOADER_H__
#define __CSPSIM_SOUND_LOADER_H__

#include <csp/cspsim/Export.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Export.h>
#include <string>

namespace openalpp { class Sample; }


namespace csp {


void CSPSIM_EXPORT __registerOgg();

/** A base class for loading sound samples.  Subclasses should implement
 *  _load to load samples in various formats, and call registerExtension
 *  from their constructor.  Create a single static instance of each
 *  loader subclass.
 */
class CSPSIM_EXPORT SoundFileLoader {
public:
	/** Initialize the sound loader.  Must be called before loading samples.
	 */
	static void init();

	/** Load a sound sample.  Returns a new sample on success, or NULL
	 *  on failure.
	 */
	static openalpp::Sample *load(std::string const &filename);

protected:
	/** Implement this method in subclasses to load a sound file and
	 *  convert it to an openAL++ sample.  Returns a new sample instance,
	 *  or NULL on failure.
	 */
	virtual openalpp::Sample * _load(std::string const &filename)=0;

	static void registerExtension(SoundFileLoader *loader, std::string const &ext);
	static void unregisterExtension(SoundFileLoader *loader, std::string const &ext);

	SoundFileLoader();
	virtual ~SoundFileLoader();

private:
	static bool initialized;
};


} // namespace csp

#endif // __CSPLIB_SOUND_LOADER_H__

