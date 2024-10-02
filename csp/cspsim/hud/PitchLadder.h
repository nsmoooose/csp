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
 * @file PitchLadder.h
 *
 **/

#include <csp/cspsim/hud/HUD.h>

namespace csp {
namespace hud {

/** A general purpose HUD pitch-ladder manager.  This class updates the pitch
 *  bar positions and orientations and culls pitch bars that are outside the
 *  field of view.  The actual pitch bar symbology is defined externally.
 */
class PitchLadder: public osg::Referenced {
public:
	typedef osg::ref_ptr<display::DirectionElement> PitchBar;

	PitchLadder();

	/** Add a new pitch bar at the specified angle with respect to the horizon.
	 *  @param bar The pitch bar symbol.
	 *  @param angle The pitch angle in degrees.
	 */
	void add(PitchBar bar, float angle);

	/** Update the pitch ladder.  This method should be called once per frame.
	 *
	 *  @param center_direction_body The direction to the horizon in body coordinates.
	 *    The pitch ladder will intersect this ray.
	 *  @param right_direction_body A vector pointing to the horizon +90 degrees from
	 *    center_direction_body, in body coordinates.
	 *  @param pitch_angle Euler pitch angle (in radians).
	 */
	void update(osg::Vec3 center_direction_body, osg::Vec3 const &right_direction_body, double pitch_angle);

	/** Show or hide the pitch ladder.
	 */
	void show(bool visible);

	/** Test if the pitch ladder is hidden.
	 */
	bool isHidden() const { return m_Hidden; }

private:
	typedef std::pair<PitchBar, float> PitchInfo;

	/** Reevaluate which pitch bars are potentially within the field of view.  This
	 *  method should only be called when the pitch angle has changed significantly,
	 *  and is an optimization so that we do not have to continually update pitch
	 *  bars that are not visible.
	 */
	void updateActive(float pitch_angle);

	std::vector<PitchInfo> m_Ladder;
	std::vector<unsigned> m_Active;
	bool m_Dirty;
	bool m_Hidden;
	float m_LastPitchAngle;
};

} // end namespace hud
} // end namespace csp
