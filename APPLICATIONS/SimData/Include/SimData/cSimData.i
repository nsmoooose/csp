/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

%module SimData.cSimData

%{
#include <SimData/HashUtility.h>
#include <SimData/Exception.h>
#include <SimData/Types.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Version.h>

%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/Namespace.h"
%include "SimData/Export.h"
%include "SimData/Version.h"

%include "SimData/Log.i"

%feature("polymorphic") SIMDATA(InterfaceProxy);
%feature("polymorphic") SIMDATA(Object);

%feature("polymorphic:except") {
    if ($error != NULL) {
        printf("got a python exception\n");
        throw SIMDATA(PythonException)();
    }
}

%include "SimData/Exception.i"
%include "SimData/HashUtility.i"
%include "SimData/Conversions.i"
%include "SimData/Random.i"
%include "SimData/Noise.i"
%include "SimData/Types.i"

%exception {
        try {
        $action
        } catch (SIMDATA(PythonException) &e) {
		printf("SWIG: passing Python exception back\n");
		e.details();
		return NULL;
        } catch (SIMDATA(Exception) e) {
		printf("SWIG: caught a SimData Exception\n");
		//e.details();
		PyErr_SetString(PyExc_RuntimeError, e.getError().c_str());
		return NULL;
        } catch (...) {
        	printf("SWIG: passing C++ exception back\n");
		PyErr_SetString(PyExc_RuntimeError, "Unknownn C++ exception");
        	return NULL;
    	}
}

%include "SimData/DataArchive.i"
%include "SimData/DataManager.i"
%include "SimData/InterfaceRegistry.i"
%exception;
