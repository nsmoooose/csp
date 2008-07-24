// Combat Simulator Project
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
 * @file Median.h
 *
 */

#ifndef __CSPLIB_NET_MEDIAN9HISTORY_H__
#define __CSPLIB_NET_MEDIAN9HISTORY_H__

namespace csp {

template<class T>
class Median9History {
	int m_count;
	int m_index;
	T m_value;
	T m_history[9];

public:
	Median9History(): m_count(0), m_index(0) { }

	inline const T & add(const T &value) {
		m_history[m_index] = value;
		if (++m_index >= 9) m_index = 0;
		if (m_count < 9) {
			++m_count;
			m_value = submedian();
		} else {
			m_value = median();
		}
		return m_value;
	}

	inline const T & value() const { return m_value; }
	inline int count() const { return m_count; }

private:
#define MEDIAN9_SORT(a, b, t) if ((a) > (b)) { t = (b); (b) = (a); (a) = t; }
	T median() {
		T m[9], tmp;
		memcpy(m, m_history, sizeof(m));
		// find the median of 9 elements
		MEDIAN9_SORT(m[1], m[2], tmp); MEDIAN9_SORT(m[4], m[5], tmp); MEDIAN9_SORT(m[7], m[8], tmp);
		MEDIAN9_SORT(m[0], m[1], tmp); MEDIAN9_SORT(m[3], m[4], tmp); MEDIAN9_SORT(m[6], m[7], tmp);
		MEDIAN9_SORT(m[1], m[2], tmp); MEDIAN9_SORT(m[4], m[5], tmp); MEDIAN9_SORT(m[7], m[8], tmp);
		MEDIAN9_SORT(m[0], m[3], tmp); MEDIAN9_SORT(m[5], m[8], tmp); MEDIAN9_SORT(m[4], m[7], tmp);
		MEDIAN9_SORT(m[3], m[6], tmp); MEDIAN9_SORT(m[1], m[4], tmp); MEDIAN9_SORT(m[2], m[5], tmp);
		MEDIAN9_SORT(m[4], m[7], tmp); MEDIAN9_SORT(m[4], m[2], tmp); MEDIAN9_SORT(m[6], m[4], tmp);
		MEDIAN9_SORT(m[4], m[2], tmp);
		return m[4];  // median
	}
#undef MEDIAN9_SORT

	T submedian() {
		T m[9];
		memcpy(m, m_history, sizeof(m));
		std::sort(m, m + m_count);
		return (m[m_count/2] + m[(m_count-1)/2]) / 2;
	}

};

} // namespace csp

#endif // __CSPLIB_NET_MEDIAN9HISTORY_H__
