// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file StopWatch.h
 *
 */

#ifndef __SIMNET_STOPWATCH_H__
#define __SIMNET_STOPWATCH_H__

#include <SimNet/NetLog.h>
#include <SimData/Timing.h>

namespace simnet {

class StopWatch {
public:
	struct Data {
		double scale;
		// default to 100 us per iteration inital guess, adjust as needed
		Data(float itertime=0.0001): scale(1.0/itertime) {}
	};
	StopWatch(double timeout, Data &data): m_data(data), m_tally(0), m_count(0), m_counts(0), m_timeout(timeout) {
		resetCount(timeout);
		SIMNET_LOG(TIMING, DEBUG, "STOPWATCH: " << m_counts << " iterations ~ " << (timeout * 1000.0) << " ms, scale=" << m_data.scale);
		m_DEBUG_extra = 0;
	}
	inline void start() { m_start_time = simdata::get_realtime(); }
	void forceCheck() {
		resetCount(m_timeout - elapsed());
	}
	inline bool checkExpired() {
		if (--m_count > 0) return false;
		double dt = elapsed();
		if (dt > m_timeout) {
			SIMNET_LOG(TIMING, DEBUG, "STOPWATCH: TOO LONG (" << dt << " > " << m_timeout << "; " << m_DEBUG_extra << " extra)");
			if (m_counts > 1) {
				m_data.scale *= 0.99;
			}
			return true;
		}
		m_data.scale *= 1.01;
		resetCount(dt);
		if (m_count < 1) {
			SIMNET_LOG(TIMING, DEBUG, "STOPWATCH: TOO SHORT (" << (dt * 1000.0) << " <= " << (m_timeout*1000.0) << "ms; " << m_DEBUG_extra << " extra)");
		}
		m_DEBUG_extra += m_count;
		return (m_count < 1);
	}
	inline void calibrate() {
		resetCount(0.0);
		if (m_tally >= 10) {
			double avg_freq = double(m_tally) / elapsed();
			if (avg_freq > m_data.scale) {
				m_data.scale *= 1.01;
			} else {
				m_data.scale *= 0.99;
			}
		}
	}
	inline double elapsed() {
		return simdata::get_realtime() - m_start_time;
	}
private:
	Data &m_data;
	int m_tally;
	int m_count;
	int m_counts;
	bool m_DEBUG_extra;
	double m_timeout;
	double m_start_time;
	inline void resetCount(double timeout) {
		m_tally += m_counts - m_count;
		m_counts = int(timeout * m_data.scale);
		m_count = m_counts;
	}
};

} // namespace simnet

#endif // __SIMNET_STOPWATCH_H__

