%typemap(python, in) FILE* {
	if (!PyFile_Check($input)) {
		FILE* fp;
    		if ((SWIG_ConvertPtr($input,(void **) &fp, SWIGTYPE_p_FILE,SWIG_POINTER_EXCEPTION | 0 )) == -1) {
			PyErr_SetString(PyExc_TypeError, "Need a file!");
			return NULL;
		}
		$1 = fp;
        } else $1 = PyFile_AsFile($input);
}


/*
%typemap(python,in) FILE * {
	if (!PyFile_Check($input)) {
		PyErr_SetString(PyExc_TypeError, "Need a file!");
		return NULL;
	}
	$1 = PyFile_AsFile($input);
}
*/
