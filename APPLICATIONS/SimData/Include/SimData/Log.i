%module Log
%{
#include "SimData/LogStream.h"
#include "SimData/Log.h"
%}

%include "std_string.i"
%include "SimData/ns-simdata.h"

NAMESPACE_SIMDATA

class logstream 
{
public:
	logstream(std::ostream& out);
	~logstream();
	void _close();
	void set_output(std::string const &fn);
	void setLogLevels(int c, int p);
};

NAMESPACE_END // namespace simdata

%include "SimData/Log.h"
