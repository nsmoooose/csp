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
 * @file Steerpoint.h
 *
 **/

#ifndef __CSPSIM_STEERPOINT_H__
#define __CSPSIM_STEERPOINT_H__

#include <csp/cspsim/Projection.h>

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Date.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/GeoPos.h>

namespace csp {

/** Base class for representing steerpoints / waypoints.  It is intended to contain both
 *  standard steerpoint data (e.g. position) and AI cues (e.g. formation, action).  Very
 *  specialized behavior should go in subclasses.
 */
class Steerpoint: public Referenced {
public:
	struct OffsetPoint {
		OffsetPoint(): enabled(false), range(0.0), bearing(0.0), elevation(0.0) { }
		OffsetPoint(float range_, float bearing_, float elevation_): enabled(true), range(range_), bearing(bearing_), elevation(elevation_) { }
		bool enabled;
		float range;
		float bearing;
		float elevation;
	};

	typedef Ref<Steerpoint> RefT;

	Steerpoint(Projection::CRefT projection, int index, std::string const &label, Vector3 const &position):
		m_Projection(projection), m_Index(index), m_Label(label), m_Position(position) { assert(projection.valid()); updateLLA(); }

	Steerpoint(Projection::CRefT projection, int index, std::string const &label, LLA const &lla):
		m_Projection(projection), m_Index(index), m_Label(label), m_LLA(lla) { assert(projection.valid()); updatePosition(); }

	virtual ~Steerpoint() { }

	virtual int index() const { return m_Index; }
	virtual DateZulu const &time() const { return m_Time; }
	virtual Vector3 const &position() const { return m_Position; }
	virtual LLA const &lla() const { return m_LLA; }
	virtual float altitude() const { return static_cast<float>(m_LLA.altitude()); }

	virtual void setAltitude(float altitude) { m_LLA.set(m_LLA.latitude(), m_LLA.longitude(), altitude); updatePosition(); }
	virtual void setPosition(Vector3 const &position) { m_Position = position; updateLLA(); }
	virtual void setPosition(LLA const &position) { m_LLA = position; updatePosition(); }
	virtual void setTime(DateZulu const &time) { m_Time = time; }

	virtual void setOffsetAimpoint0(OffsetPoint const &point) { m_OffsetAimpoint0 = point; }
	virtual void setOffsetAimpoint1(OffsetPoint const &point) { m_OffsetAimpoint1 = point; }
	virtual OffsetPoint const &OffsetAimpoint0() const { return m_OffsetAimpoint0; }
	virtual OffsetPoint const &OffsetAimpoint1() const { return m_OffsetAimpoint1; }

private:
	void updatePosition() { m_Position = m_Projection->convert(m_LLA); }
	void updateLLA() { m_LLA = m_Projection->convert(m_Position); }

	/** @TODO AI/mission planning cues: formation, action */

	Ref<const Projection> m_Projection;
	int m_Index;
	std::string m_Label;
	DateZulu m_Time;
	Vector3 m_Position;
	LLA m_LLA;

	OffsetPoint m_OffsetAimpoint0;
	OffsetPoint m_OffsetAimpoint1;
	OffsetPoint m_VisualInitialPoint;
	OffsetPoint m_VisualReferencePoint;
};

} // namespace csp

#endif // __CSPSIM_STEERPOINT_H__
