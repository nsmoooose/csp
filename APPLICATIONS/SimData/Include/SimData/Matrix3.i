%module Matrix3
%{
#include "SimData/Matrix3.h"
#include "SimData/Vector3.h"
%}

%include "std_string.i"
%include "std_vector.i"


%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Matrix3)::asString() const;

%include "SimData/Matrix3.h"

