%module Noise
%{
#include "SimData/Noise.h"
%}

%include "std_vector.i"

// HACK!
#ifndef VFLOAT
#define VFLOAT
namespace std {
	%template(vector_t) vector<float>;
}
#endif

%include "SimData/Noise.h"

