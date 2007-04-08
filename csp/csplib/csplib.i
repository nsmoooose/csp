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

%module csplib_module

%{
#include <csp/csplib/data/DataArchive.h>
#include <csp/csplib/data/DataManager.h>
#include <csp/csplib/data/InterfaceRegistry.h>
#include <csp/csplib/data/Types.h>
#include <csp/csplib/util/Version.h>
#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Exception.h>

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

/* For now, suppress warning 124:
 *   Specifying the language name in %typemap is >deprecated - use
 *   #ifdef SWIG<LANG> instead.
 * We'll do this after a few minor version releases of swig (or when
 * support for languages in typmap is removed.
 */
#pragma SWIG nowarn=124

// standard includes
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"

%include "csp/csplib/util/Namespace.h"

// vector templates
%import "csp/csplib/swig/vector.i"
new_vector(double, double);
new_vector(float, float);
new_vector(int, int);
new_vector(string, std::string);
new_vector(Real, CSP(Real));
new_vector(Path, CSP(Path));
new_vector(Key, CSP(Key));

%include "csp/csplib/util/Export.h"
%include "csp/csplib/util/swig/Log.i"
%include "csp/csplib/util/Uniform.h"
%include "csp/csplib/util/Version.h"

// not currently used (will be directors).
//%feature("polymorphic") CSP(InterfaceProxy);
//%feature("polymorphic") CSP(Object);
//%feature("polymorphic:except") {
//    if ($error != NULL) {
//        printf("got a python exception\n");
//        throw CSP(PythonException)();
//    }
//}

%include "csp/csplib/util/swig/Testing.i"
%include "csp/csplib/util/swig/Exception.i"
%include "csp/csplib/util/swig/HashUtility.i"
%include "csp/csplib/util/swig/Conversions.i"
%include "csp/csplib/util/swig/Modules.i"
%include "csp/csplib/util/swig/Random.i"
%include "csp/csplib/util/swig/Noise.i"
%include "csp/csplib/data/swig/Types.i"

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

%include "csp/csplib/data/swig/DataArchive.i"
%include "csp/csplib/data/swig/DataManager.i"
%include "csp/csplib/data/swig/InterfaceRegistry.i"
%exception;

class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};

