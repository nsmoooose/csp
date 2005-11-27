/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

%{
#include <csp/csplib/data/TypeAdapter.h>
%}

%include "csp/csplib/util/Export.h"
%include "csp/csplib/util/Namespace.h"
%include "csp/csplib/data/BaseType.h"
%include "csp/csplib/data/TypeAdapter.h"

%exception {
	try {
		$action
	} catch (CSP(PythonException) &e) {
		e.details();
		printf("passing it back\n");
		return NULL;
	}
}
%include "csp/csplib/data/swig/Object.i"
%include "csp/csplib/data/swig/Archive.i"
%include "csp/csplib/data/swig/Path.i"
%include "csp/csplib/data/swig/Link.i"
%include "csp/csplib/data/swig/Date.i"

%exception;

%include "csp/csplib/data/swig/Vector3.i"
%include "csp/csplib/data/swig/GeoPos.i"
%include "csp/csplib/data/swig/Key.i"
%include "csp/csplib/data/swig/Real.i"
%include "csp/csplib/data/swig/Enum.i"
%include "csp/csplib/data/swig/LUT.i"
%include "csp/csplib/data/swig/Matrix3.i"
%include "csp/csplib/data/swig/Quat.i"

