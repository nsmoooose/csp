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

%module(directors="1") cspsim_module

%{
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/data/Types.h>
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/InterfaceRegistry.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/RegisterObjectInterfaces.h>
%}

%include exception.i
%include std_string.i
%include std_vector.i

namespace csp {

%feature("ref")   Referenced "$this->_incref();"
%feature("unref") Referenced "$this->_decref();"

%ignore Referenced;
class Referenced {
};

}

%include <csp/cspsim/Export.h>
%include <csp/cspsim/swig/Config.i>
%include <csp/cspsim/swig/VirtualScene.i>
%include <csp/cspwf/cspwf.i>
%include <csp/cspsim/swig/Screens.i>
%include <csp/cspsim/swig/Projection.i>
%include <csp/cspsim/swig/CSPSim.i>
%include <csp/cspsim/RegisterObjectInterfaces.h>
%include <csp/cspsim/swig/CSPViewer.i>
%include <csp/cspsim/indexserver.i>
