%module InterfaceRegistry
%{
#include <SimData/InterfaceRegistry.h>
#include <SimData/ns-simdata.h>
using SIMDATA(TypeAdapter);
%}

%include "SimData/ns-simdata.h"
%include "exception.i"

NAMESPACE_SIMDATA
%newobject InterfaceProxy::createObject;
NAMESPACE_END

%exception {
        try {
		$action
        } catch (SIMDATA(TypeMismatch) &e) {
		e.clear();
		SWIG_exception(SWIG_TypeError, e.getMessage().c_str());
        } catch (SIMDATA(InterfaceError) &e) {
		e.clear();
		SWIG_exception(SWIG_SyntaxError, e.getMessage().c_str());
        }
}

%include "SimData/InterfaceRegistry.h"

%exception;

