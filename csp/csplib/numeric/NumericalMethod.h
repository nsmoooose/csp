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

#ifndef __CSPLIB_NUMERIC_NUMERICALMETHOD_H__
#define __CSPLIB_NUMERIC_NUMERICALMETHOD_H__

#include <limits>
#include <string>

#include <csp/csplib/util/Export.h>
#include <csp/csplib/numeric/Vector.h>

namespace csp {

namespace numeric {

class VectorField;

// Interesting readings about numerical analysis of ODE can be found in:
// - M. Crouzeix, A.L. Mignot, Analyse numerique des equations
//   differentielles, Masson, 1984.
// - R.L. Burden, J.D. Faires, Numerical analysis, Brooks/Cole, 2001.

/// Abstract base class for numerical solvers of dynamical systems.
/// NumericalMethod subclasses bind to a VectorField instance that defines
/// the equation, and typically contain temporary state that is shared
/// across methods.  A single NumericalMethod instance can be shared by
/// multiple VectorFields, but there is significant overhead to switch
/// between VectorFields of different dimension.
class CSPLIB_EXPORT NumericalMethod {
public:
	/// Construct a NumericalMethod instance; setVectorField must be called
	/// before using.
	NumericalMethod(double epsilon);
	virtual ~NumericalMethod();

	/// Set the vector field f to solve (y' = f(t,y))
	/// NumericalMethod does not take ownership of the vector field pointer.
	void setVectorField(VectorField *vector_field);

	/// Set the desired precision of the solution.
	virtual void setPrecision(double precision);

	/// Limit the number of steps that the enhanced solver can take before
	/// falling back on the quick solver.  The default value is lange (e.g.,
	/// 1000), which may not be appropriate for realtime simulations.
	void setSteps(unsigned steps);

	/// Integrate the differential equation y' = f(t,y) using a sophisticated,
	/// accurate method (e.g., manage a variable step size, variable order,
	/// multistep, be implicit, ...).
	/// @param y0 initial solution at time t0
	/// @param y returns the final solution at time t0 + dt
	/// @param t0 the initial time
	/// @param dt the time interval of integration
	/// @return true if the integration succeeded, in which case y returns the
	///   result.  if the integration fails, the caller should fall back on
	///   quickSolve.
	virtual bool enhancedSolve(Vectord const &y0, Vectord& y, double t0, double dt) = 0;

	/// In case of that enhancedSolve fails, quickSolve serves as a fallback to
	/// guarantee a solution.  In general, it should be relatively fast and
	/// less accurate than enhancedSolve, but ideally more stable.  An implicit
	/// method may be required to assure more stability.
	/// @param y0 initial solution at time t0
	/// @param y returns the final solution at time t0 + dt
	/// @param t0 the initial time
	/// @param dt the time interval of integration
	virtual void quickSolve(Vectord const &y0, Vectord &y, double t0, double dt) = 0;

	/// @return the name of the numerical method.
	virtual std::string getName() const = 0;
	double epsilon() const { return m_epsilon; }
	unsigned dimension() const { return m_dimension; }
	unsigned steps() const { return m_steps; }

protected:
	void f(double x, Vectord const &y, Vectord &dydx);
	virtual void redimension();

private:
	double m_epsilon;
	unsigned m_dimension;
	unsigned m_steps;
	VectorField* m_VectorField;
};

//=================================================================

// Runge-Kutta 4th order solver with "quality control" variable step size.
class CSPLIB_EXPORT RungeKutta: public NumericalMethod {
public:
	RungeKutta(double epsilon = 1.e-3, double Hmin = 0.0, double Hestimate = 1.e-2):
		NumericalMethod(epsilon),
		m_hmin(Hmin),
		m_hestimate(Hestimate) { }

	virtual std::string getName() const;
	virtual bool enhancedSolve(Vectord const &y0, Vectord &y, double t0, double dt);
	virtual void quickSolve(Vectord const &y0, Vectord &y, double t0, double dt);

private:
	virtual void redimension();
	void rk4(Vectord const &y0, Vectord const &dyx, double x, double h, Vectord &y);
	bool rkqc(Vectord const &y0, Vectord const &dydx, Vectord &y, double &x, double htry, double eps, Vectord const &yscal, double &hdid, double &hnext);
	bool odeint(Vectord const &y0, Vectord &y, double x1, double x2, double eps, double h1, double hmin, unsigned int &nok, unsigned int &nbad);

	static double const PGROW ;
	static double const PSHRNK;
	static double const FCOR;

	static double const SAFETY;
	static double const ERRCON;

	static double const TINY;

