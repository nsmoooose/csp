%module cCSP 
%{
#include <SimData/HashUtility.h>
#include <SimData/Exception.h>
#include <SimData/Types.h>
#include <SimData/DataArchive.h>
#include <SimData/InterfaceRegistry.h>
#include "CSPSim.h"
%}

%include exception.i
%include std_string.i
%include std_vector.i


%import "SimData/cSimData.i"


%include "VirtualBattlefield.i"
%include "VirtualScene.i"
%include "CSPSim.i"
//%include "SimpleConfig.i"
%include "Config.i"

/*
%include "SimObject.i"
%include "SimTankObject.i"
%include "AeroDynamics.i"
%include "TerrainObject.i"
*/
