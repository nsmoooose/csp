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
 * @file NumericalMethod.h
 *
 **/

#ifndef __NUMERICALMETHOD_H__
#define __NUMERICALMETHOD_H__


#include <limits>
#include <vector>


class VectorField
{
	unsigned short const m_dimension;
public:
	VectorField(unsigned short dimension):
	  m_dimension(dimension) {
	  }
	  unsigned short getDimension() const {
		  return m_dimension;
	  }
	virtual std::vector<double> const& _f(double t, std::vector<double>& y) = 0;
};


class NumericalMethod {
	bool m_bdeleteVF;
protected:
	unsigned short m_dimension;
	VectorField* vectorField;
	bool m_bfailed;
public:
	NumericalMethod(bool deleteVF = true):
		vectorField(0),
		m_bfailed(false),
		m_bdeleteVF(deleteVF) {
	}
	NumericalMethod(VectorField* pf, bool deleteVF = true):
		vectorField(pf), 
		m_bfailed(false),
		m_bdeleteVF(deleteVF) {
			if (vectorField)
				m_dimension = vectorField->getDimension();
			else
				std::cout << "ODE dimension undefined!\n";
	}
	void setVectorField(VectorField* pf) {
		vectorField = pf;
		m_dimension = vectorField->getDimension();
	}
	virtual std::vector<double> const& quickSolve(std::vector<double>& y0, double t0, double dt)const = 0; 
	virtual std::vector<double> const& enhancedSolve(std::vector<double>& y0, double t0, double dt) = 0;
	virtual ~NumericalMethod(){
		if (m_bdeleteVF && vectorField) {
		delete vectorField;
		vectorField = 0;
		}
	}
	virtual bool failed() const;
};

class RungeKutta: public NumericalMethod
{
	std::vector<double> const & rk4(std::vector<double> const & y, 
		                            std::vector<double> const & dyx, 
		                            double x, double h) const;
	std::vector<double> const & rkqc(std::vector<double> &y, 
		std::vector<double> &dyx, 
		double &x, double htry, double eps, 
		std::vector<double> const &yscal, 
		double &hdid, double &hnext) const;
	std::vector<double> const &odeint(std::vector<double> const & ystart,
		double x1, double x2, 
		double eps, double h1, double hmin, 
		unsigned int &nok, unsigned int &nbad);
	
	static double const PGROW ;
	static double const PSHRNK;
	static double const FCOR;
	
	static double const SAFETY;
	static double const ERRCON;
	
	static unsigned int const MAXSTP;
    static double const TINY;
	double m_precision, m_hmin, m_hestimate;
public:
	std::vector<double> const& quickSolve(std::vector<double>& y0, double t0, double dt) const; 
	std::vector<double> const& enhancedSolve(std::vector<double>& y0, double t0, double dt);
	RungeKutta( 
		VectorField* vectorField = 0,
		bool deleteVF = true,
		double Precision = 1.e-3, //1.e-4
		double Hmin = 0.0, 
		double Hestimate = 1.e-2):
	    NumericalMethod(vectorField, deleteVF),
		m_precision(Precision),
		m_hmin(Hmin),
		m_hestimate(Hestimate)
	{}
};

#endif // __NUMERICALMETHOD_H__

