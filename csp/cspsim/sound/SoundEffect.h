#pragma once
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
 * @file SoundEffect.h
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <osg/ref_ptr>

namespace osgAL { class SoundState; }

namespace csp {

class SoundModel;
class SoundSample;

/** SoundEffect represents a sound sample that is bound to a sound model.
 *  A sound effect can be bound in one of three modes: external, internal,
 *  and headset, which determine how the sound is modified in response to
 *  the position of the camera.
 */
class SoundEffect: public Referenced {
public:
	typedef enum { EXTERNAL, INTERNAL, HEADSET } Mode;

	SoundEffect(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model, Mode mode);

	static SoundEffect* HeadsetSound(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model);
	static SoundEffect* InternalSound(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model);
	static SoundEffect* ExternalSound(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model);

	// TODO gain setting methods to merge volume and external/internal muting without using
	// the osgAL::SoundState occlusion interface.

	void play(bool on=true);
	osgAL::SoundState *state();

protected:
	virtual ~SoundEffect();

private:
	osg::ref_ptr<osgAL::SoundState> m_SoundState;
};

} // namespace csp
