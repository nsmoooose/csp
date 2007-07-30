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


/**
 * @file VectorField.h
 *
 **/

#ifndef __CSPLIB_NUMERIC_VECTORFIELD_H__
#define __CSPLIB_NUMERIC_VECTORFIELD_H__

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Properties.h>
#include <csp/csplib/numeric/Vector.h>

CSP_NAMESPACE

namespace numeric {

class NumericalMethod;

/// An interface to define ordinary differential equations that can be solved
/// by NumericalMethod.  Subclasses must specify the dimension of the vector
/// field and implement method f to compute dy/dx as a function of (x, y).
class CSPLIB_EXPORT VectorField: NonCopyable {
public:
	virtual ~VectorField() {}

	/// Evaluate dy/dx at (x, y).
	virtual void f(double x, Vectord const &y, Vectord &dydx) = 0;

	/// Return the dimension of the vector field.
	unsigned dimension() const { return m_dimension; }

protected:
	VectorField(unsigned dimension): m_dimension(dimension) {}
	unsigned m_dimension;
};

} // namespace numeric

CSP_NAMESPACE_END

#endif // __CSPLIB_NUMERIC_VECTORFIELD_H__

