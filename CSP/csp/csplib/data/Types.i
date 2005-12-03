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
#include <csp/lib/data/TypeAdapter.h>
%}

%include "csp/lib/util/Export.h"
%include "csp/lib/util/Namespace.h"
%include "csp/lib/data/BaseType.h"
%include "csp/lib/data/TypeAdapter.h"

%exception {
	try {
		$action
	} catch (CSP(PythonException) &e) {
		e.details();
		printf("passing it back\n");
		return NULL;
	}
}
%include "csp/lib/data/Object.i"
%include "csp/lib/data/Archive.i"
%include "csp/lib/data/Path.i"
%include "csp/lib/data/Link.i"
%include "csp/lib/data/Date.i"

%exception;

%include "csp/lib/data/Vector3.i"
%include "csp/lib/data/GeoPos.i"
%include "csp/lib/data/Key.i"
%include "csp/lib/data/Real.i"
%include "csp/lib/data/Enum.i"
%include "csp/lib/data/LUT.i"
%include "csp/lib/data/Matrix3.i"
%include "csp/lib/data/Quat.i"