	double m_hmin;
	double m_hestimate;
	Vectord m_rk4_yt;
	Vectord m_rk4_dyt;
	Vectord m_rk4_dym;
	Vectord m_rkqc_ytemp;
	Vectord m_rkqc_y1;
	Vectord m_rkqc_dy;
	Vectord m_odeint_dydx;
	Vectord m_odeint_yscal;
};

//=================================================================

/// Runge-Kutta Cash-Karp 5th order, variable step size,
/// adapted from RKFNC in:
/// J. R. Cash, A. H. Karp, A variable order Runge-Kutta method for initial value
/// problems with rapidly varying right-hand sides, ACM Transactions on Mathematical
/// Software, Vol. 16, No. 3, Sept 1990, pp. 201-222.
/// The authors conclude that RKFNC performs significantly better on DETEST than
/// RKF45.
class CSPLIB_EXPORT RungeKuttaCK: public NumericalMethod {
public:
	RungeKuttaCK(double epsilon = 1.e-3, double Hmin = std::numeric_limits<float>::epsilon(), double Hestimate = 1.e-2):
		NumericalMethod(epsilon),
		m_hmin(Hmin),
		m_hestimate(Hestimate) { }

	virtual std::string getName() const;
	virtual bool enhancedSolve(Vectord const &y0, Vectord &y, double t0, double dt);
	virtual void quickSolve(Vectord const &y0, Vectord &y, double t0, double dt);

private:
	virtual void redimension();
	void rkck(Vectord const &y0, Vectord const &dyx, double x, double h, Vectord &y, Vectord *yerr = 0);
	bool rkqs(Vectord &y, Vectord &dydx, double &x, double htry, double eps, Vectord const &yscal, double &hdid, double &hnext);
	bool odeint(Vectord const &y0, Vectord &y, double x1, double x2, double eps, double h1, double hmin, unsigned int &nok, unsigned int &nbad);

	static double const PGROW ;
	static double const PSHRNK;

	static double const SAFETY;
	static double const ERRCON;

	static double const TINY;
	double m_hmin, m_hestimate;
	Vectord m_rkqs_yerr;
	Vectord m_rkqs_ytemp;
	Vectord m_rkck_ak2;
	Vectord m_rkck_ak3;
	Vectord m_rkck_ak4;
	Vectord m_rkck_ak5;
	Vectord m_rkck_ak6;
	Vectord m_odeint_yscal;
	Vectord m_odeint_dydx;
};

//=================================================================

/// Runge-Kutta Cash-Karp embedded methods 1-5 variable order, variable step size,
/// adapted from VRKF:
/// J. R. Cash, A. H. Karp, A variable order Runge-Kutta method for initial value
/// problems with rapidly varying right-hand sides, ACM Transactions on Mathematical
/// Software, Vol. 16, No. 3, Sept 1990, pp. 201-222.
/// VRKF tends to perform better than RKFNC (RungeKuttaCK) at low precision when the
/// derivative evaluation function is expensive.
class CSPLIB_EXPORT RKCK_VS_VO: public NumericalMethod {
public:
	RKCK_VS_VO(double epsilon = 1.e-3, double Hmin = std::numeric_limits<float>::epsilon(), double Hestimate = 1.e-2);

	virtual std::string getName() const;
	virtual bool enhancedSolve(Vectord const &y0, Vectord &y, double t0, double dt);
	/// @warning dt must be positive
	virtual void quickSolve(Vectord const &y0, Vectord &y, double t0, double dt);

private:
	virtual void redimension();
	virtual void setPrecision(double precision);

	/// Evaluate the second order solution and approximate error at a+h.
	void rkck12(double a, double h, Vectord const &y0, double* err1);

	/// Evaluate the third order solution and approximate error at a+h.
	/// @warning rkck12 must be called first with the same input parameters.
	void rkck23(double a, double h, Vectord const &y0, double* err2);

	/// Evaluate the fifth order solution and approximate error at a+h.
	/// @warning rkck23 must be called first with the same input parameters.
	void rkck45(double a, double h, Vectord const &y0, double* err4);

	/// @param hdone is the step size which has been done, in respect to error criteria
	/// @param hnext is a prevision of the step size for next integration in [a,a+h0]
	/// @return to vrkfBound the solution at a+h
	bool vrkf(Vectord const &y0, Vectord &y, double a, double h, double &hdone, double &hnext);

	/// @param y0
	/// @param a are initial conditions (Cauchy conditions (y0,a))
	/// @param h1 is an estimate step size for current integration.
	/// @param nok, nbad informs about the robustness of the result; not used yet.
	/// @return the solution at a + h
	bool vrkfBound(Vectord const &y0, Vectord &y, double a, double h, double h1, unsigned int &nok, unsigned int &nbad);

	static double const SF;              ///< safety factor
	static double const TINY;            ///< small positive number to assure non nullity of divisors

	/// m_hmin is minimum time step size; if internal step size is below this
	/// value calculations may go out of float precision.  m_hestimate is just
	/// an estimation of the step size to start integration (it is
	/// automatically adjusted if the desired precision is not reached)
	double m_hmin, m_hestimate;
	double m_twiddle[3];
	double m_quit[3];
	double m_inv_eps2;
	double m_inv_eps3;
	double m_inv_eps5;
	Vectord m_k1, m_k2, m_k3, m_k4, m_k5, m_k6;
	Vectord m_ytemp;
	Vectord m_y2;
	Vectord m_y5;
	Vectord m_e35;
};

} // namespace numeric

} // namespace csp

#endif // __CSPLIB_NUMERIC_NUMERICALMETHOD_H__

