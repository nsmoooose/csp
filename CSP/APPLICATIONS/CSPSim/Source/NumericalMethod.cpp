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
 * @file NumericalMethod.cpp
 *
 **/

# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#define NOMINMAX
#endif

#include "NumericalMethod.h"

#include <cmath>

bool NumericalMethod::failed() const {
		return m_failed;
}

double const RungeKutta2::PGROW  = -0.20;
double const RungeKutta2::PSHRNK = -0.25;
double const RungeKutta2::FCOR   = 0.06666666;         // 1.0/15.0

double const RungeKutta2::SAFETY = 0.9;
double const RungeKutta2::ERRCON = 6.0e-4;

unsigned int const RungeKutta2::MAXSTP = 8;
double const       RungeKutta2::TINY   = std::numeric_limits<float>::epsilon();//1.0e-30;


//===========================================================================

std::vector<double> const &RungeKutta2::rk4(std::vector<double> const &y, 
												std::vector<double> const &dydx, 
												double x, double h) const
{	
	static std::vector<double> yout(m_dimension), yt(m_dimension);

	double hh = h * 0.5;
    unsigned short i = 0;

	for (; i < m_dimension; ++i)
		yt[i] = y[i] + hh * dydx[i];
	
	double xh = x + hh;

	std::vector<double> dyt = vectorField->_f(xh, yt);
	for (i = 0; i < m_dimension; ++i) 
		yt[i] = y[i] + hh * dyt[i];
	
	std::vector<double> dym = vectorField->_f(xh, yt);
	for (i = 0; i < m_dimension; ++i) {
		yt[i] = y[i] + h * dym[i];
		dym[i] += dyt[i];
	}
	
	dyt = vectorField->_f(x + h, yt);
	double h6 = h / 6.0;
	for (i = 0; i < m_dimension; ++i)
		yout[i] = y[i] + h6 * (dydx[i] + dyt[i] + 2.0 * dym[i]);

	return yout;
}

//=========================================================================== 

std::vector<double> const &RungeKutta2::rkqc(std::vector<double> &y, 
												 std::vector<double> &dydx, 
												 double &x, double htry, double eps, 
												 std::vector<double> const &yscal, 
												 double &hdid, double &hnext) {
	static std::vector<double> y1, ysav;

	double xsav = x;
	ysav = y;
	std::vector<double> dysav = dydx;
	double h = htry;
	unsigned short i;
	bool loop = true;
	std::vector<double> ytemp;
	
	do {
		double hh = 0.5 * h;
		ytemp = rk4(ysav, dysav, xsav, hh);
		x = xsav + hh;
		dydx = vectorField->_f(x, ytemp);
		y1 = rk4(ytemp, dydx, x, hh);
		x = xsav + h;
		if (x == xsav) {
			std::cout << "Step size too small in RungeKutta2::rkqc\n";
			m_failed = true;
			loop = false;
		}
		else {
			ytemp = rk4(ysav, dysav, xsav, h);
			double errmax = 0.0;
			for (i = 0; i < m_dimension; ++i) {
				ytemp[i] = y1[i] - ytemp[i];
				errmax = std::max(errmax,fabs(ytemp[i] / yscal[i]));
			}
			errmax /= eps;
			if (errmax <= 1.0) {
				hdid = h;
				hnext = (errmax > ERRCON ?
					SAFETY * h * pow(errmax,PGROW) : 4.0 * h);
				loop = false;
			}
			else
			 h *= SAFETY * pow(errmax,PSHRNK);
		}
	}
	while ( loop );
	if (m_failed) 
		return ysav;
	else {
		for (i = 0; i < m_dimension; ++i) 
			y1[i] += ytemp[i] * FCOR;
		return y1;
	}
}

//===========================================================================

