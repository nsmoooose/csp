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

%module Pack
%{
#include "SimData/Pack.h"
%}

%include "std_string.i"
%include "SimData/filemap.i"

%rename(unpack_packable) SIMDATA(UnPacker)::unpack(BaseType&);

%ignore SIMDATA(Packer)::pack(char);
%ignore SIMDATA(Packer)::pack(short);
%ignore SIMDATA(Packer)::pack(hasht);
%ignore SIMDATA(Packer)::pack(std::string&);
%ignore SIMDATA(Packer)::pack(float);
%ignore SIMDATA(Packer)::pack(const std::string&);

%ignore SIMDATA(UnPacker)::unpack(char &);
%ignore SIMDATA(UnPacker)::unpack(short &);
%ignore SIMDATA(UnPacker)::unpack(hasht &);
%ignore SIMDATA(UnPacker)::unpack(std::string&);
%ignore SIMDATA(UnPacker)::unpack(float &);

%typemap(python, in) (const char* data, int n) {
	$1 = PyString_AsString($input);
	$2 = PyString_Size($input);
}

%include "SimData/Pack.h"

%exception;
