%module BaseType
%{
#include "SimData/BaseType.h"
%}

%include exception.i

%import "SimData/Exception.i"
%include "std_string.i"

%exception BaseType {
	try {
		$action
	} catch (SIMDATA(ParseException) e) {
		e.details();
		SWIG_exception(SWIG_RuntimeError, "");
	}
}

%include "SimData/BaseType.h"

%exception;
