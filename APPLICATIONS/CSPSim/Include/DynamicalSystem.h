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
 * @file DynamicalSystem.h
 *
 **/


#ifndef __DYNAMICALSYSTEM_H__
#define __DYNAMICALSYSTEM_H__

#include <vector>

#include "NumericalMethod.h"


class DynamicalSystem: public VectorField {
	NumericalMethod* _numericalMethod;
public:
	DynamicalSystem(unsigned short dimension);
	DynamicalSystem(VectorField* pf);
	virtual ~DynamicalSystem();
	//void setVectorField(VectorField* pf);
	void setNumericalMethod(NumericalMethod* pnumericalMethod);
	std::vector<double> const& flow(std::vector<double>& y0, double t0, double dt) const;
};

#endif //__DYNAMICALSYSTEM_H__

