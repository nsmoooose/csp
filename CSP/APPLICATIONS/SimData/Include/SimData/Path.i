%module Path
%{
#include "SimData/Path.h"
%}

%include "std_string.i"
%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Path)::asString() const;

%warnfilter(508, 509) Path;

%include "SimData/Path.h"


