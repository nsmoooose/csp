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
 * @file csplib/sound/Sample.h
 * @brief Object class for sound samples and related utilities.
 **/


#ifndef __CSPLIB_SOUND_SAMPLE_H__
#define __CSPLIB_SOUND_SAMPLE_H__

#include <csp/csplib/data/External.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector3.h>

#include <openalpp/ref_ptr.h>
#include <osg/ref_ptr>

namespace openalpp { class Sample; }
namespace osgAL { class SoundState; }


CSP_NAMESPACE

/** Object class for representing openAL sound samples.  Allows the sound
 *  file as well as standard openAL parameters to be specified using XML.
 */
class SoundSample: public Object {
public:
	CSP_DECLARE_STATIC_OBJECT(SoundSample)

	osgAL::SoundState *getSoundState() const;
	osgAL::SoundState const *getSharedSoundState() const;
	int priority() const { return m_Priority; }
	std::string const &getName() const { return m_Name; }
	std::string const &getFilename() const { return m_Filename.getSource(); }

	SoundSample();

protected:
	virtual void postCreate();

private:
	External m_Filename;
	std::string m_Name;
	bool m_Looping;
	bool m_Ambient;
	bool m_Relative;
	Vector3 m_Direction;
	Vector3 m_Position;
	double m_Gain;
	double m_ReferenceDistance;
	double m_MaxDistance;
	double m_RolloffFactor;
	double m_Pitch;
	double m_InnerAngle;
	double m_OuterAngle;
	double m_OuterGain;
	int m_Priority;

	mutable osg::ref_ptr<osgAL::SoundState> m_State;
	mutable openalpp::ref_ptr<openalpp::Sample> m_Sample;

	void loadSample() const;
};


CSP_NAMESPACE_END

#endif // __CSPLIB_SOUND_SAMPLE_H__

