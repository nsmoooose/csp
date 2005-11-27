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
 * @file DynamicalSystem.cpp
 *
 **/

#include <csp/cspsim/DynamicalSystem.h>
#include <csp/csplib/numeric/NumericalMethod.h>

CSP_NAMESPACE

DynamicalSystem::DynamicalSystem(size_type dimension):
	VectorField(dimension),
	m_NumericalMethod(0) {
}

void DynamicalSystem::setNumericalMethod(NumericalMethod *pnumericalMethod) {
	if (m_NumericalMethod && m_NumericalMethod != pnumericalMethod) delete m_NumericalMethod;
	m_NumericalMethod = pnumericalMethod;
	m_NumericalMethod->setVectorField(this);
}

DynamicalSystem::~DynamicalSystem() {
}

NumericalMethod *const DynamicalSystem::getNumericalMethod() const {
	return m_NumericalMethod;
}

Vector::Vectord const &DynamicalSystem::flow(Vector::Vectord &y0, double t0, double dt){
	//PROF0(FLOW);
	return m_NumericalMethod->enhancedSolve(y0, t0, dt);
	//PROF1(FLOW, 200);
	if (neededQuickSolve()) {
		return m_NumericalMethod->quickSolve(y0, t0, dt);
	}
}

bool DynamicalSystem::neededQuickSolve() const {
	return m_NumericalMethod->hasFailed();
}

CSP_NAMESPACE_END

