// Combat Simulator Project - CSPSim
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

#ifndef __VECTORFIELD_H__
#define __VECTORFIELD_H__

#include "Vector.h"

class NumericalMethod;

/**
 * A simple class to represent a multidimensional vector
 * field describing the kinetic parameters of a dynamical
 * system.
 */
class VectorField {
protected:
	typedef size_t size_type;
	size_type const m_Dimension;
	Vector::Vectord m_dy; ///< contains the current value of the vector field at (t,y_1,...,y_d)
public:
	/**
	 * Construct a new vector field of the specified dimension.
	 */
	VectorField(size_type dimension):
		m_Dimension(dimension),
		m_dy(m_Dimension) {
	}

		virtual ~VectorField(){}

	/**
	 * @return the dimension of the vector field.
	 */
	size_type getDimension() const {
		return m_Dimension;
	}

	/**
	 * @return the vector field at the specified point.
	 */
	virtual Vector::Vectord const &f(double t, Vector::Vectord &y) = 0;

	/**
	 * @return the numerical method, if any, used to solve
	 * y' = f(t,y)
	 */
	virtual NumericalMethod *const getNumericalMethod() const {
		return 0;
	}
};

#endif // __VECTORFIELD_H__

