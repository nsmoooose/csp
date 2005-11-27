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
 * @file NumericalMethod.cpp
 *
 **/

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/numeric/NumericalMethod.h>
#include <csp/csplib/numeric/VectorField.h>
#include <cmath>

CSP_NAMESPACE

using Vector::Vectord;

NumericalMethod::NumericalMethod():
	m_VectorField(0),
	m_State(),
	m_Name("Undefined numerical method!") {
}

NumericalMethod::NumericalMethod(VectorField *pvf):
	m_VectorField(pvf),
	m_State(),
	m_Name("Undefined numerical method!")
{
	if (m_VectorField)
		m_Dimension = m_VectorField->getDimension();
	else {
		m_Dimension = 2;
		CSPLOG(ERROR, PHYSICS) << "NumericalMethod: VectorField dimension undefined! (default is 2)";
	}
}

void NumericalMethod::setVectorField(VectorField *pvf) {
		m_VectorField = pvf;
		m_Dimension = m_VectorField->getDimension();
}

void NumericalMethod::setPrecision(double precision) {
	m_Epsilon = precision;
}

NumericalMethod::~NumericalMethod(){
	if (m_VectorField && (m_VectorField->getNumericalMethod() != this)) {
		std::cerr << "\nDeleting vector field in ~NumericalMethod(): " << m_Name << std::endl;
		delete m_VectorField;
		m_VectorField = 0;
	}
}

std::string const &NumericalMethod::getName() const {
	return m_Name;
}

void NumericalMethod::printState() const {
	m_State.print();
}

bool NumericalMethod::hasFailed() const {
	return m_State.getFailed();
}

//===========================================================================

double const RungeKutta::PGROW  = -0.20;
double const RungeKutta::PSHRNK = -0.25;
double const RungeKutta::FCOR   = 0.06666666;         // 1.0/15.0

double const RungeKutta::SAFETY = 0.9;
double const RungeKutta::ERRCON = 6.0e-4;

unsigned int const RungeKutta::MAXSTP = 8;
double const       RungeKutta::TINY   = std::numeric_limits<float>::epsilon();//1.0e-30;


Vectord const &RungeKutta::rk4(Vectord const &y, Vectord const &dydx, double x, double h) const {	
	static Vectord yout(m_Dimension), yt(m_Dimension);

	double hh = h * 0.5;
	unsigned short i = 0;

	for (; i < m_Dimension; ++i) {
		yt[i] = y[i] + hh * dydx[i];
	}

	double xh = x + hh;

	Vectord dyt = m_VectorField->f(xh, yt);
	for (i = 0; i < m_Dimension; ++i) {
		yt[i] = y[i] + hh * dyt[i];
	}

	Vectord dym = m_VectorField->f(xh, yt);
	for (i = 0; i < m_Dimension; ++i) {
		yt[i] = y[i] + h * dym[i];
		dym[i] += dyt[i];
	}
	
	dyt = m_VectorField->f(x + h, yt);
	double h6 = h / 6.0;
	for (i = 0; i < m_Dimension; ++i) {
		yout[i] = y[i] + h6 * (dydx[i] + dyt[i] + 2.0 * dym[i]);
	}

	return yout;
}

Vectord const &RungeKutta::rkqc(Vectord &y, Vectord &dydx, double &x, double htry, double eps,
                                       Vectord const &yscal, double &hdid, double &hnext) {
	static Vectord y1, ysav;

	double xsav = x;
	ysav = y;
	Vectord dysav = dydx;
	double h = htry;
	unsigned short i;
	bool loop = true;
	Vectord ytemp;

	//PROF0(RKQC);
	do {
		double hh = 0.5 * h;
		ytemp = rk4(ysav, dysav, xsav, hh);
		x = xsav + hh;
		dydx = m_VectorField->f(x, ytemp);
		y1 = rk4(ytemp, dydx, x, hh);
		x = xsav + h;
		if (x == xsav) {
			m_State << "Step size too small in RungeKutta::rkqc\n";
			loop = false;
		} else {
			ytemp = rk4(ysav, dysav, xsav, h);
			double errmax = 0.0;
			for (i = 0; i < m_Dimension; ++i) {
				ytemp[i] = y1[i] - ytemp[i];
				errmax = std::max(errmax,fabs(ytemp[i] / yscal[i]));
			}
			errmax /= eps;
			if (errmax <= 1.0) {
				hdid = h;
				hnext = (errmax > ERRCON ?
					SAFETY * h * pow(errmax,PGROW) : 4.0 * h);
				loop = false;
			} else {
			 h *= SAFETY * pow(errmax,PSHRNK);
			}
		}
	}
	while ( loop );
	//PROF1(RKQC, 100);
	if (m_State.getFailed()) {
		return ysav;
	} else {
		for (i = 0; i < m_Dimension; ++i)
			y1[i] += ytemp[i] * FCOR;
		return y1;
	}
}