std::vector<double> const &RungeKutta2::odeint(
												   std::vector<double> const & ystart,
                                                   double x1, double x2, 
												   double eps, double h1, double hmin, 
												   unsigned int &nok, unsigned int &nbad)
{
	static std::vector<double> yscal(m_dimension), y;

	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	nok = nbad =  0;
	y = ystart;
	unsigned int nstp = 0;
	do {
		++nstp;
		std::vector<double> dydx = vectorField->_f(x, y);
		for (unsigned short i = 0;i < m_dimension; ++i)
			yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;
		
		if ((x+h-x2)*(x+h-x1) > 0.0) h = x2 - x;
		y = rkqc(y, dydx, x, h, eps, yscal, hdid, hnext);
		if ( hdid == h ) ++nok; else ++nbad;
		if ( (x-x2) * (x2-x1) >= 0.0 )
			return y;
		else {
			if (fabs(hnext) <= hmin) {
				m_failed = true;
				std::cout << "Step size too small in RungeKutta2::odeint\n";
			}
			else
				h = hnext;
		}
	}
	while ( nstp < MAXSTP && !m_failed);
	if (nstp == MAXSTP) {
		std::cout << "Too many steps in RungeKutta2::odeint\n";
		m_failed = true;
	}

	return ystart;
}

//=================================================================

std::vector<double> const& RungeKutta2::quickSolve(std::vector<double>& y0, double t0, double dt) const {
	static std::vector<double> y;
    y = rk4(y0, vectorField->_f(t0,y0), t0, dt);
	return y;
}

//=================================================================

std::vector<double> const& RungeKutta2::enhancedSolve(std::vector<double>& y0, double t0, double dt) {
	static std::vector<double> result;
 unsigned int nok,nbad;
 m_failed = false;
 result = odeint(y0, t0, t0+dt, m_precision, m_hestimate, m_hmin, nok, nbad);
 std::cout << "RungeKutta2::enhancedSolve nok =" << nok << "; nbad = " << nbad << "\n" << std::endl;
 return result;               
}




double const RungeKuttaCK::PGROW  = -0.20;
double const RungeKuttaCK::PSHRNK = -0.25;

double const RungeKuttaCK::SAFETY = 0.9;
double const RungeKuttaCK::ERRCON = 1.89e-4;

unsigned int const RungeKuttaCK::MAXSTP = 8;
double const       RungeKuttaCK::TINY   = std::numeric_limits<float>::epsilon();


std::vector<double> const & RungeKuttaCK::rkck(std::vector<double> const & y, 
		                            std::vector<double> const & dydx, 
									double x, double h, std::vector<double>& yerr) const {
	static const double a2 = 0.2, a3 = 0.3, a4 = 0.6, a5 = 1.0, a6 = 0.875, 
						b21 = 0.2, 
						b31 = 3.0/40.0, b32 = 9.0/40.0, 
						b41 = 0.3, b42 = -0.9, b43 =1.2,
						b51 = -11.0/54.0, b52 = 2.5, b53 = -70.0/27.0, b54 = 35.0/27.0,
						b61=1631.0/55296.0, b62=175.0/512.0, b63=575.0/13824.0, b64=44275.0/110592.0, b65=253.0/4096.0,
						c1 = 37.0/378.0, c3 = 250.0/621.0, c4 = 125.0/594.0, c6 = 512.0/1771.0,
						dc5 = -277.0/14336.0,
						dc1 = c1 - 2825.0/27648.0, dc3 = c3 - 18575.0/48384.0, dc4 = c4 - 13525.0/55296.0, dc6 = c6 - 0.25;
 
	static std::vector<double> ytemp(m_dimension);

	unsigned short i = 0;
	for (; i < m_dimension; ++i)
		ytemp[i] = y[i] + b21 * h * dydx[i];
	std::vector<double> ak2 = vectorField->_f(x + a2*h, ytemp);
	for (i = 0; i < m_dimension; ++i)
		ytemp[i] = y[i] + h * (b31*dydx[i] + b32*ak2[i]);
    std::vector<double> ak3 = vectorField->_f(x + a3*h, ytemp);
	for (i = 0; i < m_dimension; ++i)
		ytemp[i] = y[i] + h * (b41*dydx[i] + b42*ak2[i] + b43*ak3[i]);
	std::vector<double> ak4 = vectorField->_f(x + a4*h, ytemp); 
    for (i = 0; i < m_dimension; ++i)
		ytemp[i] = y[i] + h * (b51*dydx[i] + b52*ak2[i] + b53*ak3[i] + b54*ak4[i]);
	std::vector<double> ak5 = vectorField->_f(x + a5*h, ytemp); 
    for (i = 0; i < m_dimension; ++i)
		ytemp[i] = y[i] + h * (b61*dydx[i] + b62*ak2[i] + b63*ak3[i] + b64*ak4[i] + b65*ak5[i]);
	std::vector<double> ak6 = vectorField->_f(x + a6*h, ytemp); 
    for (i = 0; i < m_dimension; ++i)
		ytemp[i] = y[i] + h * (c1*dydx[i] + c3*ak3[i] + c4*ak4[i] + c6*ak6[i]);
	for (i = 0; i < m_dimension; ++i)
		yerr[i] = h * (dc1*dydx[i] + dc3*ak3[i] + dc4*ak4[i] + dc5*ak5[i] + dc6*ak6[i]);
	return ytemp;
}

