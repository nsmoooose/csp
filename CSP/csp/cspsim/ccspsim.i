// Combat Simulator Project
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

%module ccspsim
%{
#include <csp/lib/util/HashUtility.h>
#include <csp/lib/util/Exception.h>
#include <csp/lib/data/Types.h>
#include <csp/lib/data/DataArchive.h>
#include <csp/lib/data/InterfaceRegistry.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Animation.h>
%}

%include exception.i
%include std_string.i
%include std_vector.i

%include <csp/cspsim/VirtualScene.i>
%include <csp/cspsim/CSPSim.i>
%include <csp/cspsim/Config.i>