Vectord const &RungeKutta::odeint(Vectord const &ystart, double x1, double x2, double eps, double h1,
                                  double hmin, unsigned int &nok, unsigned int &nbad) {
	static Vectord yscal(m_Dimension), y;

	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	nok = nbad =  0;
	y = ystart;
	unsigned int nstp = 0;
	do {
		Vectord dydx = m_VectorField->f(x, y);
		for (unsigned short i = 0;i < m_Dimension; ++i) {
			yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;
		}

		if ((x+h-x2)*(x+h-x1) > 0.0) h = x2 - x;
		y = rkqc(y, dydx, x, h, eps, yscal, hdid, hnext);
		if ( hdid == h ) ++nok; else ++nbad;
		if ( (x-x2) * (x2-x1) >= 0.0 ) {
			return y;
		} else {
			if (fabs(hnext) <= hmin) {
				m_State << "Step size too small in RungeKutta::odeint\n";
			} else {
				h = hnext;
			}
		}
	}
	while ( ++nstp < MAXSTP && !m_State.getFailed());
	if (nstp == MAXSTP) {
		m_State << "Too many steps in RungeKutta::odeint\n";
	}
	return ystart;
}

Vectord const& RungeKutta::quickSolve(Vectord &y0, double t0, double dt) {
	static Vectord y;
	y = rk4(y0, m_VectorField->f(t0,y0), t0, dt);
	return y;
}



Vectord const& RungeKutta::enhancedSolve(Vectord &y0, double t0, double dt) {
	static Vectord result;
	unsigned int nok,nbad;
	m_State = State();
	//PROF0(_2ODEINT);
	result = odeint(y0, t0, t0+dt, m_Epsilon, m_hestimate, m_hmin, nok, nbad);
	//PROF1(_2ODEINT, 100);
	//m_State << "RungeKutta::enhancedSolve nok =" << nok << "; nbad = " << nbad << "\n" << std::endl;
	return result;
}

//=================================================================

double const RungeKuttaCK::PGROW  = -0.20;
double const RungeKuttaCK::PSHRNK = -0.25;

double const RungeKuttaCK::SAFETY = 0.9;
double const RungeKuttaCK::ERRCON = 1.89e-4;

unsigned int const RungeKuttaCK::MAXSTP = 10000;//10000
double const       RungeKuttaCK::TINY   = std::numeric_limits<float>::epsilon();


Vectord const & RungeKuttaCK::rkck(Vectord const &y, Vectord const &dydx, double x, double h,
                                   Vectord& yerr) const {
	static const double a2 = 0.2, a3 = 0.3, a4 = 0.6, a5 = 1.0, a6 = 0.875,
	b21 = 0.2,
	b31 = 3.0/40.0, b32 = 9.0/40.0,
	b41 = 0.3, b42 = -0.9, b43 =1.2,
	b51 = -11.0/54.0, b52 = 2.5, b53 = -70.0/27.0, b54 = 35.0/27.0,
	b61 = 1631.0/55296.0, b62=175.0/512.0, b63=575.0/13824.0, b64=44275.0/110592.0, b65=253.0/4096.0,
	c1 = 37.0/378.0, c3 = 250.0/621.0, c4 = 125.0/594.0, c6 = 512.0/1771.0,
	dc5 = -277.0/14336.0,
	dc1 = c1 - 2825.0/27648.0, dc3 = c3 - 18575.0/48384.0, dc4 = c4 - 13525.0/55296.0, dc6 = c6 - 0.25;

	static Vectord ytemp(m_Dimension);

	unsigned short i = 0;
	register short int const dim = m_Dimension;
	for (; i < dim; ++i) {
		ytemp[i] = y[i] + b21 * h * dydx[i];
	}
	Vectord ak2 = m_VectorField->f(x + a2*h, ytemp);
	for (i = 0; i < dim; ++i) {
		ytemp[i] = y[i] + h * (b31*dydx[i] + b32*ak2[i]);
	}
	Vectord ak3 = m_VectorField->f(x + a3*h, ytemp);
	for (i = 0; i < dim; ++i) {
		ytemp[i] = y[i] + h * (b41*dydx[i] + b42*ak2[i] + b43*ak3[i]);
	}
	Vectord ak4 = m_VectorField->f(x + a4*h, ytemp);
	for (i = 0; i < dim; ++i) {
		ytemp[i] = y[i] + h * (b51*dydx[i] + b52*ak2[i] + b53*ak3[i] + b54*ak4[i]);
	}
	Vectord ak5 = m_VectorField->f(x + a5*h, ytemp);
	for (i = 0; i < dim; ++i) {
		ytemp[i] = y[i] + h * (b61*dydx[i] + b62*ak2[i] + b63*ak3[i] + b64*ak4[i] + b65*ak5[i]);
	}
	Vectord ak6 = m_VectorField->f(x + a6*h, ytemp);
	for (i = 0; i < dim; ++i) {
		ytemp[i] = y[i] + h * (c1*dydx[i] + c3*ak3[i] + c4*ak4[i] + c6*ak6[i]);
	}
	for (i = 0; i < dim; ++i) {
		yerr[i] = h * (dc1*dydx[i] + dc3*ak3[i] + dc4*ak4[i] + dc5*ak5[i] + dc6*ak6[i]);
	}
	return ytemp;
}

