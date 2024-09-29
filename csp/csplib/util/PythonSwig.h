#pragma once
// Copyright 2007 Mark Rose <mkrose@users.sf.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/**
 * @file PythonSwig.h
 * @brief 
 * DEFINE_SWIG_CASTS(T) defines a number of functions that are needed
 * to use a referenced type T with the python signal callback framework. 
 * This framework is a bridge between sigc++ and python. This
 * macro should be placed in the %{ } header section of the SWIG interface
 * file that declares callbacks using type T.  For example:

 * test.i:
 * %module "test";
 * %{
 * #include "mytype.h"
 * #include "PythonSignals.h"
 * DEFINE_SWIG_CASTS(MyType)
 * %}
 * ...
 */

#define DEFINE_SWIG_CASTS(T) \
	static T* pycast_from_##T(PyObject *obj) { \
		void* argp1 = 0; \
		int res1 = SWIG_ConvertPtr(obj, &argp1, SWIGTYPE_p_##T, 0); \
		if (!SWIG_IsOK(res1)) { \
			PyErr_SetString(PyExc_TypeError, "expected value of type " #T "*"); \
			return NULL; \
		} \
		return reinterpret_cast<T*>(argp1); \
	} \
	static PyObject* pycast_to_##T(T* obj) { \
		obj->_incref(); \
		PyObject* ret = SWIG_NewPointerObj(SWIG_as_voidptr(obj), SWIGTYPE_p_##T, SWIG_POINTER_OWN); \
		return ret; \
	} \
	inline PyObject* py_cast(T*& x) { return pycast_to_##T(x); } \
	template <> inline T* py_cast<T*>(PyObject* o) { return pycast_from_##T(o); } \
	template <> inline T* py_decref_cast<T*>(PyObject* o) { T* result = pycast_from_##T(o); Py_XDECREF(o); return result;  } \
	inline PyObject* py_cast(ref_ptr<T>& x) { return pycast_to_##T(x.get()); } \
	template <> inline ref_ptr<T> py_cast<ref_ptr<T> >(PyObject* o) { return pycast_from_##T(o); } \
	template <> inline ref_ptr<T> py_decref_cast<ref_ptr<T> >(PyObject* o) { ref_ptr<T> result = pycast_from_##T(o); Py_XDECREF(o); return result; }
