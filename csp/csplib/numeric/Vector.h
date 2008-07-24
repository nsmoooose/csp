// Combat Simulator Project
// Copyright (C) 2003, 2004 The Combat Simulator Project
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

#ifndef __CSPLIB_NUMERIC_VECTOR_H__
#define __CSPLIB_NUMERIC_VECTOR_H__

#include <csp/csplib/util/Namespace.h>

#include <cassert>
#include <cmath>
#include <ostream>
#include <valarray>

namespace csp {

/** Numerical integration and related utilities */
namespace numeric {

typedef std::valarray<double> Vectord;

inline double norm_2(Vectord const& v) {
	const unsigned dim = v.size();
	double s2 = 0.0;
	for (unsigned i = 0; i < dim; ++i) s2 += v[i] * v[i];
	return sqrt(s2);
}

inline double norm_1(Vectord const& v) {
	const unsigned dim = v.size();
	double s = 0.0;
	for (unsigned i = 0; i < dim; ++i) s += fabs(v[i]);
	return s;
}

} // namespace numeric

// declaring operator<<() inside the csp namespace allows it to be called
// from anywhere in that namespace.  otherwise it would not generally be
// found by koenig lookup, since both arguments are fundamentally in the
// std namespace.  (Even though Vectord looks like it is in csp::numeric,
// a typedef is not sufficient for koenig lookup.)  one more argument for
// replacing valarray with a custom class!
inline std::ostream &operator<<(std::ostream& os, const numeric::Vectord& v) {
	const size_t dim = v.size();
	os << '(';
	if (dim > 0) os << v[0];
	for (size_t i = 1; i < dim; ++i) os << ',' << v[i];
	return os << ')';
}

} // namespace csp

#endif // __CSPLIB_NUMERIC_VECTOR_H__

