%module Path
%{
#include "SimData/Link.h"
%}

%ignore SIMDATA(PathPointerBase)::operator==(const PathPointerBase&, const PathPointerBase&);
%ignore SIMDATA(PathPointerBase)::operator=(const PathPointerBase&);
%ignore operator==(const PathPointerBase&, const PathPointerBase&);
%ignore operator!=(const PathPointerBase&, const PathPointerBase&);
%ignore SIMDATA(PointerBase)::operator=(void *);
%ignore SIMDATA(PointerBase)::operator=(const PointerBase& r);

%include "std_string.i"
%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(PathPointerBase)::asString() const;
%rename(__not__) SIMDATA(PointerBase)::operator!() const; 

%warnfilter(508, 509) PathPointer;

%include "SimData/Link.h"


#define new_pointer(a)	%template(p##a) SIMDATA(PathPointer)<a>;

