%module Object
%{
#include "SimData/Object.h"
%}

%include "std_string.i"
%include "SimData/ns-simdata.h"
%include "SimData/Export.h"

%feature("polymorphic") SIMDATA(Object);

%rename(__repr__) SIMDATA(Object)::asString() const;

NAMESPACE_SIMDATA
	%ignore Referenced;
	class Referenced {};
NAMESPACE_END

%include "SimData/Object.h"

