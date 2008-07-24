/* Combat Simulator Project
 * Copyright (C) 2002, 2003 Mark Rose <mkrose@users.sf.net>
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
#include <csp/csplib/data/Enum.h>
%}

%import "csp/csplib/util/Ref.h"

namespace csp {

%template(RefEnumerationCore) Ref<CSP(EnumerationCore) const>;

} // namespace csp

%typemap(out) std::vector<CSP(EnumLink)>
       "{
            $result = PyTuple_New($1.size());
            for (unsigned int i=0; i<$1.size(); i++) {
                CSP(EnumLink)* ptr = new CSP(EnumLink)((($1_type &)$1)[i]);
                PyTuple_SetItem($result, i,
                                SWIG_NewPointerObj((void *) ptr,
                                $descriptor(csp::EnumLink *), 1));
            }
       }"

%typemap(out) std::vector<std::string> CSP(Enumeration)::eachString()
       "{
            $result = PyTuple_New($1.size());
            for (unsigned int i=0; i<$1.size(); i++) {
                PyTuple_SetItem($result,i, PyString_FromString($1[i].c_str()));
            }
       }"


#define TRY try { $action }
#define CATCH(a, b) \
	catch (a& e) {\
		SWIG_exception(b, const_cast<char*>(e.getError().c_str()));\
	}

%exception  {
	TRY
	CATCH(CSP(EnumTypeError), SWIG_TypeError)
	CATCH(CSP(EnumIndexError), SWIG_IndexError)
	CATCH(CSP(EnumError), SWIG_RuntimeError)
}

#undef TRY
#undef CATCH


%include "csp/csplib/data/Enum.h"


#define CSP_ENUM_WRAP1(T) \
%typemap(in) CSP(Enum)<T> * (int __index, std::string __string, CSP(EnumLink) *__enumlink) \
	"{ \
		__index = -1; \
		__string = \"\"; \
		__enumlink = 0; \
		if (PyInt_Check($input)) { \
			__index = PyInt_AsLong($input); \
		} else \
		if (PyString_Check($input)) { \
			__string = std::string(PyString_AsString($input)); \
		} else \
		if ((SWIG_ConvertPtr($input,(void **) &$1, $1_descriptor, 0 )) == -1) { \
			if ((SWIG_ConvertPtr($input,(void **) &__enumlink, SWIGTYPE_p_csp__EnumLink, 0 )) == -1) { \
				PyErr_SetString(PyExc_TypeError, \"string or enum expected\"); \
			} \
		} \
	}"; \

#define CSP_ENUM_WRAP2(T) \
%typemap(memberin) CSP(Enum)<T> \
	"{ \
		if ($input) { \
			$1 = *$input; \
		} else \
		if (__enumlink2) { \
			$1 = *__enumlink2; \
		} else \
		if (__index2 >=0) { \
			$1 = __index2; \
		} else { \
			$1 = __string2; \
		} \
	 }";

#define CSP_ENUM_WRAP(NAME, T) \
	%template(NAME) csp::Enum<T>; \
	CSP_ENUM_WRAP1(T) \
	CSP_ENUM_WRAP2(T)

%exception;

