// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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


#include "DynamicalSystem.h"
#include "Profile.h"

#include <cmath>
#include <iostream>
#include <limits>


DynamicalSystem::DynamicalSystem(unsigned short dimension):
	VectorField(dimension),
	_numericalMethod(0) 
{
}

DynamicalSystem::~DynamicalSystem() 
{ 
	if (_numericalMethod) {
		delete _numericalMethod;
		_numericalMethod = 0;
	} 
}

void DynamicalSystem::setNumericalMethod(NumericalMethod* pnumericalMethod)
{
	_numericalMethod = pnumericalMethod;
}

std::vector<double> const& DynamicalSystem::flow(std::vector<double>& y0, double t0, double dt) const 
{
	static std::vector<double> y;
	//PROF0(FLOW);
	y = _numericalMethod->enhancedSolve(y0, t0, dt);
	//PROF1(FLOW, 200);
	if (_numericalMethod->failed()) {
		//std::cout << "quick solve is required\n";
		y = _numericalMethod->quickSolve(y0, t0, dt);
	}
	return y;
}

