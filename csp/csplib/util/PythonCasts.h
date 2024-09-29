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
 * @file PythonCasts.h
 * @brief 
 *
 * Define the following functions to cast between common C/C++ types and
 * Python types:

 * py_none()
 *   : returns PyNone (with incref)
 * py_cast(ctype)
 *   : casts from ctype to PyObject*
 * py_cast<ctype>(PyObject*)
 *   : casts from PyObject* to ctype
 * py_decref_cast<ctype>(PyObject*)
 *   : casts from PyObject* to ctype and decrefs the PyObject.
 *
 * For conversions from Python, no type checks are performed.
 */

#include <Python.h>
#include <string>

inline PyObject* py_none() { return Py_BuildValue(""); }

inline PyObject* py_cast(const char* x) { return Py_BuildValue("s", x); }
inline PyObject* py_cast(std::string const& x) { return Py_BuildValue("s", x.data(), x.size()); }
inline PyObject* py_cast(char x) { return Py_BuildValue("c", x); }
inline PyObject* py_cast(int x) { return Py_BuildValue("i", x); }
inline PyObject* py_cast(float x) { return Py_BuildValue("f", x); }
inline PyObject* py_cast(double x) { return Py_BuildValue("d", x); }

// explicitly not bothering with const and volatile qualifiers since they add no
// functionality to these conversions; if such qualifiers are needed in a template
// context, one can define a type mapping along the lines of:
//
//   typedef unqualified_T = unqualified<T>::type;
//   T result = pycast<unqualified_T>(...);

#define DEFINE_PY_CASTS(type, func) \
	template<> inline type py_cast<type>(PyObject* o) { return static_cast<type>(func(o)); } \
	template<> inline type py_decref_cast<type>(PyObject* o) { type x = static_cast<type>(func(o)); Py_XDECREF(o); return x; } \

template <typename T> T py_cast(PyObject*);
template <> inline void py_cast<void>(PyObject*) { }

template <typename T> T py_decref_cast(PyObject*);
template <> inline void py_decref_cast<void>(PyObject* o) { Py_XDECREF(o); }

DEFINE_PY_CASTS(signed char, PyInt_AsLong)
DEFINE_PY_CASTS(unsigned char, PyInt_AsLong)

DEFINE_PY_CASTS(short, PyInt_AsLong)
DEFINE_PY_CASTS(unsigned short, PyInt_AsLong)

DEFINE_PY_CASTS(int, PyInt_AsLong)
DEFINE_PY_CASTS(unsigned int, PyLong_AsUnsignedLong)

DEFINE_PY_CASTS(long, PyLong_AsLong)
DEFINE_PY_CASTS(unsigned long, PyLong_AsUnsignedLong)

#ifdef PY_LONG_LONG
DEFINE_PY_CASTS(PY_LONG_LONG, PyLong_AsLongLong)
DEFINE_PY_CASTS(unsigned PY_LONG_LONG, PyLong_AsUnsignedLongLong)
#endif // PY_LONG_LONG

DEFINE_PY_CASTS(float, PyFloat_AsDouble)
DEFINE_PY_CASTS(double, PyFloat_AsDouble)

DEFINE_PY_CASTS(std::string, PyString_AsString)