Vectord const &RungeKuttaCK::rkqs(Vectord &y, Vectord &dydx, double &x, double htry, double eps,
                                         Vectord const &yscal, double &hdid, double &hnext) {
	static Vectord yerr(m_Dimension);
	double h = htry, xnew, errmax;
	unsigned short i;
	bool loop = true;
	Vectord ytemp;
	do {
		//PROF0(RKCK);
		ytemp = rkck(y,dydx,x,h,yerr);
		//PROF1(RKCK, 1);
		errmax = 0.0;
		for (i = 0; i < m_Dimension; ++i) {
			errmax = std::max(errmax,fabs(yerr[i]/yscal[i]));
		}
		errmax /= eps;
		if (errmax <= 1.0) {
			loop = false;
		} else {
			 double	htemp =	SAFETY * h * pow(errmax,PSHRNK);
			 h = (h>=0.0 ? std::max(htemp,0.1 * h) : std::min(htemp,0.1*h));
			 xnew =	x + h;
			 if (x == xnew) {
				 m_State << "Stepsize underflow in RungeKuttaCK::rkqs\n";
			 }
		}
	}
	while (	loop );
	if (!m_State.getFailed()) {
		if (errmax > ERRCON) {
			hnext =	SAFETY * h * pow(errmax,PGROW);
		} else {
			hnext =	5.0 * h;
		}
		x += (hdid = h);
		for (i = 0; i < m_Dimension; ++i) {
			y[i] = ytemp[i];
		}
	}
	return y;
}

Vectord const &RungeKuttaCK::odeint(Vectord const &ystart, double x1, double x2, double eps, double h1,
                                    double hmin, unsigned int &nok, unsigned int &nbad)
{
	static Vectord yscal(m_Dimension), y;

	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	nok = nbad =  0;
	y = ystart;
	unsigned int nstp = 0;
	double const delta21 = x2 - x1;

	do {
		Vectord dydx = m_VectorField->f(x, y);
		for (unsigned short i = 0;i < m_Dimension; ++i) {
			yscal[i] = fabs(y[i]) + fabs(dydx[i] * h) + TINY;
		}

		if ((x+h-x2)*(x+h-x1) > 0.0) h = x2 - x;
		//PROF0(RKQS);
		y = rkqs(y, dydx, x, h, eps, yscal, hdid, hnext);
		//PROF1(RKQS, 1);
		if ( hdid == h ) ++nok; else ++nbad;
		if ( (x-x2) * delta21 >= 0.0 ) {
			return y;
		} else {
			if (fabs(hnext) <= hmin) {
				m_State << "Step size too small in RungeKuttaCK::odeint\n";
			} else {
				h = hnext;
			}
		}
	}
	while (++nstp < MAXSTP && !m_State.getFailed());
	if (nstp == MAXSTP) {
		m_State << "Too many steps in RungeKuttaCK::odeint\n";
	}

	return ystart;
}

