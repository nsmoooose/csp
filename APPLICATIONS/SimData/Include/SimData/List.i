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

%module List
%{
#include "SimData/List.h"
#include "SimData/Real.h"
#include "SimData/Vector3.h"
#include "SimData/Path.h"
#include "SimData/Interpolate.h"
#include "SimData/External.h"
#include "SimData/Key.h"
%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/ns-simdata.h"

%rename (__repr__) SIMDATA(List)::asString() const;

%feature("polymorphic") SIMDATA(ListBase);

%include "SimData/List.h"

//%import "SimData/Vector3.h"

#define new_list(a, b)	\
	%template(v##a) std::vector< b >; \
	%template(l##a) SIMDATA(List)< b >;


//#ifdef __LIST_TEMPLATES__
//#define __LIST_TEMPLATES__
new_list(Int, int);
new_list(Real, SIMDATA(Real));
//new_list(Vector3, SIMDATA(Vector3));
new_list(Path, SIMDATA(Path));

// HACK!
#ifndef VSTRING
new_list(String, std::string);
#define VSTRING
#endif

new_list(Curve, SIMDATA(Curve));
new_list(Table, SIMDATA(Table));
new_list(External, SIMDATA(External));
new_list(Key, SIMDATA(Key));
//#endif // __LIST_TEMPLATES__

