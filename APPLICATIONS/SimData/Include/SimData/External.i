%module External
%{
#include "SimData/External.h"
%}

%include "std_string.i"
//%import "SimData/Pack.i"

%rename(__repr__) SIMDATA(External)::asString() const;

%include "SimData/External.h"