Vectord const& RungeKuttaCK::quickSolve(Vectord &y0, double t0, double dt) {
	static Vectord y, yerr(m_Dimension);
	y = rkck(y0, m_VectorField->f(t0,y0), t0, dt, yerr);
	return y;
}



Vectord const& RungeKuttaCK::enhancedSolve(Vectord &y0, double t0, double dt) {
	static Vectord result;
	unsigned int nok,nbad;
	m_State = State();
	result = odeint(y0, t0, t0+dt, m_Epsilon, m_hestimate, m_hmin, nok, nbad);
	//std::cout << "RungeKuttaCK::enhancedSolve nok = " << nok << "; nbad = " << nbad << "\n" << std::endl;
	return result;
}

//=================================================================

double const		RKCK_VS_VO::SF = 0.9;
unsigned int const	RKCK_VS_VO::MAXSTEP = 8;
double const		RKCK_VS_VO::TINY   = std::numeric_limits<float>::epsilon();//1.0e-30;

RKCK_VS_VO::RKCK_VS_VO(VectorField *vectorField, double epsilon, double Hmin, double Hestimate):
	NumericalMethod(vectorField),
	m_Hmin(Hmin),
	m_Hestimate(Hestimate),
	m_Twiddle(3),
	m_Quit(3)
{
	m_Epsilon = epsilon;
	m_Name = "Runge-Kutta Cash-Karp variable order (extrapolation at 5), variable step size";
	resize();
}

void RKCK_VS_VO::resize() {
	m_y2.resize(m_Dimension);
	m_y3.resize(m_Dimension);
	m_y4.resize(m_Dimension);
	m_y5.resize(m_Dimension);
	m_ytemp.resize(m_Dimension);
	m_Result.resize(m_Dimension);
	m_k1.resize(m_Dimension);
	m_k2.resize(m_Dimension);
	m_k3.resize(m_Dimension);
	m_k4.resize(m_Dimension);
}

void RKCK_VS_VO::setVectorField(VectorField *pvf) {
	NumericalMethod::setVectorField(pvf);
	resize();
}

double RKCK_VS_VO::rkck12(double a, double h, Vectord const &ystart) {
	static double const c2 = 1.0/5.0;
	static double const a21 = 1.0/5.0;
	static double const b21 = -3.0/2.0;
	static double const b22 = 5.0/2.0;

#ifdef USE_VALARRAY_ARITHMETIC
	Vectord y1 = ystart  + h * m_k1;
	m_ytemp = ystart + h * a21 * m_k1;
	m_k2 = m_VectorField->f(a + c2 * h, m_ytemp);
	m_y2 = ystart + h * (b21 * m_k1 + b22 * m_k2);
#else
	Vectord y1 = ystart   + h * m_k1;
	//for (size_type i = 0; i < m_Dimension; ++i)
	//	y1[i] = ystart[i]  + h * m_k1[i];
	//m_ytemp = ystart + h * a21 * m_k1;
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_ytemp[i] = ystart[i]   + h * a21 * m_k1[i];
	}
    m_k2 = m_VectorField->f(a + c2 * h, m_ytemp);
    //m_y2 = ystart + h * (b21 * m_k1 + b22 * m_k2);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_y2[i] = ystart[i]   + h * (b21 * m_k1[i]   + b22 * m_k2[i]);
	}
#endif
	Vectord verr1 = m_y2 - y1;
	double err1 = sqrt(norm_2(verr1));
	/*
	double err1 = 0.0;
	for(size_type i = 0; i < m_Dimension; ++i) {
		double x = m_y2[i] - y1[i];
		err1 += x*x;
	}
	
    return sqrt(sqrt(err1));
	*/
	return err1;
}

