%module Spread
%{
#include "SimData/Spread.h"
%}

%include "std_string.i"
//%import "SimData/Pack.i"

%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Spread)::asString() const;

%include "SimData/Spread.h"

