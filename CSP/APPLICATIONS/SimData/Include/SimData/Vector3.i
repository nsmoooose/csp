%module Vector3
%{
#include "SimData/Vector3.h"
#include "SimData/Matrix3.h"
%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Vector3)::asString() const;

%include "SimData/Vector3.h"

