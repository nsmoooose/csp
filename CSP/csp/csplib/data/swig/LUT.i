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

%module LUT
%{
#include <csp/csplib/data/LUT.h>
%}

#ifndef __NO_LUT__

%include "csp/csplib/util/Namespace.h"

%typemap(in) std::vector<std::vector<float> > const & (std::vector<std::vector<float> > temp) {
	if (PyTuple_Check($input) || PyList_Check($input)) {
		unsigned int size = (PyTuple_Check($input) ?  PyTuple_Size($input) : PyList_Size($input));
		temp.resize(size);
		$1 = &temp;
		for (unsigned int i=0; i<size; i++) {
			PyObject* o = PySequence_GetItem($input,i);
			if (PyTuple_Check(o) || PyList_Check(o)) {
				unsigned int size = (PyTuple_Check(o) ?  PyTuple_Size(o) : PyList_Size(o));
				temp[i].resize(size);
				for (unsigned int j=0; j<size; j++) {
					PyObject* v = PySequence_GetItem(o,j);
					if (PyFloat_Check(v)) {
						temp[i][j] = static_cast<float>(PyFloat_AsDouble(v));
						Py_DECREF(v);
					} else if (PyInt_Check(v)) {
						temp[i][j] = static_cast<float>(PyInt_AsLong(v));
						Py_DECREF(v);
					} else if (PyLong_Check(v)) {
						temp[i][j] = static_cast<float>(PyLong_AsLong(v));
						Py_DECREF(v);
					} else {
						Py_DECREF(v);
						PyErr_SetString(PyExc_TypeError, "vector<vector<float>> expected");
						SWIG_fail;
					}
				}
				Py_DECREF(o);
			} else {
				Py_DECREF(o);
				PyErr_SetString(PyExc_TypeError, "vector<vector<float>> expected");
				SWIG_fail;
			}
		}
	} else {
		PyErr_SetString(PyExc_TypeError,"vector<vector<float>> expected");
		SWIG_fail;
	}
}


CSP_NAMESPACE

class Interpolation {
	Interpolation();
public:
	typedef enum { LINEAR, SPLINE } Modes;
	bool isInterpolated() const;
};


template <typename X>
class InterpolationType: public Interpolation {
};

#define TRY try { $action }
#define CATCH(a, b) \
	catch (a& e) {\
		std::cout << "EXCEPT\n"; \
		SWIG_exception(b, const_cast<char*>(e.getError().c_str()));\
	}
	
%exception  {
	TRY
	CATCH(InterpolationIndex, SWIG_IndexError)
}

#undef TRY
#undef CATCH


template <int N, class X=float>
class LUT: public InterpolationType<X> {
	InterpolationType();
public:
	typedef VEC<N, int> Dim;
	typedef VEC<N, X> Vec;
	typedef VEC<N, std::vector<X> > Breaks;

	LUT();
	~LUT();

	%rename(__getitem__) getValue(std::vector<float> const &) const;
	X getValue(std::vector<float> const &) const;
	void load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks);
	void interpolate(std::vector<int> const &dim, Interpolation::Modes mode);
	void serialize(Reader&);
	void serialize(Writer&) const;
	%rename(__repr__) asString() const;
	std::string asString() const;
	std::string typeString() const;
};



%template(InterpolationTypeFloat) InterpolationType<float>;
%template(Table1) LUT<1,float>;
%template(Table2) LUT<2,float>;
%template(Table3) LUT<3,float>;


%exception;

CSP_NAMESPACE_END

#endif // __NO_LUT__

