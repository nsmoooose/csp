%module DataArchive
%{
#include <SimData/Object.h>
#include <SimData/DataArchive.h>
%}


%import "SimData/Exception.i"

typedef  unsigned long long  hasht;   /* unsigned 8-byte type */
typedef  int		     int32;

%include exception.i
%exception SIMDATA(DataArchive) {
	try {
		$function
	} catch (SIMDATA(IndexError) e) {
		e.details();
		SWIG_exception(SWIG_IndexError, "Path not found");
	} catch (SIMDATA(ObjectMismatch) e) {
		e.details();
		SWIG_exception(SWIG_TypeError, "Object Mismatch");
	} catch (SIMDATA(BadMagic) e) {
		e.details();
		SWIG_exception(SWIG_IOError, "Bad Magic");
	} catch (SIMDATA(BadByteOrder) e) {
		e.details();
		SWIG_exception(SWIG_IOError, "Incorrect Byte Order");
	}
}


%import "SimData/Object.i"

%typemap(out) FP {
	$result = PyFile_FromFile($1.f, const_cast<char*>($1.name.c_str()), 
					const_cast<char*>($1.mode.c_str()), 0);
}

%include "SimData/DataArchive.h"

%exception;

