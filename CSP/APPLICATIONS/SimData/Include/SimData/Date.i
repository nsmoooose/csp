%module Date
%{
#include "SimData/Date.h"
%}

%include "std_string.i"
//%import "SimData/Pack.i"

NAMESPACE_SIMDATA
%rename(__repr__) Date::asString() const;
%rename(__repr__) Zulu::asString() const;
%rename(__repr__) DateZulu::asString() const;
%rename(__repr__) SimDate::asString() const;
NAMESPACE_END

%include "SimData/Date.h"

