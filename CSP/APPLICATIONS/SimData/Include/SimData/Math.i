%module Math
%{
#include "SimData/Matrix3.h"
#include "SimData/Vector3.h"
%}

%include "std_string.i"
%include "std_vector.i"

#define new_vector(a, b)	\
	%template(v##a) std::vector< b >;

namespace std {
	%template(vector_d) vector<double>;
}

//%import "SimData/Pack.i"
//%include "SimData/Matrix3.h"

%include "SimData/Matrix3.i"
%include "SimData/Vector3.i"
