#include <iostream.h>
#include "airplane.h"
#include "mtxlib.h"


void output_header()
{
  cout << "t:      U:     dU:     V:     dV:     W:     dW:      P:     dP:    Q:    dQ:    R:    dR:    Vt:    att:" << endl;

}

void output(csp_scalar t, AirPlane & plane)
{
  cout << t;
  cout << "   " << plane.U << "   " << plane.dU;
  cout << "   " << plane.V << "   " << plane.dV;
  cout << "   " << plane.W << "   " << plane.dW;
  cout << "   " << plane.P << "   " << plane.dP;
  cout << "   " << plane.Q << "   " << plane.dQ;
  cout << "   " << plane.R << "   " << plane.dR;
  cout << "   " << plane.Vt;
  cout << "   " << plane.atk_ang << endl;
}

int main(int argc, char * argv[])
{
	AirPlane plane;
	plane.assignCoefficients();
	plane.U = 100.0;
	plane.V = 0.0;
	plane.W = 0.0;
	plane.Fthrust = 250.0;
	plane.atk_ang = 0.0;
	plane.elevator = 0.0;
	plane.aileron = 0.0;
	double t = 0.0;
	double dt = 0.01;
	output_header();
	for (int i = 0; i<1000;i++)
	{
		output(t,plane);
		plane.calcForces();
		plane.U += dt*plane.dU;
		plane.V += dt*plane.dV;
		plane.W += dt*plane.dW;
		plane.P += dt*plane.dP; 
		plane.Q += dt*plane.dQ;
		plane.R += dt*plane.dR;
		t += dt;
	}

}

