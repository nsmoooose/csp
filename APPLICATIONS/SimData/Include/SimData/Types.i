/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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

%include "SimData/Export.h"
%include "SimData/Namespace.h"
%include "SimData/BaseType.i"

%exception {
        try {
		$action
        } catch (SIMDATA(PythonException) &e) {
		e.details();
		printf("passing it back\n");
		return NULL;
        }
}
%include "SimData/Object.i"
%include "SimData/Pack.i"
//%include "SimData/Archive.i"
%include "SimData/Path.i"
%include "SimData/Link.i"
%include "SimData/Date.i"

%exception;

%include "SimData/List.i"
%include "SimData/Vector3.i"
%include "SimData/GeoPos.i"
%include "SimData/External.i"
%include "SimData/Key.i"
%include "SimData/Real.i"
%include "SimData/String.i"
%include "SimData/Enum.i"
%include "SimData/Interpolate.i"
%include "SimData/LUT.i"
%include "SimData/Matrix3.i"
%include "SimData/Quat.i"

