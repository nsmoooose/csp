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
 * @file PitchLadder.cpp
 *
 **/


#include <HUD/PitchLadder.h>
#include <cmath>


PitchLadder::PitchLadder(): m_Dirty(false), m_Hidden(false), m_LastPitchAngle(100.0f) { }

void PitchLadder::add(PitchBar bar, float angle) {
	bar->show(false);
	m_Ladder.push_back(PitchInfo(bar, osg::inDegrees(angle)));
	m_Dirty = true;
}

void PitchLadder::update(osg::Vec3 center_direction_body, osg::Vec3 const &right_direction_body, double pitch_angle) {
	if (!m_Hidden) {
		center_direction_body.normalize();
		float roll_angle = atan2(right_direction_body.z(), right_direction_body.x());
		if (m_Dirty || std::abs(pitch_angle - m_LastPitchAngle) > 0.044 /*~2.5 deg*/) {
			updateActive(pitch_angle);
		}
		for (unsigned i = 0; i < m_Active.size(); ++i) {
			PitchInfo &info = m_Ladder[m_Active[i]];
			float angle = info.second;
			info.first->setDirection(center_direction_body * osg::Matrix::rotate(angle, right_direction_body));
			info.first->setOrientation(-roll_angle);
		}
	}
}

void PitchLadder::updateActive(float pitch_angle) {
	if (!m_Hidden) {
		m_Active.clear();
		for (unsigned i = 0; i < m_Ladder.size(); ++i) {
			if (std::abs(m_Ladder[i].second - pitch_angle) < 0.35 /*~20 deg*/ && !m_Hidden) {
				m_Active.push_back(i);
				m_Ladder[i].first->show(true);
			} else {
				m_Ladder[i].first->show(false);
			}
		}
		m_Dirty = false;
		m_LastPitchAngle = pitch_angle;
	}
}

void PitchLadder::show(bool visible) {
	m_Dirty = m_Dirty || (visible && m_Hidden);
	m_Hidden = !visible;
	if (m_Hidden) {
		m_Active.clear();
		for (unsigned i = 0; i < m_Ladder.size(); ++i) {
			m_Ladder[i].first->show(false);
		}
	}
}
