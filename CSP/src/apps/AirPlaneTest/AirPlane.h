#ifndef _AIRPLANE_H__
#define _AIRPLANE_H__

/** Airplane
 *  This structure encapsulates the variables and parameters
 *  to simualate the motion of an airplane.
 *  These terms orginate from the paper "NPSNET: Flight Simulation
 *  Dynamic modeling using Quaternions" by Joseph M. Cooke, Michael
 *  J. Zyda, David R. Pratt and Robert B. McGhee.
 * 
 *  The terms are divided into World coordinates and Body Coordinates.
 * 
 */


struct AirPlane
{
	AirPlane();
	void assignCoefficients();
	void calcForces();


	/* world coordinate variables */
	double Xw,Yw,Zw;   /* center of mass positions */
	double Uw,Vw,Ww;   /* center of mass velocities */
	double azimuth, elevation, roll;    /* center of mass angles */

	/* body coordinate variables */
	double U,V,W;   /* velocities with respect to body axis */
    double P,Q,R;      /* Angular velocities with respect to body axis */
	double Vt;         /* Resulant velocity */
	double Vc;         /* Wind velocity */
	double dU, dV, dW; /* linear acceleration */
	double dP, dQ, dR; /* angular acceleration */
	double Fx, Fy, Fz; /* forces acting on aircraft */
	double L, M, N;    /* Moments about the X, Y and Z axis */
	double atk_ang;    /* angle of attack */
	double slip_ang;   /* sideslip angle */
	double elevator;   /* elevator deflection position */
	double aileron;    /* aileron deflection position */
	double rudder;     /* rudder deflection position */
	double Fthrust;

	double SArea;      /* surface area of wing */
	double WSpan;      /* wing span */
	double chord;      /* coord length */
	double mass;     /* weight */
	double Ixx;        /* roll inertia */
	double Iyy;        /* pitch inertia */
	double Izz;        /* yaw inertia */
	double Ixz;

	/* longitudinal coefficients */
	double Cl0;        /* reference lift at zero angle of attack */
	double Cd0;        /* reference drag at zero angle of attack */
	double Cla;        /* lift curve slop */
	double Cda;        /* drag curve slop */
	double Cm0;        /* pitch moment */
	double Cma;        /* pitch moment due to angle of attack */
	double Clq;        /* lift due to pitch rate */
	double Cmq;        /* pitch moment due to pitch rate */
	double Clat;        /* lift due to angle of attack rate */
	double Cmat;        /* pitch moment due to angle of attack rate */

	/* lateral coefficients */
	double Cyb;        /* side force due to sideslip */
	double Clb;        /* dihedral effect */
	double Clp;        /* roll damping */
	double Clr;        /* roll due to yaw rate */
	double Cnb;        /* weather cocking stability */
	double Cnp;        /* rudder adverse yaw */
	double Cnr;        /* yaw damping */

	/* control coefficients */
	double Clae;       /* lift due to elevator */
	double Cdda;       /* drag due to elevator */
	double Cmde;       /* pitch due to elevator */
	double Clda;       /* roll due to aileron */

	double g;

};


#endif