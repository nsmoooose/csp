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

#include <cmath>
#include <iostream>

#include "DynamicalSystem.h"

double const DynamicalSystem::PGROW  = -0.20;
double const DynamicalSystem::PSHRNK = -0.25;
double const DynamicalSystem::FCOR   = 0.06666666;         // 1.0/15.0

double const DynamicalSystem::SAFETY = 0.9;
double const DynamicalSystem::ERRCON = 6.0e-4;

unsigned int const DynamicalSystem::MAXSTP = 10000;
double const       DynamicalSystem::TINY   = std::numeric_limits<float>::epsilon();//1.0e-30;


//===========================================================================

std::vector<double> const &DynamicalSystem::rk4(std::vector<double> const &y, 
												std::vector<double> const &dydx, 
												double x, double h)
{	
	static std::vector<double> yout(m_NVariable), yt(m_NVariable), dyt(m_NVariable), 
		                       dym(m_NVariable);

	double hh = h * 0.5;
    unsigned short i = 0;

	for (; i < m_NVariable; ++i)
		yt[i] = y[i] + hh * dydx[i];
	
	double xh = x + hh;

	dyt = f(xh, yt);
	for (i = 0; i < m_NVariable; ++i) 
		yt[i] = y[i] + hh * dyt[i];
	
	dym = f(xh, yt);
	for (i = 0; i < m_NVariable; ++i) {
		yt[i] = y[i] + h * dym[i];
		dym[i] += dyt[i];
	}
	
	dyt = f(x + h, yt);
	double h6 = h / 6.0;
	for (i = 0; i < m_NVariable; ++i)
		yout[i] = y[i] + h6 * (dydx[i] + dyt[i] + 2.0 * dym[i]);

	return yout;
}

//===========================================================================

std::vector<double> const &DynamicalSystem::rkqc(std::vector<double> &y, 
												 std::vector<double> &dydx, 
												 double &x, double htry, double eps, 
												 std::vector<double> const &yscal, 
												 double &hdid, double &hnext)
{
	static std::vector<double> ytemp, ysav, dysav;

	double xsav = x;
	ysav = y;
	dysav = dydx;
	double h = htry;
	unsigned short i;
	bool loop = true;
	do {
		double hh = 0.5 * h;
		ytemp = rk4(ysav, dysav, xsav, hh);
		x = xsav + hh;
		dydx = f(x, ytemp);
		y = rk4(ytemp, dydx, x, hh);
		x = xsav + h;
		if (x == xsav) {
			std::cout << "Step size too small in routine RKQC\n" << std::endl;
			loop = false;
		}
		else {
			ytemp = rk4(ysav, dysav, xsav, h);
			double errmax = 0.0;
			for (i = 0; i < m_NVariable; ++i) {
				ytemp[i] -= y[i]; // ytemp[i] = y[i] - ytemp[i];
				double temp = fabs(ytemp[i] / yscal[i]);
				if (errmax < temp) 
					errmax = temp;
			}
			errmax /= eps;
			if (errmax <= 1.0) {
				hdid = h;
				hnext = (errmax > ERRCON ?
					SAFETY * h * exp(PGROW * log(errmax)) : 4.0 * h);
				loop = false;
			}
			else
			 h *= SAFETY * exp(PSHRNK * log(errmax));
		}
	}
	while ( loop );
	for (i = 0; i < m_NVariable; ++i) 
		y[i] += ytemp[i] * FCOR;
	return y;
}

//===========================================================================

std::vector<double> const &DynamicalSystem::odeint(std::vector<double> const & ystart,
                                                   double x1, double x2, 
												   double eps, double h1, double hmin, 
												   unsigned int &nok, unsigned int &nbad)
{
	static std::vector<double> y, dydx, yscal(m_NVariable);

	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	nok = nbad =  0;
	y = ystart;
	unsigned int nstp = 0;
	do {
		++nstp;
		dydx = f(x, y);
		for (unsigned short i = 0;i < m_NVariable; ++i)
			yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;
		
		if ((x+h-x2)*(x+h-x1) > 0.0) h = x2 - x;
		y = rkqc(y, dydx, x, h, eps, yscal, hdid, hnext);
		if ( hdid == h ) ++nok; else ++nbad;
		if ( (x-x2) * (x2-x1) >= 0.0 )
			return y;
		else {
			if (fabs(hnext) <= hmin) {
				std::cout << "Step size too small in ODEINT\n" << std::endl;
				nstp = MAXSTP;
			}
			h = hnext;
		}
	}
	while ( nstp < MAXSTP );
	std::cout << "Too many steps in routine ODEINT\n" << std::endl;
	return ystart;
}

//=================================================================

std::vector<double> const &DynamicalSystem::RungeKutta(std::vector<double> const &y0, 
													   double t1, double t2)
{
 static std::vector<double> result;
 unsigned int nok,nbad;
 result = odeint(y0, t1, t2, m_precision, m_hestimate, m_hmin, nok, nbad);
 //std::cout << "nok =" << nok << "; nbad = " << nbad << "\n";
 return result;               
}
