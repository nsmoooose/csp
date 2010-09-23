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
 * @file VoiceMessageUnit.h
 *
 * @brief Implements the F-16 Voice Message Unit (aka Bitchin' Betty).
 * 
 * @TODO generalize this implementation to work with arbitrary triggers and sounds.
 **/


#ifndef __F16_VOICEMESSAGEUNIT_H__
#define __F16_VOICEMESSAGEUNIT_H__

#include <csp/cspsim/System.h>

namespace csp {

class VoiceMessageUnit: public System, public sigc::trackable {
public:
	CSP_DECLARE_OBJECT(VoiceMessageUnit)

	VoiceMessageUnit();

	virtual void registerChannels(Bus* bus);
	virtual void importChannels(Bus* bus);

private:
	DataChannel<bool>::CRefT bindChannel(Bus *bus, std::string const &channel_name, void (VoiceMessageUnit::*handler)());
	void playMessage(Ref<SoundEffect> const &effect, bool active);

	void onAltitudeAdvisory();
	void onDescentWarningAfterTakeoff();

	Ref<SoundEffect> m_AltitudeSoundEffect;

	DataChannel<bool>::CRefT b_AltitudeAdvisory;
	DataChannel<bool>::CRefT b_DescentWarningAfterTakeoff;
};


} // namespace csp

#endif // __F16_VOICEMESSAGEUNIT_H__

