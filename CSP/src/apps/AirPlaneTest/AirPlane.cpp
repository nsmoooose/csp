
#include <math.h>
#include "AirPlane.h"

AirPlane::AirPlane()
{
	/* initialize all variable, parameters and coefficients */
	/* world coordinate variables */
	pos_world = csp_vector3(0.0, 0.0, 0.0);
	velocity_world = csp_vector3(0.0, 0.0, 0.0);
	azimuth = 0.0;
	elevator = 0.0;
	roll = 0.0;

	/* body coordinate variables */
	velocity_body = csp_vector3(0.0, 0.0, 0.0);
        ang_velocity_body = csp_vector3(0.0, 0.0, 0.0);
	Vt = 0.0;         
	Vc = 0.0;
	linear_accel = csp_vector3(0.0, 0.0, 0.0);
	ang_accel = csp_vector3(0.0, 0.0, 0.0);
	force_total = csp_vector3(0.0, 0.0, 0.0);
	moment_total = csp_vector3(0.0, 0.0, 0.0);
	atk_ang = 0.0; 
	slip_ang = 0.0;
	elevator = 0.0; 
	aileron = 0.0; 
	rudder = 0.0; 
	Fthrust = 0.0;

	SArea = 0.0; 
	WSpan = 0.0; 
	chord = 0.0; 
	mass = 0.0; 
	Ixx = 0.0; 
	Iyy = 0.0; 
	Izz = 0.0;
	Ixz = 0.0;

	/* longitudinal coefficients */
	Cl0 = 0.0; 
	Cd0 = 0.0;
	Cla = 0.0;
	Cda = 0.0;
	Cm0 = 0.0;
	Cma = 0.0;
	Clq = 0.0; 
	Cmq = 0.0;
	Clat = 0.0; 
	Cmat = 0.0;

	/* lateral coefficients */
	Cyb = 0.0;
	Clb = 0.0;
	Clp = 0.0;
	Clr = 0.0;
	Cnb = 0.0;
	Cnp = 0.0;
	Cnr = 0.0; 

	/* control coefficients */
	Clae = 0.0;
	Cdda = 0.0;
	Cmde = 0.0;
	Clda = 0.0;


}

void AirPlane::assignCoefficients()
{
	SArea = 260.0;
	WSpan = 27.5;
	chord = 10.8;
	mass = 10000.0;
	g = 32.0;

	Ixx = 8090.0;
	Iyy = 25900.0;
	Izz = 29200.0;
	Ixz = 5000.0;

	Cl0 = 0.28; 
	Cd0 = 0.03;
	Cla = 3.45;
	Cda = 0.3;
	Cm0 = 0.0;
	Cma = -0.38;
	Clq = 0.0; 
	Cmq = -3.6;
	Clat = 0.0; 
	Cmat = 0.0;

	Cyb = -0.98;
	Clb = -0.12;
	Clp = -0.26;
	Clr = 0.14;
	Cnb = 0.25;
	Cnp = 0.022;
	Cnr = -0.35; 



	Clae = 0.0;
	Cdda = 0.0;
	Cmde = 0.0;
	Clda = 0.08;


}

void AirPlane::calcForces()
{
  csp_vector3 force_aero;
  csp_vector3 force_thrust;
  csp_vector3 moment_aero;

	Vt = velocity_body.length();
        atk_ang = atan(W/U);
	double lift = (5.0+1.0*atk_ang+1.0*Q/Vt+1.0*elevator)*Vt*Vt;
	double drag = (4.5+1.0*atk_ang+1.0*elevator)*Vt*Vt;

	force_aero.z = -lift*cos(atk_ang)-drag*sin(atk_ang);
	force_aero.x = lift*sin(atk_ang)-drag*cos(atk_ang);

	force_thrust.x = Fthrust;


	moment_aero.x = (1.0*P/Vt + 1.0*R/Vt + 1.0*aileron)*Vt*Vt;
	moment_aero.y = (1.0*atk_ang + 1.0*Q/Vt + 1.0*atk_ang/Vt + 1.0*elevator)*Vt*Vt;
	moment_aero.z = (1.0*P/Vt + 1.0*R/Vt + 1.0*aileron)*Vt*Vt;

	force_total = force_aero + force_thrust;

	moment_total = moment_aero;


	dU = V*R - W*Q + g*sin(elevation) + Fx/mass;
	dV = W*P - U*R + g*sin(roll)*cos(elevation)+Fy/mass;
	dW = U*Q - V*P + g*cos(roll)*cos(elevation)+Fz/mass;

	double LL = L+Ixz*P*Q - (Izz-Iyy)*R*Q;
	double NN = N - (Iyy-Ixx)*P*Q-Ixz*R*Q;

	dP = (LL*Izz-NN*Ixz)/(Ixx*Izz-Ixz*Ixz);
	dQ = (M-(Ixx-Izz)*P*R-Ixz*(P*P-R*R))/Iyy;
	dR = (NN*Ixx+LL*Ixz)/(Ixx*Izz-Ixz*Ixz);
}
