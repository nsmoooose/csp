#pragma once
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

#include <csp/cspsim/ai/AircraftTask.h>
#include <csp/csplib/data/Vector3.h>

namespace csp {
namespace ai {

class LandingTask: public AircraftTask {
public:
	LandingTask();

private:
	enum {
		LINEUP, APPROACH, FINAL_APPROACH, AEROBRAKE, BRAKE, ABORT
	};

	virtual void init();
	virtual void preupdate();

	void onLineup();
	void checkCaptureAngle();
	void onApproach();
	void onFinalApproach();
	void onAeroBrake();
	void onBrake();
	void onAbort();

	double m_Range;
	double m_SlantRange;
	double m_Offset;
	double m_AngleOff;
	double m_TargetSlope;
	double m_GlideSlope;
	double m_AimBearing;
	Vector3 m_AimPoint;
	Vector3 m_AbortPoint;
	bool m_AbortSafe;
};

} // end namespace ai
} // end namespace csp
