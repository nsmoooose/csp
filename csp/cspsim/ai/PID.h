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


#ifndef __CSPSIM_AI_PID_H__
#define __CSPSIM_AI_PID_H__

#include <csp/csplib/util/Math.h>


namespace csp {
namespace ai {

/** A simple proportional-integral-derivative (PID) controller
 * implementation.  See http://en.wikipedia.org/wiki/PID_controller for general background 
 * on PID controllers.
 *
 * From wikipedia:
 * A proportional–integral–derivative controller (PID controller) is a generic control
 * loop feedback mechanism widely used in industrial control systems. A PID controller
 * attempts to correct the error between a measured process variable and a desired setpoint
 * by calculating and then outputting a corrective action that can adjust the process accordingly.
 *
 * A familiar example of a control loop is the action taken to keep one's shower water at the
 * ideal temperature, which typically involves the mixing of two process streams, cold and hot 
 * water. The person feels the water to estimate its temperature. Based on this measurement they
 * perform a control action: use the cold water tap to adjust the process. The person would repeat
 * this input-output control loop, adjusting the hot water flow until the process temperature
 * stabilized at the desired value.
 */
class PID {
public:
	/** Initialize the controller with all gains set to zero.  See
	 *  setGains().  By default the output is limited to +/- 1e+10.
	 */
	PID(): m_GainP(0.0), m_GainI(0.0), m_GainD(0.0) { init(); }

	/** Initialize the controller with the specified gains.
	 *  By default the output is limited to +/- 1e+10.
	 */
	PID(double p, double i, double d=0.0): m_GainP(p), m_GainI(i), m_GainD(d) { init(); }

	/** Set the controller gains.
	 */
	void setGains(double p, double i, double d) {
		m_GainP = p; m_GainI = i; m_GainD = d;
	}

	/** Set the low and high limits of the controller output.
	 */
	void clamp(double lo, double hi) {
		m_Lo = lo;
		m_Hi = hi;
	}

	/** Update the controller, given the error signal over the last timestep.
	 *  Returns the new output value of the controller.
	 */
	double update(double error, double dt) {
		double d = (dt > 0.0) ? (error - m_LastError) / dt : 0.0;
		m_LastError = error;
		m_I = clampTo(m_I + error * dt * m_GainI, m_Lo, m_Hi);
		m_LastValue = clampTo(m_GainP * error + m_I + d * m_GainD, m_Lo, m_Hi);
		return m_LastValue;
	}

	/** Return the output value of the last update.
	 */
	double get() const { return m_LastValue; }

	/** Set the integral value.
	 */
	void set(double x) { m_I = x; }

	/** Update the integral value from another PID controller, and
	 *  set the last error value to zero.
	 */
	void copy(PID const &p) {
		m_I = p.m_I;
		m_LastError = 0.0;  // is this needed? (what is the use case?)
	}

	/** Test if the controller is saturated at either the high limit.
	 */
	bool isClampedHi() const { return m_LastValue == m_Hi; }

	/** Test if the controller is saturated at either the low limit.
	 */
	bool isClampedLo() const { return m_LastValue == m_Lo; }

private:
	void init() {
		m_I = 0.0;
		m_Lo = -1e+10;
		m_Hi = 1e+10;
		m_LastError = 0.0;
	}

	double m_GainP;
	double m_GainI;
	double m_GainD;
	double m_I;  // integrated error
	double m_Lo;
	double m_Hi;
	double m_LastError;
	double m_LastValue;
};

} // end namespace ai
} // end namespace csp

#endif // __CSPSIM_AI_PID_H__

