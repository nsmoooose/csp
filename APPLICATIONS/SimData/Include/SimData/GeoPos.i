%module GeoPos
%{
#include "SimData/Vector3.h"
#include "SimData/GeoPos.h"
%}

%include "std_string.i"

NAMESPACE_SIMDATA
%rename(__repr__) GeoPos::asString() const;
%rename(__repr__) LLA::asString() const;
%rename(__repr__) UTM::asString() const;
%rename(__repr__) ECEF::asString() const;
%ignore GeoPos::parseXML(const char*);
%ignore GeoPos::operator=(Vector3 const &);
%ignore GeoPos::operator=(GeoPos const &);
%ignore UTM::operator=(LLA const&);
%ignore UTM::operator=(ECEF const&);
%ignore LLA::operator=(UTM const&);
%ignore LLA::operator=(ECEF const&);
%ignore ECEF::operator=(UTM const&);
%ignore ECEF::operator=(LLA const&);
NAMESPACE_END

%include "SimData/GeoPos.h"

