/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
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

%module csplib

%{
#include <csp/lib/data/DataArchive.h>
#include <csp/lib/data/DataManager.h>
#include <csp/lib/data/InterfaceRegistry.h>
#include <csp/lib/data/Types.h>
#include <csp/lib/util/Version.h>
#include <csp/lib/util/HashUtility.h>
#include <csp/lib/util/Exception.h>

class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};

PyObject *cspPyException;
%}

%init %{
	cspPyException = PyErr_NewException("_csp.libexc", NULL, NULL);
	if (cspPyException) PyDict_SetItemString(d, "libexc", cspPyException);
%}

// standard includes
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"

%include "csp/lib/util/Export.h"
%include "csp/lib/util/Namespace.h"
%include "csp/lib/util/swig/Log.i"
%include "csp/lib/util/Uniform.h"
%include "csp/lib/util/Version.h"

// not currently used (will be directors).
//%feature("polymorphic") CSP(InterfaceProxy);
//%feature("polymorphic") CSP(Object);
//%feature("polymorphic:except") {
//    if ($error != NULL) {
//        printf("got a python exception\n");
//        throw CSP(PythonException)();
//    }
//}

%include "csp/lib/util/swig/Testing.i"
%include "csp/lib/util/swig/Exception.i"
%include "csp/lib/util/swig/HashUtility.i"
%include "csp/lib/util/swig/Conversions.i"
%include "csp/lib/util/swig/Random.i"
%include "csp/lib/util/swig/Noise.i"
%include "csp/lib/data/swig/Types.i"

%import "csp/lib/swig/vector.i"
new_vector(double, double);
new_vector(string, std::string);
new_vector(int, int);
new_vector(Real, CSP(Real));
new_vector(Path, CSP(Path));
new_vector(Key, CSP(Key));


%{
PyObject *csp_swig_exception(CSP(ExceptionBase) &e) {
	CSP(ExceptionBase) *pyexc = new CSP(ExceptionBase)(e);
	e.clear();
	return SWIG_NewPointerObj((void*)(pyexc), SWIGTYPE_p_csp__ExceptionBase, 1);
}
%}

%exception {
	try {
		$action
	} catch (CSP(PythonException) &e) {
		printf("SWIG: passing Python exception back\n");
		e.details();
		return NULL;
	} catch (CSP(Exception) &e) {
		//printf("SWIG: caught a SimData Exception\n");
		//PyErr_SetObject(PyExc_RuntimeError, simdata_swig_exception(e));
		PyErr_SetObject(cspPyException, csp_swig_exception(e));
		////e.details();
		//PyErr_SetString(PyExc_RuntimeError, e.getError().c_str());
		return NULL;
	} catch (...) {
		printf("SWIG: passing C++ exception back\n");
		PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
		return NULL;
	}
}

%include "csp/lib/data/swig/DataArchive.i"
%include "csp/lib/data/swig/DataManager.i"
%include "csp/lib/data/swig/InterfaceRegistry.i"
%exception;

class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};

