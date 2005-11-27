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
 * @file GroundAvoidanceAdvisory.h
 *
 **/


#ifndef __CSPSIM_F16_GROUND_AVOIDANCE_ADVISORY_H__
#define __CSPSIM_F16_GROUND_AVOIDANCE_ADVISORY_H__

#include <System.h>
#include <csp/lib/data/Vector3.h>


/** Implements the F-16 Ground Avoidance Advisory Function (GAAF).
 *
 *  This system provides audible and visual warnings intended to reduce
 *  Controlled Flight Into Terrain (CFIT) incidents.
 *
 *  The system computes the altitude at which an immediate recovery
 *  procedure must be initiatied to avoid impact with the ground.  This
 *  implementation also provides the pull-up advisory cue for A-G modes
 *  and the Descent Warning After Takeoff (DWAT) function.
 *
 *  The advisory altitudes are computed as follows:
 *
 *    - The floor is taken as CARA ALOW above the terrain elevation as
 *      determined by the FCR or Combined Altitude Radar Altimeter (CARA).
 *    - An additional buffer is added to the floor as follows:
 *        < 325 KCAS : 150 FT + 25% of predicted altitude loss
 *        > 375 KCAS : 50 FT = 12.5% of predicted altitude loss
 *        (Linear interpolation between 325 and 375 KCAS.)
 *    - Predicted altitude loss is the sum of:
 *      - Dive recovery (assuming 4 g pullup)
 *      - Aircraft response time (rolling wings level at 120 deg/sec)
 *      - Pilot reaction time (1.0 sec, or 0.45 sec in A-G mode)
 *
 *  An advance notice/preparatory advisory is issued 2 seconds prior to
 *  reaching the advisory altitude.
 *
 *  The A-G pull-up advisory cue channel provides a [0.0, 1.0] signal indicating
 *  estimated time to the advisory altitude where 0.0 corresponds to 8 seconds
 *  and 1.0 corresponds to the advisory altitude (0 seconds).  The value is
 *  biased negative to avoid numerical precision errors, so that testing > 0
 *  is sufficient to decide if the cue should be displayed.
 *
 *  Note that the GAAF is only operational if the landing gear is up.  The
 *  advisories are suppressed if the descent rate is less than 960 ft/min.
 *
 *  The DWAT function provides an audible warning of an unintended descent
 *  immediately after takeoff.  The following conditions must be be true for
 *  the alert to sound:
 *
 *    - Less than 3 minutes since takeoff (WOW)
 *    - Landing gear handle is up.
 *    - Aircaft has climbed at least 300 ft above runway elevation (MSL).
 *    - The descent rate will cause the aircraft to reach runway elevation
 *      (msl) in less than 30 seconds.
 *    - The aircraft has not exceeded 10000 ft above runway elevation (MSL).
 *    - The message has not already occured since takeoff.
 */

class GroundAvoidanceAdvisory: public System {
public:
	SIMDATA_DECLARE_OBJECT(GroundAvoidanceAdvisory)

	GroundAvoidanceAdvisory();

	virtual void registerChannels(Bus* bus);
	virtual void importChannels(Bus* bus);
	virtual double onUpdate(double dt);

private:

	void updateAltitudeAdvisories(const double descent_velocity);
	void updateTakeoff(const double dt);
	void updateDescentWarning(const double descent_velocity);

	DataChannel<bool>::Ref b_AltitudeAdvisory;
	DataChannel<bool>::Ref b_AdvanceAltitudeAdvisory;
	DataChannel<bool>::Ref b_DescentWarningAfterTakeoff;
	DataChannel<double>::Ref b_PullupAnticipation;
	DataChannel<simdata::Vector3>::CRef b_Velocity;
	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<double>::CRef b_CAS;
	DataChannel<double>::CRef b_GroundZ;
	DataChannel<double>::CRef b_Roll;
	DataChannel<double>::CRef b_Pitch;
	DataChannel<bool>::CRef b_GearHandleUp;
	DataChannel<bool>::CRef b_WOW;
	DataChannel<double>::CRef b_CaraAlow;
	double m_MinimumDescentRate;
	double m_RollRate;
	double m_InverseG;
	double m_RunwayAltitude;
	double m_TakeoffElapsedTime;
	typedef enum { ENABLED, ARMED, DISARMED } DescentWarningState;
	DescentWarningState m_DescentWarningState;
};

#endif // __CSPSIM_F16_GROUND_AVOIDANCE_ADVISORY_H__

