// Combat Simulator Project - CSPSim
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
 * @file NumericalMethod.h
 *
 **/

#ifndef __NUMERICALMETHOD_H__
#define __NUMERICALMETHOD_H__

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "Vector.h"

#define USE_VALARRAY_ARITHMETIC

class VectorField;


/**
* Interesting readings about numerical analysis of ODE can be found in:
* M. Crouzeix, A.L. Mignot, Analyse numerique des equations differentielles, Masson, 1984.
* R.L. Burden, J.D. Faires, Numerical analysis, Brooks/Cole, 2001.
*/ 

/**
 * Abstract base class for numerical solvers of dynamical systems.
 */
class NumericalMethod {
protected:
	/**
	* State of a numerical method
	*/
	class State {
		std::ostringstream m_Message;
		bool m_Failed;
		void reset() {
			m_Message.clear();
			m_Message.str("");
			m_Message << "Success\n";
		}
	public:
		State(): 
		m_Message("Success\n"), 
		m_Failed(false) {
		}
		~State(){}
		void setFailed(bool failed) {
			if (failed) 
				m_Message.str("Fail\n");
			else
				reset();
			m_Failed = failed;
		}
		bool getFailed() const {
			return m_Failed;
		}
		std::ostringstream& operator<<(std::string const &rhs) {
			m_Message << rhs;
			m_Failed |= true;
			return m_Message;
		}
		State &operator=(State const &rhs) {
			m_Message.str(rhs.m_Message.str());
			m_Failed = rhs.getFailed();
			return *this;
		}
		void print() const {
			std::cerr << m_Message.str();
		}
	};
	VectorField* m_VectorField;
	State m_State;
	size_t m_Dimension;
	std::string m_Name;
	double m_Epsilon;
public:
	NumericalMethod();
	NumericalMethod(VectorField *pvf);

	/**
	* set the vector field f to solve (y' = f(t,y))
	* @warning this may need customized treatment
	*/
	virtual void setVectorField(VectorField *pvf);
	void setPrecision(double precision);
	virtual ~NumericalMethod();

	/**
	* enhancedSolve may be a sophisticated method (manage a variable step size, variable order, multistep,
	* be implicit, ...). 
	* @warning It is called intensively so it may well be a bottleneck. 
	* @param y0 initial position at  ...
	* @param t = t0 (Cauchy conditions)
	* @param dt delta t
	* @return the position of the system at t = t0 + dt (returns y0 if failed)
	*/
	virtual Vector::Vectord const &enhancedSolve(Vector::Vectord &y0, double t0, double dt) = 0;

	/**
	* In case of that enhancedSolve fails, quickSolve acts as a security to waranty an integration. In
	* general, it should be relatively fast and eventually less accurate but ideally more stable (implicit 
	* method may be required to assure more stability. It is automatically called by DynamicalSystem::flow 
	* if needed.
	* @param see enhancedSolve
	*/
	virtual Vector::Vectord const &quickSolve(Vector::Vectord &y0, double t0, double dt) = 0; 

	/**
	 * @return the name of the numerical method.
	 */
	std::string const &getName() const;

	/**
	* print the state of the numerical method (success/fail) and other debugging msgs
	*/
	void printState() const;

	/**
	* @return true if the enhanced solver has failed.
	*/
	bool hasFailed() const;
};

//=================================================================

class RungeKutta: public NumericalMethod {
	Vector::Vectord const &rk4(Vector::Vectord const &y, Vector::Vectord const &dyx, double x, double h) const;
	Vector::Vectord const &rkqc(Vector::Vectord &y, Vector::Vectord &dydx, double &x, double htry, double eps, 
		                            Vector::Vectord const &yscal, double &hdid, double &hnext);
	Vector::Vectord const &odeint(Vector::Vectord const &ystart, double x1, double x2, double eps, double h1, 
									  double hmin, unsigned int &nok, unsigned int &nbad);
	
	static double const PGROW ;
	static double const PSHRNK;
	static double const FCOR;
	
	static double const SAFETY;
	static double const ERRCON;
	
	static unsigned int const MAXSTP;
    static double const TINY;
	double m_hmin, m_hestimate;
public:
	Vector::Vectord const &quickSolve(Vector::Vectord &y0, double t0, double dt); 
	Vector::Vectord const &enhancedSolve(Vector::Vectord &y0, double t0, double dt);
	RungeKutta(VectorField *vectorField = 0, double epsilon = 1.e-3, double Hmin = 0.0, double Hestimate = 1.e-2):
	    NumericalMethod(vectorField),
		m_hmin(Hmin),
		m_hestimate(Hestimate){
			m_Epsilon = epsilon;
			m_Name = "Runge-Kutta order 4 with variable step size";
	}
};

