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
 * @file DynamicalSystem.h
 *
 **/


#ifndef __CSPSIM_DYNAMICALSYSTEM_H__
#define __CSPSIM_DYNAMICALSYSTEM_H__

#include <csp/lib/util/Namespace.h>
#include <csp/cspsim/VectorField.h>

#include <vector>

CSP_NAMESPACE

class NumericalMethod;


/**
 * DynamicalSystem is a base class for physics models that can
 * be solved by numerical integration.  The dynamical variables
 * are stored in a vector field, and are integrated by an 
 * associated numerical method (e.g. Runge-Kutta).  The f()
 * function of VectorField provides the driving term, which
 * must be implemented in derived classes.  Implementations of 
 * f() typically involve evaluation of one or more BaseDynamics
 * instances to determine the total force and moment acting on
 * the body at each instant.
 */
class DynamicalSystem: public VectorField {
	NumericalMethod *m_NumericalMethod;
public:
	DynamicalSystem(size_type dimension = 0);
	virtual ~DynamicalSystem();
	/**
	 * Set the numerical integration method used to integrate
	 * the equations of motion.
	 */
	void setNumericalMethod(NumericalMethod *pnumericalMethod);

	virtual NumericalMethod *const getNumericalMethod() const;

	/**
	 * Integrate the model over a specified time interval (dt) given
	 * a set of initial conditions (y0, t0). 
	 * @return the value of the ODE flow at (t0,y0,t0+dt).
	 */
	Vector::Vectord const &flow(Vector::Vectord &y0, double t0, double dt);

	/**
	* @return true if the enhanced solver has failed and a quick solver has been called
	*/
	bool neededQuickSolve() const;
};

CSP_NAMESPACE_END

#endif //__DYNAMICALSYSTEM_H__

