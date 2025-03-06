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

#include <csp/csplib/util/Log.h>
#include <csp/csplib/numeric/NumericalMethod.h>
#include <csp/csplib/numeric/VectorField.h>
#include <cmath>
#include <iostream> // XXX

namespace csp {

namespace numeric {

NumericalMethod::NumericalMethod(double epsilon) {
	m_epsilon = epsilon;
	m_dimension = 0;
	m_VectorField = 0;
	m_steps = 1000;
}

NumericalMethod::~NumericalMethod() {
}

void NumericalMethod::redimension() {
}

void NumericalMethod::setVectorField(VectorField *vector_field) {
	m_VectorField = vector_field;
	if (m_dimension != vector_field->dimension()) {
		m_dimension = vector_field->dimension();
		redimension();
	}
}

void NumericalMethod::setPrecision(double precision) {
	m_epsilon = precision;
}

void NumericalMethod::setSteps(unsigned steps) {
	m_steps = steps;
}

void NumericalMethod::f(double x, Vectord const &y, Vectord &dydx) {
	m_VectorField->f(x, y, dydx);
}

//===========================================================================

double const RungeKutta::PGROW  = -0.20;
double const RungeKutta::PSHRNK = -0.25;
double const RungeKutta::FCOR   = (1.0 / 15.0);

double const RungeKutta::SAFETY = 0.9;
double const RungeKutta::ERRCON = 6.0e-4;

double const RungeKutta::TINY = std::numeric_limits<float>::epsilon();

std::string RungeKutta::getName() const {
	return "Runge-Kutta order 4 with variable step size";
}

void RungeKutta::redimension() {
	const unsigned dim = dimension();
	m_rk4_yt.resize(dim);
	m_rk4_dyt.resize(dim);
	m_rk4_dym.resize(dim);
	m_rkqc_ytemp.resize(dim);
	m_rkqc_y1.resize(dim);
	m_rkqc_dy.resize(dim);
	m_odeint_yscal.resize(dim);
	m_odeint_dydx.resize(dim);
}

void RungeKutta::rk4(Vectord const &y0, Vectord const &dydx, double x, double h, Vectord &y) {
	const unsigned dim = dimension();
	double hh = h * 0.5;
	for (unsigned i = 0; i < dim; ++i) {
		m_rk4_yt[i] = y0[i] + hh * dydx[i];
	}

	double xh = x + hh;
	f(xh, m_rk4_yt, m_rk4_dyt);
	for (unsigned i = 0; i < dim; ++i) {
		m_rk4_yt[i] = y0[i] + hh * m_rk4_dyt[i];
	}

	f(xh, m_rk4_yt, m_rk4_dym);
	for (unsigned i = 0; i < dim; ++i) {
		m_rk4_yt[i] = y0[i] + h * m_rk4_dym[i];
		m_rk4_dym[i] += m_rk4_dyt[i];
	}
	
	f(x + h, m_rk4_yt, m_rk4_dyt);
	double h6 = h / 6.0;
	for (unsigned i = 0; i < dim; ++i) {
		y[i] = y0[i] + h6 * (dydx[i] + m_rk4_dyt[i] + 2.0 * m_rk4_dym[i]);
	}
}

bool RungeKutta::rkqc(Vectord const &y0, Vectord const &dydx, Vectord &y, double &x, double htry, double eps,
                      Vectord const &yscal, double &hdid, double &hnext) {
	const unsigned dim = dimension();
	const double x0 = x;
	double h = htry;

	while (true) {
		// first try two half steps
		double hh = 0.5 * h;
		rk4(y0, dydx, x0, hh, m_rkqc_ytemp);
		x = x0 + hh;
		f(x, m_rkqc_ytemp, m_rkqc_dy);
		rk4(m_rkqc_ytemp, m_rkqc_dy, x, hh, m_rkqc_y1);
		x = x0 + h;
		if (x == x0) {
			CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Step size too small in RungeKutta::rkqc";
			return false;
		}

		// now try a single full step
		rk4(y0, dydx, x0, h, m_rkqc_ytemp);

		// compare the two results
		double errmax = 0.0;
		for (unsigned i = 0; i < dim; ++i) {
			m_rkqc_ytemp[i] = m_rkqc_y1[i] - m_rkqc_ytemp[i];
			errmax = std::max(errmax, fabs(m_rkqc_ytemp[i] / yscal[i]));
		}

		// we're done if the truncation error is small
		errmax /= eps;
		if (errmax <= 1.0) {
			hdid = h;
			hnext = (errmax > ERRCON ? SAFETY * h * pow(errmax, PGROW) : 4.0 * h);
			for (unsigned i = 0; i < dim; ++i) {
				y[i] = m_rkqc_y1[i] + m_rkqc_ytemp[i] * FCOR;
			}
			return true;
		}

		// otherwise retry with a smaller step
		h *= SAFETY * pow(errmax, PSHRNK);
	}
}

bool RungeKutta::odeint(Vectord const &y0, Vectord &y, double x1, double x2, double eps, double h1,
                        double hmin, unsigned int &nok, unsigned int &nbad) {
	const unsigned dim = dimension();
	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	nok = nbad = 0;
	y = y0;
	for (unsigned nstp = 0; nstp < steps(); ++nstp) {
		f(x, y, m_odeint_dydx);
		for (unsigned short i = 0; i < dim; ++i) {
			m_odeint_yscal[i] = fabs(y[i]) + fabs(m_odeint_dydx[i] * h) + TINY;
		}
		if ((x+h-x2)*(x+h-x1) > 0.0) h = x2 - x;
		if (!rkqc(y, m_odeint_dydx, y, x, h, eps, m_odeint_yscal, hdid, hnext)) return false;
		if (hdid == h) ++nok; else ++nbad;
		if ((x-x2) * (x2-x1) >= 0.0) return true;
		if (fabs(hnext) < hmin) {
			CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Step size too small in RungeKutta::odeint";
			return false;
		}
		h = hnext;
	}

	CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Too many steps in RungeKutta::odeint";
	return false;
}

void RungeKutta::quickSolve(Vectord const &y0, Vectord &y, double t0, double dt) {
	f(t0, y0, y);
	rk4(y0, y, t0, dt, y);
}

bool RungeKutta::enhancedSolve(Vectord const &y0, Vectord &y, double t0, double dt) {
	unsigned int nok, nbad;
	const bool result = odeint(y0, y, t0, t0+dt, epsilon(), m_hestimate, m_hmin, nok, nbad);
	CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "RungeKutta::enhancedSolve nok = " << nok << "; nbad = " << nbad;
	return result;
}

//=================================================================

double const RungeKuttaCK::PGROW  = -0.20;
double const RungeKuttaCK::PSHRNK = -0.25;

double const RungeKuttaCK::SAFETY = 0.9;
double const RungeKuttaCK::ERRCON = 1.89e-4;

double const RungeKuttaCK::TINY = std::numeric_limits<float>::epsilon();

std::string RungeKuttaCK::getName() const {
	return "Runge-Kutta Cash-Karp order 5 variable step size";
}

void RungeKuttaCK::redimension() {
	const unsigned dim = dimension();
	m_rkqs_yerr.resize(dim);
	m_rkqs_ytemp.resize(dim);
	m_rkck_ak2.resize(dim);
	m_rkck_ak3.resize(dim);
	m_rkck_ak4.resize(dim);
	m_rkck_ak5.resize(dim);
	m_rkck_ak6.resize(dim);
	m_odeint_yscal.resize(dim);
	m_odeint_dydx.resize(dim);
}

void RungeKuttaCK::rkck(Vectord const &y0, Vectord const &dydx, double x, double h,
                        Vectord& y, Vectord* yerr) {
	// see Cash & Karp, p. 206, eq (5).
	static const double a2 = 0.2, a3 = 0.3, a4 = 0.6, a5 = 1.0, a6 = 0.875;
	static const double b21 = 0.2, b31 = 3.0/40.0, b32 = 9.0/40.0;
	static const double b41 = 0.3, b42 = -0.9, b43 = 1.2;
	static const double b51 = -11.0/54.0, b52 = 2.5, b53 = -70.0/27.0, b54 = 35.0/27.0;
	static const double b61 = 1631.0/55296.0, b62 = 175.0/512.0, b63 = 575.0/13824.0, b64 = 44275.0/110592.0, b65 = 253.0/4096.0;
	static const double c1 = 37.0/378.0, c3 = 250.0/621.0, c4 = 125.0/594.0, c6 = 512.0/1771.0;
	static const double dc1 = c1 - 2825.0/27648.0, dc3 = c3 - 18575.0/48384.0, dc4 = c4 - 13525.0/55296.0, dc5 = -277.0/14336.0, dc6 = c6 - 0.25;

	unsigned i = 0;
	const unsigned dim = dimension();
	for (; i < dim; ++i) {
		y[i] = y0[i] + b21 * h * dydx[i];
	}
	f(x + a2*h, y, m_rkck_ak2);
	for (i = 0; i < dim; ++i) {
		y[i] = y0[i] + h * (b31 * dydx[i] + b32 * m_rkck_ak2[i]);
	}
	f(x + a3*h, y, m_rkck_ak3);
	for (i = 0; i < dim; ++i) {
		y[i] = y0[i] + h * (b41 * dydx[i] + b42 * m_rkck_ak2[i] + b43 * m_rkck_ak3[i]);
	}
	f(x + a4*h, y, m_rkck_ak4);
	for (i = 0; i < dim; ++i) {
		y[i] = y0[i] + h * (b51 * dydx[i] + b52 * m_rkck_ak2[i] + b53 * m_rkck_ak3[i] + b54 * m_rkck_ak4[i]);
	}
	f(x + a5*h, y, m_rkck_ak5);
	for (i = 0; i < dim; ++i) {
		y[i] = y0[i] + h * (b61 * dydx[i] + b62 * m_rkck_ak2[i] + b63 * m_rkck_ak3[i] + b64 * m_rkck_ak4[i] + b65 * m_rkck_ak5[i]);
	}
	f(x + a6*h, y, m_rkck_ak6);
	for (i = 0; i < dim; ++i) {
		y[i] = y0[i] + h * (c1 * dydx[i] + c3 * m_rkck_ak3[i] + c4 * m_rkck_ak4[i] + c6 * m_rkck_ak6[i]);
	}
	if (yerr) {
		for (i = 0; i < dim; ++i) {
			(*yerr)[i] = h * (dc1 * dydx[i] + dc3 * m_rkck_ak3[i] + dc4 * m_rkck_ak4[i] + dc5 * m_rkck_ak5[i] + dc6 * m_rkck_ak6[i]);
		}
	}
}

bool RungeKuttaCK::rkqs(Vectord &y, Vectord &dydx, double &x, double htry, double eps,
                        Vectord const &yscal, double &hdid, double &hnext) {
	const unsigned dim = dimension();
	double h = htry, xnew, errmax;
	unsigned short i;
	while (true) {
		rkck(y, dydx, x, h, m_rkqs_ytemp, &m_rkqs_yerr);
		errmax = 0.0;
		for (i = 0; i < dim; ++i) {
			errmax = std::max(errmax, fabs(m_rkqs_yerr[i] / yscal[i]));
		}
		errmax /= eps;
		if (errmax <= 1.0) break;
		double htemp = SAFETY * h * pow(errmax, PSHRNK);
		h = (h >= 0.0 ? std::max(htemp, 0.1 * h) : std::min(htemp, 0.1 * h));
		xnew = x + h;
		if (x == xnew) {
			CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Stepsize underflow in RungeKuttaCK::rkqs";
			return false;
		}
	}
	if (errmax > ERRCON) {
		hnext = SAFETY * h * pow(errmax, PGROW);
	} else {
		hnext = 5.0 * h;
	}
	x += (hdid = h);
	y = m_rkqs_ytemp;
	return true;
}

bool RungeKuttaCK::odeint(Vectord const &y0, Vectord &y, double x1, double x2, double eps, double h1,
                          double hmin, unsigned int &nok, unsigned int &nbad) {
	const unsigned dim = dimension();

	double hnext, hdid;
	double x = x1;
	double h = (x2 > x1) ? fabs(h1) : -fabs(h1);
	double const delta21 = x2 - x1;

	nok = nbad =  0;
	y = y0;

	for (unsigned nstp = 0; nstp < steps(); ++nstp) {
		f(x, y, m_odeint_dydx);
		for (unsigned short i = 0;i < dim; ++i) {
			m_odeint_yscal[i] = fabs(y[i]) + fabs(m_odeint_dydx[i] * h) + TINY;
		}

		if ((x + h - x2) * (x + h - x1) > 0.0) h = x2 - x;
		if (!rkqs(y, m_odeint_dydx, x, h, eps, m_odeint_yscal, hdid, hnext)) return false;
		if (hdid == h) ++nok; else ++nbad;
		if ((x-x2) * delta21 >= 0.0) return true;
		if (fabs(hnext) <= hmin) {
			CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Step size too small in RungeKuttaCK::odeint: " << hnext;
			return false;
		}
		h = hnext;
	}

	CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Too many steps in RungeKuttaCK::odeint: " << steps();
	return false;
}

void RungeKuttaCK::quickSolve(Vectord const &y0, Vectord &y, double t0, double dt) {
	f(t0, y0, m_odeint_dydx);  // reusing m_odeint_dydx since odeing isn't called
	rkck(y0, m_odeint_dydx, t0, dt, y);
}

bool RungeKuttaCK::enhancedSolve(Vectord const &y0, Vectord &y, double t0, double dt) {
	unsigned int nok, nbad;
	const bool result = odeint(y0, y, t0, t0+dt, epsilon(), m_hestimate, m_hmin, nok, nbad);
	//CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "RungeKuttaCK::enhancedSolve nok = " << nok << "; nbad = " << nbad;
	return result;
}

//=================================================================

double const RKCK_VS_VO::SF = 0.9;
double const RKCK_VS_VO::TINY = std::numeric_limits<float>::epsilon();//1.0e-30;

RKCK_VS_VO::RKCK_VS_VO(double epsilon, double hmin, double hestimate):
	NumericalMethod(epsilon),
	m_hmin(hmin),
	m_hestimate(hestimate)
{
	for (int i = 0; i < 3; ++i) {
		m_twiddle[i] = 0.0;
		m_quit[i] = 0.0;
	}
	// virtual call from ctor won't reach subclasses, but RKCK_VS_VO isn't
	// intended to be subclassed.
	setPrecision(epsilon);
}

std::string RKCK_VS_VO::getName() const {
	return "Runge-Kutta Cash-Karp variable order (extrapolation at 5), variable step size";
}

void RKCK_VS_VO::redimension() {
	const unsigned dim = dimension();
	m_y2.resize(dim);
	m_y5.resize(dim);
	m_ytemp.resize(dim);
	m_k1.resize(dim);
	m_k2.resize(dim);
	m_k3.resize(dim);
	m_k4.resize(dim);
	m_k5.resize(dim);
	m_k6.resize(dim);
	m_e35.resize(dim);
}

void RKCK_VS_VO::setPrecision(double epsilon) {
	NumericalMethod::setPrecision(epsilon);
	m_inv_eps2 = 1.0 / sqrt(epsilon);
	m_inv_eps3 = 1.0 / pow(epsilon, (1.0 / 3.0));
	m_inv_eps5 = 1.0 / pow(epsilon, 0.2);
}

void RKCK_VS_VO::rkck12(double a, double h, Vectord const &y0, double *err1) {
	static double const c2 = 1.0/5.0;
	static double const a21 = 1.0/5.0;
	static double const b21 = -3.0/2.0;
	static double const b22 = 5.0/2.0;

	const unsigned dim = dimension();
	for (unsigned i = 0; i < dim; ++i) {
		m_ytemp[i] = y0[i] + h * a21 * m_k1[i];
	}
	f(a + c2 * h, m_ytemp, m_k2);
	for (unsigned i = 0; i < dim; ++i) {
		m_y2[i] = y0[i] + h * (b21 * m_k1[i] + b22 * m_k2[i]);
	}
	if (err1) {
		double sqerr = 0.0;
		for (unsigned i = 0; i < dim; ++i) {
			double y1 = y0[i] + h * m_k1[i];
			double d21 = m_y2[i] - y1;
			sqerr += d21 * d21;
		}
		*err1 = sqrt(sqrt(sqerr));
	}
}

void RKCK_VS_VO::rkck23(double a, double h, Vectord const &y0, double *err2) {
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

	const unsigned dim = dimension();
	for (unsigned i = 0; i < dim; ++i) {
		m_ytemp[i] = y0[i] + h * (a31 * m_k1[i] + a32 * m_k2[i]);
	}
	f(a + c3*h, m_ytemp, m_k3);
	for (unsigned i = 0; i < dim; ++i) {
		m_ytemp[i] = y0[i] + h * (a41 * m_k1[i] + a42 * m_k2[i] + a43 * m_k3[i]);
	}
	f(a + c4*h, m_ytemp, m_k4);
	if (err2) {
		double sqerr = 0.0;
		for (unsigned i = 0; i < dim; ++i) {
			double y3 = y0[i] + h * (b31 * m_k1[i] + b33 * m_k3[i] + b34 * m_k4[i]);
			double d32 = y3 - m_y2[i];
			sqerr += d32 * d32;
		}
		*err2 = pow(sqrt(sqerr), third);
	}
}

void RKCK_VS_VO::rkck45(double a, double h, Vectord const &y0, double *err4) {
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

	const unsigned dim = dimension();
	for (unsigned i = 0; i < dim; ++i) {
		m_ytemp[i] = y0[i] + h * (a51 * m_k1[i] + a52 * m_k2[i] + a53 * m_k3[i] + a54 * m_k4[i]);
	}
	f(a + h, m_ytemp, m_k5);
	for (unsigned i = 0; i < dim; ++i) {
		m_ytemp[i] = y0[i] + h * (a61 * m_k1[i] + a62 * m_k2[i] + a63 * m_k3[i] + a64 * m_k4[i] + a65 * m_k5[i]);
	}
	f(a + c6*h, m_ytemp, m_k6);
	for (unsigned i = 0; i < dim; ++i) {
		m_y5[i] = y0[i] + h * (b51 * m_k1[i] + b53 * m_k3[i] + b54 * m_k4[i] + b56 * m_k6[i]);
	}

	if (err4) {
		double sqerr = 0.0;
		for (unsigned i = 0; i < dim; ++i) {
			double y4 = y0[i] + h * (b41 * m_k1[i] + b43 * m_k3[i] + b44 * m_k4[i] + b45 * m_k5[i] + b46 * m_k6[i]);
			double d54 = m_y5[i] - y4;
			sqerr += d54 * d54;
		}
		*err4 = pow(sqrt(sqerr), 0.2);
	}
}

bool RKCK_VS_VO::vrkf(Vectord const &y0, Vectord &y, double a, double h, double &hdid, double &hnext) {
	static double const third = 1.0/3.0;
	static double const twothird = 2.0 * third;

	const unsigned dim = dimension();
	double e[3];
	double err = epsilon();

	f(a, y0, m_k1);
	for (unsigned nstp = 0; nstp < steps() && fabs(h) > m_hmin; ++nstp) {
		double err1;
		rkck12(a, h, y0, &err1);
		e[1] = err1 * m_inv_eps2 + TINY;
		if (e[1] > m_twiddle[1] * m_quit[1]) {
			h *= std::max(0.2, SF * m_quit[1] / e[1]);
			continue;
		}

		double err2;
		rkck23(a, h, y0, &err2);
		e[2] = err2 * m_inv_eps3 + TINY;
		if (e[2] > m_twiddle[2] * m_quit[2]) {
			if (e[1] < 1.0) {
				const double _0_1h = 0.1 * h;
				for (unsigned i = 0; i < dim; ++i) {
					m_ytemp[i] = _0_1h * (m_k2[i] - m_k1[i]);
				}
				double e1_5 = norm_2(m_ytemp);
				if (e1_5 < err){
					hnext = 0.2 * h;
					hdid = 0.2 * h;
					for (unsigned i = 0; i < dim; ++i) {
						y[i] = y0[i] + 0.1 * h * (m_k1[i] + m_k2[i]);  // second order solution @ 1/5 h
					}
					return true;
				}
				h *= 0.2;
			} else {
				h *= std::max(0.2, SF * m_quit[2] / e[2]);
			}
			continue;
		}

		double err4;
		rkck45(a, h, y0, &err4);
		double e4 = err4 * m_inv_eps5 + TINY;
		if (e4 > 1.0) {
			for (size_t i = 1; i < 3; ++i) {
				double r = e[i] / m_quit[i];
				if (r < m_twiddle[i]) {
					m_twiddle[i] = std::max(1.1, r);
				}
			}
			if (e[2] < 1.0) {
				const double _0_1h = 0.1 * h;
				for (unsigned i = 0; i < dim; ++i) {
					m_e35[i] = _0_1h * (m_k1[i] - 2.0 * m_k3[i] + m_k4[i]);
				}
				if (norm_2(m_e35) < err) {
					hnext = 0.6 * h;
					hdid = 0.6 * h;
					for (unsigned i = 0; i < dim; ++i) {
						y[i] = y0[i] + m_e35[i] + (0.6 * h) * m_k3[i]; // third order solution @ 3/5 h
					}
					return true;
				}
			} else // XXX not sure from reading cash/karp what happens here; seems like this should be 'else' though
			if (e[1] < 1.0) {
				const double _0_1h = 0.1 * h;
				double sqval = 0.0;
				for (unsigned i = 0; i < dim; ++i) {
					double e = _0_1h * (m_k2[i] - m_k1[i]);
					sqval += e*e;
				}
				const double e1_5 = sqrt(sqval);
				if (e1_5 < err) {
					hnext = 0.2 * h;
					hdid = 0.2 * h;
					for (unsigned i = 0; i < dim; ++i) {
						y[i] = y0[i] + 0.1 * h * (m_k1[i] + m_k2[i]);  // second order solution @ 1/5h
					}
					return true;
				} else {
					h *= 0.2;
				}
				continue;
			}
			h *= std::max(0.2, SF / e4);
			continue;
		}

		// accept the fifth order solution computed by rkck45 above.
		hnext = std::min(5.0, SF / e4) * h;
		hdid = h;
		for (size_t i = 1; i < 3; ++i) {
			double q = e[i] / e4;
			if (q > m_quit[i]) {
				q = std::min(q, 10.0 * m_quit[i]);
			} else {
				q = std::max(q, twothird * m_quit[i]);
			}
			m_quit[i] = std::max(1.0, std::min(10000.0, q));
		}
		y = m_y5;
		return true;
	}

	if (fabs(h) <= m_hmin) {
		CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Step size too small in RKCK_VS_VO::vrkf (x=" << a << ", h=" << h << ")";
	} else {
		CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Too many steps in RKCK_VS_VO::vrkf: " << steps();
	}

	return false;
}

bool RKCK_VS_VO::vrkfBound(Vectord const &y0, Vectord &y, double a, double h, double h1, unsigned int &/*nok*/, unsigned int &/*nbad*/) {
	double b = a + h, x = a, hdid, hnext;
	// CAUTION: unusual index to match math notation
	m_twiddle[1] = 1.5;
	m_twiddle[2] = 1.1;
	m_quit[1] = 100.0;
	m_quit[2] = 100.0;
	y = y0;
	for (unsigned nstp = 0; nstp < steps(); ++nstp) {
		hnext = hdid = 0.0;
		if (!vrkf(y, y, x, h1, hdid, hnext)) return false;
		if (fabs(hdid) < m_hmin) {
			CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Step size too small in RKCK_VS_VO::vrkfBound h = " << fabs(hdid);
			return false;
		}
		x += hdid;
		h1 = std::max(0.0, std::min(hnext, b - x));
		if (fabs(h1) < m_hmin) {
			return true;
		}
	}
	CSPLOG(Prio_DEBUG, Cat_NUMERIC) << "Too many steps in RKCK_VS_VO::vrkfBound: " << steps();
	return false;
}

void RKCK_VS_VO::quickSolve(Vectord const &y0, Vectord &y, double t0, double dt) {
	f(t0, y0, m_k1);
	rkck12(t0, dt, y0, NULL);
	rkck23(t0, dt, y0, NULL);
	rkck45(t0, dt, y0, NULL);
	y = m_y5;
}

bool RKCK_VS_VO::enhancedSolve(Vectord const &y0, Vectord &y, double t0, double dt) {
	unsigned int nok, nbad;
	return vrkfBound(y0, y, t0, dt, std::min(m_hestimate, dt), nok, nbad);
}

} // namespace numeric

} // namespace csp