//=================================================================

class RungeKuttaCK: public NumericalMethod {
	Vector::Vectord const &rkck(Vector::Vectord const &y, Vector::Vectord const &dyx, double x, double h, 
		                            Vector::Vectord &yerr) const;
	Vector::Vectord const &rkqs(Vector::Vectord &y, Vector::Vectord &dydx, double &x, double htry, double eps, 
		                            Vector::Vectord const &yscal, double &hdid, double &hnext);
	Vector::Vectord const &odeint(Vector::Vectord const &ystart, double x1, double x2, double eps, double h1,
		                              double hmin, unsigned int &nok, unsigned int &nbad);
	
	static double const PGROW ;
	static double const PSHRNK;
	
	static double const SAFETY;
	static double const ERRCON;
	
	static unsigned int const MAXSTP;
    static double const TINY;
	double m_hmin, m_hestimate;
public:
	Vector::Vectord const &quickSolve(Vector::Vectord &y0, double t0, double dt); 
	Vector::Vectord const &enhancedSolve(Vector::Vectord &y0, double t0, double dt);
	RungeKuttaCK(VectorField *vectorField = 0, double epsilon = 1.e-3, 
		         double Hmin = std::numeric_limits<float>::epsilon(), double Hestimate = 1.e-2):
	    NumericalMethod(vectorField),
		m_hmin(Hmin),
		m_hestimate(Hestimate){
			m_Epsilon = epsilon;
			m_Name = "Runge-Kutta Cash-Karp order 5 variable step size";
	}
};

//=================================================================

/**
* Runge-Kutta Cash-Karp embedded methods 1-5 variable order, variable step size, 
* adapted from:
* J. R. Cash, A. H. Karp, A variable order Runge-Kutta method for initial value 
* problems with rapidly varying right-hand sides, ACM Transactions on Mathematical 
* Software, Vol. 16, No. 3, Spet 1990, pp. 201-222.
*/
class RKCK_VS_VO: public NumericalMethod {
	/**
	* Evaluate the approximated soluation at a+h (entire step size)
	* control is defered to vrkf to adjust step size and order.
	* @return the error between RK(n+1) and RK(n)
	* @warning a and h don t design the same as a,h in vrkfBound (because they are variable in vrkf)
	*/
    double rkck12(double a, double h, Vector::Vectord const &ystart);
    double rkck23(double a, double h, Vector::Vectord const &ystart);
    double rkck45(double a, double h, Vector::Vectord const &ystart);

	/**
	* @return to vrkfBound the solution at a+h
	* @param hdone is the step size which has been done, in respect to error criteria
	* @param hnext is a prevision of step size for next integration in [a,a+h0]
	*/
    Vector::Vectord const &vrkf(Vector::Vectord &ystart, double a, double h, double &hdone, double &hnext);

	/**
	* @param ystart
	* @param a are initial conditions (Cauchy conditions (ystart,a))
	* @param h1 is an estimate step size for current integration.
	* @param nok, nbad informs about the robustness of the result; not used yet.
	* @return the solution at a + h
	*/
    Vector::Vectord const &vrkfBound(Vector::Vectord &ystart, double a, double h, double h1, unsigned int &nok, 
                                         unsigned int &nbad);

	/*
	* resize vector members
	*/
	void resize();

    static double const SF;				///< safety factor
    static unsigned int const MAXSTEP;	///< maximum loop in vrkf & vrkfBound
    static double const TINY;			///< small positive number to assure non nullity of divisors

	/**
	* \var
	* m_Epsilon is the desired precision
	* m_Hmin is minimum time step size; if internal step size is below this value calculations may go out of float 
	* precision.
	* m_Hestimate is just an estimation of the step size to start integration (it is automatically
	* adpated if the precision m_Epsilon is not reached)
	* These members have default values
	*/
    double m_Hmin, m_Hestimate;
	std::vector<double> m_Twiddle, m_Quit;
    Vector::Vectord m_k1, m_k2, m_k3, m_k4;
	Vector::Vectord m_ytemp, m_y2, m_y3, m_y4, m_y5, m_Result;
public:
	RKCK_VS_VO(VectorField *vectorField = 0, double epsilon = 1.e-3, double Hmin = std::numeric_limits<float>::epsilon(), 
		       double Hestimate = 1.e-2);
	virtual void setVectorField(VectorField *pvf);

	/**
	* @warning dt must be positive
	*/
    virtual Vector::Vectord const &quickSolve(Vector::Vectord &y0, double t0, double dt); 
    virtual Vector::Vectord const &enhancedSolve(Vector::Vectord &y0, double t0, double dt);
};

#endif // __NUMERICALMETHOD_H__


