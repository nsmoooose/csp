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
 * @file SoundEngine.h
 *
 **/


#ifndef __SOUNDENGINE_H__
#define __SOUNDENGINE_H__


#include <csp/csplib/util/Namespace.h>
#include <osg/ref_ptr>

namespace osgAL { class SoundManager; }
namespace osgAL { class SoundRoot; }

namespace csp {

/** A singleton that provides access to global osgAL state.
 */
class SoundEngine {
public:
	static SoundEngine &getInstance();
	void initialize();
	void shutdown();
	osgAL::SoundManager *getManager();
	osgAL::SoundRoot *getSoundRoot();
	bool getSoundEnabled();
	void mute();
	void unmute();

private:
	SoundEngine();
	virtual ~SoundEngine();
	void createSoundRoot();
	osg::ref_ptr<osgAL::SoundRoot> m_SoundRoot;
	bool m_SoundEnabled;
};

} // namespace csp

#endif // __SOUNDENGINE_H__