std::vector<double>	const &RungeKuttaCK::rkqs(std::vector<double> &y, 
												 std::vector<double> &dydx,	
												 double	&x,	double htry, double	eps, 
												 std::vector<double> const &yscal, 
												 double	&hdid, double &hnext) {	
	static std::vector<double> yerr(m_dimension);
	double h = htry, xnew, errmax;
	unsigned short i;
	bool loop =	true;
	std::vector<double>	ytemp;
	do {
		ytemp =	rkck(y,dydx,x,h,yerr);
		errmax = 0.0;
		for	(i = 0;	i <	m_dimension; ++i)
			errmax = std::max(errmax,fabs(yerr[i]/yscal[i]));
		errmax /= eps;
		if (errmax <= 1.0)
			loop = false;
		else {
			 double	htemp =	SAFETY * h * pow(errmax,PSHRNK);
			 h = (h>=0.0 ? std::max(htemp,0.1 *	h) : std::min(htemp,0.1	*h));
			 xnew =	x +	h;
			 if	(x == xnew)	{
				 std::cout << "Stepsize	underflow in RungeKuttaCK::rkqs\n";
				 m_failed = true;
			 }
		}
	}
	while (	loop );
	if (!m_failed) {
	if (errmax > ERRCON)
		hnext =	SAFETY * h * pow(errmax,PGROW);
	else
		hnext =	5.0	* h;
	x += (hdid = h); 
	for	(i = 0;	i <	m_dimension; ++i) 
		y[i] = ytemp[i];
	}
	return y;
}

std::vector<double> const &RungeKuttaCK::odeint(std::vector<double> const & ystart,
                                                   double x1, double x2, 
												   double eps, double h1, double hmin, 
												   unsigned int &nok, unsigned int &nbad)
{
	static std::vector<double> yscal(m_dimension), y;

	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	nok = nbad =  0;
	y = ystart;
	unsigned int nstp = 0;
	double const delta21 = x2 - x1;
	do {
		++nstp;
		std::vector<double> dydx = vectorField->_f(x, y);
		for (unsigned short i = 0;i < m_dimension; ++i)
			yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;
		
		if ((x+h-x2)*(x+h-x1) > 0.0) h = x2 - x;
		y = rkqs(y, dydx, x, h, eps, yscal, hdid, hnext);
		if ( hdid == h ) ++nok; else ++nbad;
		if ( (x-x2) * delta21 >= 0.0 )
			return y;
		else {
			if (fabs(hnext) <= hmin) {
				m_failed = true;
				std::cout << "Step size too small in RungeKuttaCK::odeint\n";
			}
			else
				h = hnext;
		}
	}
	while ( nstp < MAXSTP && !m_failed);
	if (nstp == MAXSTP) {
		//std::cout << "Too many steps in RungeKuttaCK::odeint\n";
		m_failed = true;
	}

	return ystart;
}

std::vector<double> const& RungeKuttaCK::quickSolve(std::vector<double>& y0, double t0, double dt) const {
	static std::vector<double> y, yerr(m_dimension);

    y = rkck(y0, vectorField->_f(t0,y0), t0, dt, yerr);
	return y;
}

//=================================================================

std::vector<double> const& RungeKuttaCK::enhancedSolve(std::vector<double>& y0, double t0, double dt) {
	static std::vector<double> result;
 unsigned int nok,nbad;
 m_failed = false;
 result = odeint(y0, t0, t0+dt, m_precision, m_hestimate, m_hmin, nok, nbad);
 //std::cout << "RungeKuttaCK::enhancedSolve nok = " << nok << "; nbad = " << nbad << "\n" << std::endl;
 return result;               
}

