%module dem2dat 
%{
	#include "dem2dat.h"
	#include <stdexcept>
	#include <SimData/Link.h>  // XXX temporory hack
%}

%include "std_vector.i"
%include "std_string.i"

namespace std {
	%template(vectord) vector<double>;
	%template(vectorElevationRow) vector<ElevationRow>;
}

%import "SimData/cSimData.i"

%include "dem2dat.h"
