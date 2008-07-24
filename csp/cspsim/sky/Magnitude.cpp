// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

#include <csp/cspsim/sky/Magnitude.h>

namespace csp {

MagnitudeFunction::~MagnitudeFunction() {
}

double PolynomialReflector::compute(double sun_body_distance, double body_observer_distance, double phase_angle) const {
	double sum = 0.0;
	sum = (sum + m_Phase4) * phase_angle;
	sum = (sum + m_Phase3) * phase_angle;
	sum = (sum + m_Phase2) * phase_angle;
	sum = (sum + m_Phase1) * phase_angle;
	return m_AbsoluteMagnitude + 5.0 * log10(sun_body_distance * body_observer_distance * 4.4682e-23) + sum;
}

double DiffuseReflector::compute(double sun_body_distance, double body_observer_distance, double phase_angle) const {
	const double inv_d0_4 = 1.9965e-45;  // 1.0 / (1au in meters) ^ 4
	double dbs2 = sun_body_distance * sun_body_distance;
	double dbo2 = body_observer_distance * body_observer_distance;
	const double phase_integral = std::max(1e-10, 0.667 * ((1.0 - phase_angle / PI) * cos(phase_angle) + (1.0 / PI) * sin(phase_angle)));
	return m_AbsoluteMagnitude + 2.5 * log10(dbs2 * dbo2 * inv_d0_4 / phase_integral);
}

} // namespace csp

