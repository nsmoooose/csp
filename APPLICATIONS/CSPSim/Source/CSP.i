%module CSP
%{
//#include <SimData/Types.h>
#include "CSPSim.h"
%}

%include exception.i
%include std_string.i
%include std_vector.i

/*
%import "SimData/cSimData.i"
*/

%include "CSPSim.i"
%include "SimpleConfig.i"
%include "Config.i"

/*
%include "SimObject.i"
%include "SimTankObject.i"
%include "AeroDynamics.i"
%include "TerrainObject.i"
*/
