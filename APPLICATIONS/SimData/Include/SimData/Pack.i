%module Pack
%{
#include "SimData/Pack.h"
%}

%include "std_string.i"
%include "SimData/filemap.i"

%rename(unpack_packable) SIMDATA(UnPacker)::unpack(BaseType&);

%ignore SIMDATA(Packer)::pack(hasht);
%ignore SIMDATA(Packer)::pack(std::string&);
%ignore SIMDATA(Packer)::pack(float);
%ignore SIMDATA(Packer)::pack(const std::string&);

%ignore SIMDATA(UnPacker)::unpack(hasht &);
%ignore SIMDATA(UnPacker)::unpack(std::string&);
%ignore SIMDATA(UnPacker)::unpack(float &);

%typemap(python, in) (const char* data, int n) {
	$1 = PyString_AsString($input);
	$2 = PyString_Size($input);
}

%include "SimData/Pack.h"

%exception;
