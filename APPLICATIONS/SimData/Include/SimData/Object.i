%module Object
%{
#include "SimData/Object.h"
%}

%include "std_string.i"
%include "SimData/ns-simdata.h"

%feature("polymorphic") SIMDATA(Object);

%rename(__repr__) SIMDATA(Object)::asString() const;

%include "SimData/Object.h"
