%module DataManager
%{
#include <SimData/Object.h>
#include <SimData/DataManager.h>
%}


%import "SimData/Exception.i"

//typedef  unsigned long long  hasht;   /* unsigned 8-byte type */
//typedef  int		     int32;

%include exception.i
%exception SIMDATA(DataManager) {
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
%import "SimData/DataArchive.i"

%include "SimData/DataManager.h"

%exception;

