%module Exception
%{
#include "SimData/Exception.h"
%}

// suppress warning about undefined base class std::runtime_exception
// not sure how to fix this cleanly, since std::runtime_exception has
// a protected default constructor that causes the wrapper code to
// break if swig knows about the class.
%warnfilter(401) Exception;

%include "SimData/Exception.h"


