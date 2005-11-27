// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file FlightModel.cpp
 *
 **/

/*
   ==================================================================================

   aircraft axes
    - x right wing
    - y forward
    - z up

   positive control inputs
    - right stick
    - aft stick
    - right rudder

   control surface deflections (radians)
    - aileron: positive is left roll
    - elevator: positive is nose down
    - rudder: positive is left yaw
    - leading edge flap: positive is down
    - trailing edge flap: positive is down

   airflow:
    - positive alpha is airflow impacting the bottom of the aircraft
    - positive beta is airflow impacting the right side of the aircraft (left yaw)

   ==================================================================================
*/


#include "FlightModel.h"
#include <csp/lib/data/ObjectInterface.h>


SIMDATA_XML_BEGIN(FlightModel)
	SIMDATA_DEF("wing_span", m_WingSpan, true)
	SIMDATA_DEF("wing_chord", m_WingChord, true)
	SIMDATA_DEF("wing_area", m_WingArea, true)
	SIMDATA_DEF("ground_effect_offset", m_GroundEffectOffset, false)
SIMDATA_XML_END


FlightModel::FlightModel():
	m_WingSpan(1.0),
	m_WingChord(1.0),
	m_WingArea(1.0),
	m_GroundEffectOffset(0),
	m_HalfWingArea(0),
	m_AspectRatio(0),
	m_Alpha(0),
	m_AlphaDot(0),
	m_Beta(0),
	m_qBarS(0),
	m_Inv2V(0),
	m_GE(0),
	m_AirSpeed(0),
	m_Mach(0),
	m_Aileron(0),
	m_Elevator(0),
	m_Rudder(0),
	m_Airbrake(0),
	m_LeadingEdgeFlap(0),
	m_TrailingEdgeFlap(0) {
}

FlightModel::~FlightModel() {
}

void FlightModel::postCreate() {
	Object::postCreate();
	m_AspectRatio = m_WingSpan * m_WingSpan / m_WingArea;
	m_HalfWingArea = 0.5 * m_WingArea;
}

double FlightModel::groundEffect(double height) {
	if (height < m_WingSpan) {
		double x = std::max(0.0, (height + m_GroundEffectOffset) / m_WingSpan) + 0.1;
		return 0.9835 + 0.020 / (x * x);
	}
	return 1.0;
}

