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
 * @file Filters.h
 *
 * Various digital filters.
 *
 **/


#ifndef __CSPSIM_FILTERS_H__
#define __CSPSIM_FILTERS_H__

#include <csp/lib/util/Namespace.h>

CSP_NAMESPACE

// H(s) = c * (s + a) / (s + b)
/** A filter corresponding to the laplace transform H(s) = c * (s + a) / (s + b).
 *  The gain is c at high frequencies and c * a / b at low frequencies.  Note that
 *  no frequency prewarping is applied in generating the IIR digital filter.
 */
class LeadLagFilter {
public:
	LeadLagFilter(double a, double b, double c): m_A(a), m_B(b), m_C(c), m_Value(0.0), m_LastInput(0.0) { }
	void update(double input, double dt) {
		double a = m_A * dt;
		double b = m_B * dt;
		input *= m_C;
		m_Value = ((2.0 - b) * m_Value + (a + 2.0) * input + (a - 2.0) * m_LastInput) / (2.0 + b);
		m_LastInput = input;
	}
	double value() const { return m_Value; }
private:
	double m_A;
	double m_B;
	double m_C;
	double m_Value;
	double m_LastInput;
};


/** A lowpass filter corresponding to the laplace transform H(s) = a / (s + a).
 *  Note that no frequency prewarping is applied in generating the IIR digital
 *  filter.
 */
class LagFilter {
public:
	LagFilter(double a): m_A(a), m_Value(0.0), m_LastInput(0.0) { }
	void update(double input, double dt) {
		double a = m_A * dt;
		m_Value = m_Value + a / (2.0 + a) * (input + m_LastInput - 2.0 * m_Value);
		m_LastInput = input;
	}
	double value() const { return m_Value; }
private:
	double m_A;
	double m_Value;
	double m_LastInput;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_FILTERS_H__

