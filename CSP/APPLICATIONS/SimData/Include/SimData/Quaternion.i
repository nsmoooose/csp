%module Quaternion
%{
#include "SimData/Quaternion.h"
%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Quaternion)::asString() const;

%include "SimData/Quaternion.h"

