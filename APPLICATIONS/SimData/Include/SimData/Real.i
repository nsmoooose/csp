%module Real
%{
#include "SimData/Real.h"
%}

%include "std_string.i"
//%import "SimData/Pack.i"

%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Real)::asString() const;

%include "SimData/Real.h"

