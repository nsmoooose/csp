%module List
%{
#include "SimData/List.h"
#include "SimData/Real.h"
#include "SimData/Vector3.h"
#include "SimData/Path.h"
#include "SimData/Interpolate.h"
#include "SimData/External.h"
#include "SimData/Key.h"
%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/ns-simdata.h"

%rename (__repr__) SIMDATA(List)::asString() const;

%feature("polymorphic") SIMDATA(ListBase);

%include "SimData/List.h"

//%import "SimData/Vector3.h"

#define new_list(a, b)	\
	%template(v##a) std::vector< b >; \
	%template(l##a) SIMDATA(List)< b >;


//#ifdef __LIST_TEMPLATES__
//#define __LIST_TEMPLATES__
new_list(Int, int);
new_list(Real, SIMDATA(Real));
//new_list(Vector3, SIMDATA(Vector3));
new_list(Path, SIMDATA(Path));

// HACK!
#ifndef VSTRING
new_list(String, std::string);
#define VSTRING
#endif

new_list(Curve, SIMDATA(Curve));
new_list(Table, SIMDATA(Table));
new_list(External, SIMDATA(External));
new_list(Key, SIMDATA(Key));
//#endif // __LIST_TEMPLATES__

