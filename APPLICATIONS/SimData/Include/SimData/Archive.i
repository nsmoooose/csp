/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
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

%module Archive
%{
#include "SimData/Archive.h"
%}

%include "std_string.i"
%include "SimData/filemap.i"

%ignore SIMDATA(Archive)::operator();

%typemap(python, in) (const char* data, int n) {
	$1 = PyString_AsString($input);
	$2 = PyString_Size($input);
}

%include "SimData/Archive.h"

%exception;
