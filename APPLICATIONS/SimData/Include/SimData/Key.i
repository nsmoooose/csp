%module Key
%{
#include "SimData/Key.h"
%}

%include "std_string.i"

%rename(__repr__) SIMDATA(Key)::asString() const;
%rename(__eq__) SIMDATA(Key)::operator==(SIMDATA(Key) const &) const;
%rename(__eq__) SIMDATA(Key)::operator==(std::string const &) const;

%include "SimData/Key.h"
