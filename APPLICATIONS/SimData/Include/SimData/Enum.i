%module "Enum"
%{
#include "SimData/Enum.h"
%}

%include "std_string.i"
%include "std_vector.i"


%template(vEnum) std::vector<SIMDATA(Enum)>;

// HACK!
#ifndef VSTRING
%template(vString) std::vector<string>;
#define VSTRING
#endif

//%import "SimData/Pack.i"
%ignore eqstring;
%ignore hashstring;
%ignore string_map;

#define TRY	try { $action }
#define CATCH(a, b) \
	catch (a& e) {\
		SWIG_exception(b, const_cast<char*>(e.getError().c_str()));\
	}
	
%include "exception.i"

%exception  {
	TRY
	CATCH(SIMDATA(EnumTypeError), SWIG_TypeError)
	CATCH(SIMDATA(EnumIndexError), SWIG_IndexError)
	CATCH(SIMDATA(EnumError), SWIG_RuntimeError)
}
/*
%exception Enumeration {
	TRY
	CATCH(EnumTypeError, SWIG_TypeError)
	CATCH(EnumIndexError, SWIG_IndexError)
	CATCH(EnumError, SWIG_RuntimeError)
}
*/		

%include "SimData/ns-simdata.h"

%rename(__repr__) SIMDATA(Enum)::asString() const;
%rename(__repr__) SIMDATA(Enumeration)::asString() const;

%include "SimData/Enum.h"

%exception;

