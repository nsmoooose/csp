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
	void setOutput(std::string const &fn);
	void setLogLevels(int c, int p);
	void setLogClasses(int c);
};

NAMESPACE_END // namespace simdata

%include "SimData/Log.h"