double RKCK_VS_VO::rkck23(double a, double h, Vectord const &ystart) {
	static double const c3 = 3.0/10.0;
	static double const c4 = 3.0/5.0;
	static double const a31 = 3.0/40.0;
	static double const a32 = 9.0/40.0;
	static double const a41 = 3.0/10.0;
	static double const a42 = -9.0/10.0;
	static double const a43 = 6.0/5.0;
	static double const b31 = 19.0/54.0;
	static double const b33 = -10.0/27.0;
	static double const b34 = 55.0/54.0;
	static double const third = 1.0/3.0;

#ifdef USE_VALARRAY_ARITHMETIC
	m_ytemp = ystart + h * ( a31 * m_k1 + a32 * m_k2);
	m_k3 = m_VectorField->f(a + c3*h, m_ytemp);
	m_ytemp = ystart + h * (a41 * m_k1 + a42 * m_k2 + a43 * m_k3);
	m_k4 = m_VectorField->f(a + c4*h, m_ytemp);
	m_y3 = ystart + h * (b31 * m_k1 + b33 * m_k3 + b34 * m_k4);
#else
	//m_ytemp = ystart + h * ( a31 * m_k1 + a32 * m_k2);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_ytemp[i] = ystart[i]   + h * ( a31 * m_k1[i] + a32 * m_k2[i]);
	}
	m_k3 = m_VectorField->f(a + c3*h, m_ytemp);
	//m_ytemp = ystart + h * (a41 * m_k1 + a42 * m_k2 + a43 * m_k3);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_ytemp[i] = ystart[i]   + h * ( a41 * m_k1[i] + a42 * m_k2[i] + a43 * m_k3[i]);
	}
	m_k4 = m_VectorField->f(a + c4*h, m_ytemp);
	//m_y3 = ystart + h * (b31 * m_k1 + b33 * m_k3 + b34 * m_k4);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_y3[i] = ystart[i] + h * (b31 * m_k1[i] + b33 * m_k3[i] + b34 * m_k4[i]);
	}
#endif
	Vectord verr2 = m_y3 -  m_y2;
	double err2 = pow(norm_2(verr2), third);
	/*
	double err2 = 0.0;
	for(size_type i = 0; i < m_Dimension; ++i) {
		double x = y3[i]  - m_y2[i];
		err2 += x*x;
	}
	*/
	return err2;
	//return pow(sqrt(err2),third);
}

double RKCK_VS_VO::rkck45(double a, double h, Vectord const &ystart) {
	static double const c6 = 7.0/8.0;
	//static double const c5 = 1.0;
	static double const a51 = -11.0/54.0;
	static double const a52 = 5.0/2.0;
	static double const a53 = -70.0/27.0;
	static double const a54 = 35.0/27.0;
	static double const a61 = 1631.0/55296.0;
	static double const a62 = 175.0/512.0;
	static double const a63 = 575.0/13824.0;
	static double const a64 = 44275.0/110592.0;
	static double const a65 = 253.0/4096.0;
	static double const b41 = 2825.0/27648.0;
	static double const b43 = 18575.0/48384.0;
	static double const b44 = 13525.0/55296.0;
	static double const b45 = 277.0/14336.0;
	static double const b46 = 1.0/4.0;
	static double const b51 = 37.0/378.0;
	static double const b53 = 250.0/621.0;
	static double const b54 = 125.0/594.0;
	static double const b56 = 512.0/1771.0;
#ifdef USE_VALARRAY_ARITHMETIC
	m_ytemp = ystart + h * ( a51 * m_k1 + a52 * m_k2 + a53 * m_k3 + a54 * m_k4);
	Vectord k5 = m_VectorField->f(a + h, m_ytemp);
	m_ytemp = ystart + h * (a61 * m_k1 + a62 * m_k2 + a63 * m_k3 + a64 * m_k4 + a65 * k5);
	Vectord k6 = m_VectorField->f(a + c6*h, m_ytemp);
	m_y4 = ystart + h * (b41 * m_k1 + b43 * m_k3 + b44 * m_k4 + b45 * k5 + b46 * k6);
	m_y5 = ystart + h * (b51 * m_k1 + b53 * m_k3 + b54 * m_k4 + b56 * k6);
#else
	//m_ytemp = ystart + h * ( a51 * m_k1 + a52 * m_k2 + a53 * m_k3 + a54 * m_k4);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_ytemp[i] = ystart[i]   + h * ( a51 * m_k1[i] + a52 * m_k2[i] + a53 * m_k3[i] + a54 * m_k4[i]);
	}
	Vectord k5 = m_VectorField->f(a + h, m_ytemp);
	//m_ytemp = ystart + h * (a61 * m_k1 + a62 * m_k2 + a63 * m_k3 + a64 * m_k4 + a65 * k5);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_ytemp[i] = ystart[i]  + h * (a61 * m_k1[i] + a62 * m_k2[i] + a63 * m_k3[i] + a64 * m_k4[i] + a65 * k5[i]);
	}
	Vectord k6 = m_VectorField->f(a + c6*h, m_ytemp);
	//m_y4 = ystart + h * (b41 * m_k1 + b43 * m_k3 + b44 * m_k4 + b45 * k5 + b46 * k6);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_y4[i] = ystart[i] + h * (b41 * m_k1[i] + b43 * m_k3[i] + b44 * m_k4[i] + b45 * k5[i] + b46 * k6[i]);
	}
	//m_y5 = ystart + h * (b51 * m_k1 + b53 * m_k3 + b54 * m_k4 + b56 * k6);
	for (size_type i = 0; i < m_Dimension; ++i) {
		m_y5[i] = ystart[i] + h * (b51 * m_k1[i] + b53 * m_k3[i] + b54 * m_k4[i] + b56 * k6[i]);
	}
