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
 * @file cspsim/sound/Sample.cpp
 * @brief Object class for sound samples and related utilities.
 **/


#include <csp/cspsim/sound/Sample.h>
#include <csp/cspsim/sound/Loader.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/osg.h>

#include <osgAL/SoundState>
#include <openalpp/Sample>
#include <csp/csplib/util/undef.h>


namespace csp {


CSP_XML_BEGIN(SoundSample)
	CSP_DEF("filename", m_Filename, true)
	CSP_DEF("name", m_Name, false)
	CSP_DEF("looping", m_Looping, false)
	CSP_DEF("relative", m_Relative, false)
	CSP_DEF("gain", m_Gain, false)
	CSP_DEF("direction", m_Direction, false)
	CSP_DEF("position", m_Position, false)
	CSP_DEF("reference_distance", m_ReferenceDistance, false)
	CSP_DEF("max_distance", m_MaxDistance, false)
	CSP_DEF("rolloff_factor", m_RolloffFactor, false)
	CSP_DEF("pitch", m_Pitch, false)
	CSP_DEF("inner_angle", m_InnerAngle, false)
	CSP_DEF("outer_angle", m_OuterAngle, false)
	CSP_DEF("outer_gain", m_OuterGain, false)
	CSP_DEF("priority", m_Priority, false)
CSP_XML_END


void SoundSample::postCreate() {
	/** @TODO is m_Name really useful at all? */
	if (m_Name.empty()) m_Name = ospath::basename(m_Filename.getSource());
	m_State = new osgAL::SoundState(m_Name);
	m_State->setLooping(m_Looping);
	m_State->setAmbient(m_Ambient);
	m_State->setRelative(m_Relative);
	m_State->setDirection(toOSG(m_Direction));
	m_State->setPosition(toOSG(m_Position));
	m_State->setGain(m_Gain);
	m_State->setReferenceDistance(m_ReferenceDistance);
	m_State->setMaxDistance(m_MaxDistance);
	m_State->setRolloffFactor(m_RolloffFactor);
	m_State->setPitch(m_Pitch);
	/** @TODO soundcone seems to be broken --- at least i can't make sense of it. */
	if (m_OuterGain < 1.0) { // && m_OuterAngle < 180.0 && m_OuterAngle >= m_InnerAngle) {
		m_State->setSoundCone(m_InnerAngle, m_OuterAngle, m_OuterGain);
	}
}

osgAL::SoundState *SoundSample::getSoundState() const {
	if (!m_Sample) loadSample();
	osgAL::SoundState *state = new osgAL::SoundState(m_Name);
	*state = *m_State;
	/** @bug for some reason, the ancient version (~2004) of osgal I have doesn't copy maxdistance correctly.
	 */
	state->setMaxDistance(m_State->getMaxDistance());
	return state;
}

osgAL::SoundState const *SoundSample::getSharedSoundState() const {
	if (!m_Sample) loadSample();
	return m_State.get();
}

void SoundSample::loadSample() const {
	assert(!m_Sample);
	try
	{
		m_Sample = SoundFileLoader::load(m_Filename.getSource());
		/** @TODO on failure, load a dummy sound */
		m_State->setSample(m_Sample.get());	
	}
	catch (std::exception & e)
	{
		CSPLOG(ERROR, AUDIO) << "Loading sample " << m_Filename << " failed. (" << e.what() << ")";
	}
}

SoundSample::SoundSample():
	m_Looping(false),
	m_Ambient(false),
	m_Relative(false),
	m_Direction(Vector3::YAXIS),
	m_Position(Vector3::ZERO),
	m_Gain(1.0),
	m_ReferenceDistance(1.0),
	m_MaxDistance(100.0),
	m_RolloffFactor(1.0),
	m_Pitch(1.0),
	m_InnerAngle(0.0),
	m_OuterAngle(180.0),
	m_OuterGain(1.0),
	m_Priority(0) {
}


} // namespace csp

