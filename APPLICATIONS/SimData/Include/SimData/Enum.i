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

%module "Enum"
%{
#include "SimData/Enum.h"
%}

%include "std_string.i"
%include "std_vector.i"

%include "SimData/Namespace.h"

%template(vEnum) std::vector<SIMDATA(Enum)>;

// HACK!
#ifndef VSTRING
%template(vString) std::vector<string>;
#define VSTRING
#endif

//%ignore eqstring;
//%ignore hashstring;
//%ignore string_map;

#define TRY	try { $action }
#define CATCH(a, b) \
	catch (a& e) {\
		std::cout << "EXCEPT\n"; \
		SWIG_exception(b, const_cast<char*>(e.getError().c_str()));\
	}
	
%include "exception.i"

%exception  {
	TRY
	CATCH(SIMDATA(EnumTypeError), SWIG_TypeError)
	CATCH(SIMDATA(EnumIndexError), SWIG_IndexError)
	CATCH(SIMDATA(EnumError), SWIG_RuntimeError)
}

#undef TRY
#undef CATCH

%rename(__repr__) SIMDATA(Enum)::asString() const;
%rename(__repr__) SIMDATA(Enumeration)::asString() const;

%include "SimData/Enum.h"

%exception;