#endif
	Vectord verr4 = m_y5 - m_y4;
	double err4 = pow(norm_2(verr4), 0.2);
	/*
	double err4 = 0.0;
	for(size_type i = 0; i < m_Dimension; ++i) {
		double x = m_y5[i]  - y4[i];
		err4 += x*x;
	}
	return pow(sqrt(err4),0.2);
	*/
	return err4;
}

Vectord const  &RKCK_VS_VO::vrkf(Vectord &ystart, double a, double h, double &hdid, double &hnext) {
	static double const third = 1.0/3.0;
	static double const twothird = 2.0 * third;
	static double const inv_eps2 = 1.0 / sqrt(m_Epsilon);
	static double const inv_eps3 = 1.0 / pow(m_Epsilon, third);
	static double const inv_eps5 = 1.0 / pow(m_Epsilon, 0.2);

	std::vector<double> e(3);
	bool loop = true;
	unsigned int nstp = 0;

	m_k1 = m_VectorField->f(a,ystart);
	do {
		double err1 = rkck12(a, h, ystart);
		e[1] = err1 * inv_eps2 + TINY;
		if (e[1] > m_Twiddle[1] * m_Quit[1]) { // begin (e[1] > m_Twiddle[1] * m_Quit[1])
			h *= std::max(0.2, SF * m_Quit[1] / e[1]);
			loop = true;
		} // end (e[1] > m_Twiddle[1] * m_Quit[1])
		else { // begin (e[1] <= m_Twiddle[1] * m_Quit[1])
			double err2 = rkck23(a, h, ystart);
			e[2] = err2 * inv_eps3 + TINY;
			if (e[2] > m_Twiddle[2] * m_Quit[2]) { // begin (e[2] > m_Twiddle[2] * m_Quit[2])
				if (e[1] < 1.0) { // begin (e[1] < 1.0)
					Vectord ve1_5 = 0.1 * (m_k2 - m_k1);
					double e1_5 = norm_2(ve1_5);
					if (e1_5 < m_Epsilon){
						hnext = 0.2 * h;
						hdid = h;
						m_ytemp = ystart + 0.1 * (m_k1 + m_k2);
						//for (size_type i = 0; i < m_Dimension; ++i)
						//	m_ytemp[i] = ystart[i] + 0.1 * (m_k1[i] + m_k2[i]);
						return m_ytemp;
					} else {
						h *= 0.2;
						loop = true;
					}
				} // end case (e[1] < 1.0)
				else { // begin (e[1] >= 1.0)
					h *= std::max(0.2, SF * m_Quit[2]/e[2]);
					loop = true;
				} //end (e[1] >= 1.0)
			} // end case (e[2] > m_Twiddle[2] * m_Quit[2])
			else { // begin (e[2] <= m_Twiddle[2] * m_Quit[2])
				double err4 = rkck45(a, h, ystart);
				double e4 = err4 * inv_eps5 + TINY;
				if (e4 > 1.0) { // begin (e4 > 1.0)
					for (size_t i = 1; i < 3; ++i) { // begin for
						double r = e[i]/m_Quit[i];
						if (r < m_Twiddle[i]) {
							m_Twiddle[i] = std::max(1.1, r);
						}
					} // end for
					if (e[2] < 1.0) {
						//double e3_5 = norm_2(0.1 * h * (m_k1 - 2.0 * m_k3 + m_k4));
						Vectord e3_5 = 0.1 * h * (m_k1 - 2.0 * m_k3 + m_k4);
						if (norm_2(e3_5) < m_Epsilon) {
							hnext = 0.6 * h;
							hdid = h;
							//y = ystart + 0.1 * h * (m_k1 - 4.0 * m_k3 + m_k4); // third order solution
							m_ytemp = ystart + e3_5 - 0.2 * h * m_k3; // third order solution
							return m_ytemp;
						}
					}
					// begin (e3_5 >= m_Epsilon || e2 >= 1.0)
					if (e[1] < 1.0) { // begin (e[1] < 1.0)
						//double e1_5 = norm_2(0.1 * h * (m_k2 - m_k1));
						Vectord e1_5 = 0.1 * h * (m_k2 - m_k1);
						if (norm_2(e1_5) < m_Epsilon) { // begin (e1_5 < m_Epsilon)
								hnext = 0.2 * h;
								hdid = h;
								//y = ystart + 0.1 * h * (m_k2 + m_k1); // 2nd order solution
								m_ytemp = ystart + e1_5 + 0.2 * h * m_k1;
								return m_ytemp;
						} // end (e1_5 < m_Epsilon)
						else { // begin (e1_5 >= m_Epsilon)
							h *= 0.2;
							loop = true;
						} // end (e1_5 >= m_Epsilon)
					} // end (e[1] <1.0)
					else { // begin (e1 >= 1.0)
						h *= std::max(0.2, SF/e4);
						loop = true;
					} // end (e1 >= 1.0)
				} // end (e4 > 1.0)
				else { // begin case (e[4] <= 1.0)
					//std::cout << "Order 5 solution\n";
					hnext = std::min(5.0,SF/e4) * h;
					hdid = h;
					for (size_t i = 1; i< 3;++i) { // begin for i
						double q = e[i]/e4;
						if (q > m_Quit[i]) {
							q = std::min(q, 10.0 * m_Quit[i]);
						} else {
							q = std::max(q, twothird * m_Quit[i]);
						}
						m_Quit[i] = std::max(1.0,std::min(10000.0,q));
					} // end for i
					return m_y5;
				} //end case (e[4] <= 1.0)
			} // end (e[2] <= m_Twiddle[2] * m_Quit[2])
		} // end (e[1] <= m_Twiddle[1] * m_Quit[1])
	} // do
	while (loop && ++nstp < MAXSTEP && fabs(h) > m_Hmin);
	//if (nstp == MAXSTEP)
	//	m_State << "Too many steps in RKCK_VS_VO::vrkf: " << MAXSTEP << "\n";
	m_State.setFailed(true);
	return ystart;
}

