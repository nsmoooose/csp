%include "SimData/Export.h"
%include "SimData/ns-simdata.h"
%include "SimData/BaseType.i"

%exception {
        try {
		$action
        } catch (SIMDATA(PythonException) &e) {
		printf("passing it back\n");
		return NULL;
        }
}
%include "SimData/Object.i"
%include "SimData/Pack.i"
%include "SimData/Path.i"
%include "SimData/Date.i"

%exception;

%include "SimData/Vector3.i"
%include "SimData/GeoPos.i"
%include "SimData/External.i"
%include "SimData/Real.i"
%include "SimData/String.i"
%include "SimData/Enum.i"
%include "SimData/Interpolate.i"
%include "SimData/List.i"
%include "SimData/Matrix3.i"

