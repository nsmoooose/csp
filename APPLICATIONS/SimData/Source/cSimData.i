%module cSimData

%{
#include <SimData/HashUtility.h>
#include <SimData/Exception.h>
#include <SimData/Types.h>
#include <SimData/DataArchive.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/Version.h>

%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/ns-simdata.h"
%include "SimData/Export.h"
%include "SimData/Version.h"

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
%include "SimData/Types.i"

%exception {
        try {
        $action
        } catch (SIMDATA(PythonException) &e) {
        printf("SWIG: passing Python exception back\n");
        return NULL;
        } catch (SIMDATA(Exception) e) {
        printf("SWIG: caught a SimData Exception\n");
        e.details();
        return NULL;
        } catch (...) {
        printf("SWIG: passing C++ exception back\n");
        return NULL;
    }
}

%include "SimData/DataArchive.i"
%include "SimData/InterfaceRegistry.i"
%exception;
