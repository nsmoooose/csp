/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This file is part of SimData.
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
#include <SimData/ExceptionBase.h>
#include <SimData/Types.h>
#include <SimData/DataArchive.h>
#include <SimData/DataManager.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Version.h>


class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};

PyObject *SimDataPyException;
%}

%init %{
	SimDataPyException = PyErr_NewException("_cSimData.libexc", NULL, NULL);
	if (SimDataPyException) PyDict_SetItemString(d, "libexc", SimDataPyException);
%}

// standard includes
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"

%include "SimData/Namespace.h"
%include "SimData/Export.h"
%include "SimData/Version.h"
%include "SimData/Uniform.h"

%include "SimData/Log.i"

// not currently used (will be directors).
%feature("polymorphic") SIMDATA(InterfaceProxy);
%feature("polymorphic") SIMDATA(Object);

%feature("polymorphic:except") {
    if ($error != NULL) {
        printf("got a python exception\n");
        throw SIMDATA(PythonException)();
    }
}

%include "SimData/ExceptionBase.i"
%include "SimData/HashUtility.i"
%include "SimData/Conversions.i"
%include "SimData/Random.i"
%include "SimData/Noise.i"
%include "SimData/Types.i"

%{
PyObject *simdata_swig_exception(SIMDATA(Exception) &e) {
	SIMDATA(ExceptionBase) *pyexc = new SIMDATA(ExceptionBase)(e);
	e.clear();
	return SWIG_NewPointerObj((void*)(pyexc), SWIGTYPE_p_simdata__ExceptionBase, 1);
}
%}

%exception {
	try {
		$action
	} catch (SIMDATA(PythonException) &e) {
		printf("SWIG: passing Python exception back\n");
		e.details();
		return NULL;
	} catch (SIMDATA(Exception) &e) {
		//printf("SWIG: caught a SimData Exception\n");
		//PyErr_SetObject(PyExc_RuntimeError, simdata_swig_exception(e));
		PyErr_SetObject(SimDataPyException, simdata_swig_exception(e));
		////e.details();
		//PyErr_SetString(PyExc_RuntimeError, e.getError().c_str());
		return NULL;
	} catch (...) {
		printf("SWIG: passing C++ exception back\n");
		PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
		return NULL;
	}
}

%include "SimData/DataArchive.i"
%include "SimData/DataManager.i"
%include "SimData/InterfaceRegistry.i"
%exception;

class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};