Vectord const &RKCK_VS_VO::vrkfBound(Vectord &ystart, double a, double h, double h1,
                                     unsigned int &/*nok*/, unsigned int &/*nbad*/) {
	double b = a + h, x = a, hdid, hnext;
	// CAUTION: unusual index to match math notation
	m_Twiddle[1] = 1.5; m_Twiddle[2] = 1.1;
	m_Quit[1] = m_Quit[2] = 100.0;
	unsigned int nstp = 0;
	m_Result = ystart;
	do {
		hnext = hdid = 0.0;
		m_Result = vrkf(m_Result,  x, h1, hdid, hnext);
		if (fabs(hdid) < m_Hmin) {
			m_State.setFailed(true);
			//m_State << "Step size too small in RKCK_VS_VO::vrkfBound h = " << fabs(hdid) << "\n";
		} else {
			x += hdid;
			h1 = std::max(0.0,std::min(hnext, b - x));
			if (fabs(h1) < m_Hmin) {
				return m_Result;
			}
		}
	}
	while (++nstp < MAXSTEP && !m_State.getFailed());
	m_State.setFailed(true);
	//m_State << "Too many steps in RKCK_VS_VO::vrkfBound: " << MAXSTEP << "\n";
	return ystart;
}

Vectord const& RKCK_VS_VO::quickSolve(Vectord &y0, double t0, double dt) {
	//m_State << "RKCK_VS_VO::quickSolve is called.\n";
	m_k1 = m_VectorField->f(t0,y0);
	double err = rkck12(t0, dt, y0);
	err = rkck23(t0,dt,y0);
	err = rkck45(t0,dt,y0);
	//return m_y2;
	return m_y5;
}

Vectord const &RKCK_VS_VO::enhancedSolve(Vectord &y0, double t0, double dt) {
	unsigned int nok, nbad;
	m_State.setFailed(false);
	//std::cerr << "RungeKuttaCK::enhancedSolve nok = " << nok << "; nbad = " << nbad << "\n" << std::endl;
	return vrkfBound(y0, t0, dt, std::min(m_Hestimate,dt), nok, nbad);
}

CSP_NAMESPACE_END
