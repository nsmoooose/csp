// Combat Simulator Project - CSPSim
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

%module cCSP 
%{
#include <SimData/HashUtility.h>
#include <SimData/ExceptionBase.h>
#include <SimData/Types.h>
#include <SimData/DataArchive.h>
#include <SimData/InterfaceRegistry.h>
#include "CSPSim.h"
%}

%include exception.i
%include std_string.i
%include std_vector.i


//%import "SimData/cSimData.i"


//%include "Battlefield.i"
%include "VirtualScene.i"
%include "SimNet/Networking.i"
%include "CSPSim.i"
//%include "SimpleConfig.i"
%include "Config.i"
%include "Log.i"
%include "ClientNode.i"
%include "EchoServerNode.i"
%include "RedirectServerNode.i"

/*
%include "SimObject.i"
%include "SimTankObject.i"
%include "AeroDynamics.i"
%include "TerrainObject.i"
*/

