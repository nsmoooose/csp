%module Interpolate
%{
#include "SimData/Interpolate.h"
%}

%include "std_vector.i"
#ifndef VFLOAT
#define VFLOAT
namespace std {
	%template(vector_t) vector<float>;
}
#endif
using namespace std;
typedef std::vector vector;

%include "SimData/filemap.i"
%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Table)::asString() const;
%rename(__repr__) SIMDATA(Curve)::asString() const;

%include "SimData/Interpolate.h"
