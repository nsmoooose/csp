%module GeoPos
%{
#include "SimData/Vector3.h"
#include "SimData/GeoPos.h"
%}

%include "std_string.i"

NAMESPACE_SIMDATA
%rename(__repr__) GeoPos::asString() const;
%ignore GeoPos::parseXML(const char*);
%ignore GeoPos::operator=(Vector3 const &);
%ignore GeoPos::operator=(GeoPos const &);
NAMESPACE_END

%include "SimData/GeoPos.h"

