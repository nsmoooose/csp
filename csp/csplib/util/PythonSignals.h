
// Generated code --- do not edit!
// Created by: tools/python_signals/pysiggen.py

#pragma once

#include <Python.h>
#include <sigc++/functors/slot.h>
#include "csp/csplib/util/PythonCasts.h"
#include "csp/csplib/util/PythonSwig.h"

struct PyError {};

template <typename T_return>
class pyslotbase : public sigc::functor_base {
public:
	typedef T_return result_type;
protected:
	pyslotbase(PyObject* handler) : _handler(handler) { Py_INCREF(_handler); }
	pyslotbase(pyslotbase const& other) : _handler(other._handler) { Py_INCREF(_handler); }
	~pyslotbase() { Py_DECREF(_handler); }
	PyObject* _handler;
		T_return call(PyObject* func) {
		PyObject* res = PyObject_CallFunction(func, "");
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0) {
		PyObject* res = PyObject_CallFunction(func, "O", arg0);
		Py_DECREF(arg0);
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0, PyObject* arg1) {
		PyObject* res = PyObject_CallFunction(func, "OO", arg0, arg1);
		Py_DECREF(arg0);
		Py_DECREF(arg1);
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0, PyObject* arg1, PyObject* arg2) {
		PyObject* res = PyObject_CallFunction(func, "OOO", arg0, arg1, arg2);
		Py_DECREF(arg0);
		Py_DECREF(arg1);
		Py_DECREF(arg2);
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0, PyObject* arg1, PyObject* arg2, PyObject* arg3) {
		PyObject* res = PyObject_CallFunction(func, "OOOO", arg0, arg1, arg2, arg3);
		Py_DECREF(arg0);
		Py_DECREF(arg1);
		Py_DECREF(arg2);
		Py_DECREF(arg3);
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0, PyObject* arg1, PyObject* arg2, PyObject* arg3, PyObject* arg4) {
		PyObject* res = PyObject_CallFunction(func, "OOOOO", arg0, arg1, arg2, arg3, arg4);
		Py_DECREF(arg0);
		Py_DECREF(arg1);
		Py_DECREF(arg2);
		Py_DECREF(arg3);
		Py_DECREF(arg4);
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0, PyObject* arg1, PyObject* arg2, PyObject* arg3, PyObject* arg4, PyObject* arg5) {
		PyObject* res = PyObject_CallFunction(func, "OOOOOO", arg0, arg1, arg2, arg3, arg4, arg5);
		Py_DECREF(arg0);
		Py_DECREF(arg1);
		Py_DECREF(arg2);
		Py_DECREF(arg3);
		Py_DECREF(arg4);
		Py_DECREF(arg5);
		return py_decref_cast<T_return>(res);
	}
	T_return call(PyObject* func, PyObject* arg0, PyObject* arg1, PyObject* arg2, PyObject* arg3, PyObject* arg4, PyObject* arg5, PyObject* arg6) {
		PyObject* res = PyObject_CallFunction(func, "OOOOOOO", arg0, arg1, arg2, arg3, arg4, arg5, arg6);
		Py_DECREF(arg0);
		Py_DECREF(arg1);
		Py_DECREF(arg2);
		Py_DECREF(arg3);
		Py_DECREF(arg4);
		Py_DECREF(arg5);
		Py_DECREF(arg6);
		return py_decref_cast<T_return>(res);
	}

private:
	pyslotbase& operator=(pyslotbase const& other);
};


template <class T_return>
struct pyslot0 : public pyslotbase<T_return> {
	pyslot0(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()() { return this->call(this->_handler); }
};

template <class T_return, class T_arg0>
struct pyslot1 : public pyslotbase<T_return> {
	pyslot1(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0) { return this->call(this->_handler, py_cast(arg0)); }
};

template <class T_return, class T_arg0, class T_arg1>
struct pyslot2 : public pyslotbase<T_return> {
	pyslot2(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0, T_arg1 arg1) { return this->call(this->_handler, py_cast(arg0), py_cast(arg1)); }
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2>
struct pyslot3 : public pyslotbase<T_return> {
	pyslot3(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0, T_arg1 arg1, T_arg2 arg2) { return this->call(this->_handler, py_cast(arg0), py_cast(arg1), py_cast(arg2)); }
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3>
struct pyslot4 : public pyslotbase<T_return> {
	pyslot4(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3) { return this->call(this->_handler, py_cast(arg0), py_cast(arg1), py_cast(arg2), py_cast(arg3)); }
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3, class T_arg4>
struct pyslot5 : public pyslotbase<T_return> {
	pyslot5(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4) { return this->call(this->_handler, py_cast(arg0), py_cast(arg1), py_cast(arg2), py_cast(arg3), py_cast(arg4)); }
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3, class T_arg4, class T_arg5>
struct pyslot6 : public pyslotbase<T_return> {
	pyslot6(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4, T_arg5 arg5) { return this->call(this->_handler, py_cast(arg0), py_cast(arg1), py_cast(arg2), py_cast(arg3), py_cast(arg4), py_cast(arg5)); }
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3, class T_arg4, class T_arg5, class T_arg6>
struct pyslot7 : public pyslotbase<T_return> {
	pyslot7(PyObject* handler) : pyslotbase<T_return>(handler) {}
	T_return operator()(T_arg0 arg0, T_arg1 arg1, T_arg2 arg2, T_arg3 arg3, T_arg4 arg4, T_arg5 arg5, T_arg6 arg6) { return this->call(this->_handler, py_cast(arg0), py_cast(arg1), py_cast(arg2), py_cast(arg3), py_cast(arg4), py_cast(arg5), py_cast(arg6)); }
};


template <class T_return, class T_arg0 = sigc::nil, class T_arg1 = sigc::nil, class T_arg2 = sigc::nil, class T_arg3 = sigc::nil, class T_arg4 = sigc::nil, class T_arg5 = sigc::nil, class T_arg6 = sigc::nil>
struct pyslot : public pyslot7<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4, T_arg5, T_arg6> {
	pyslot(PyObject* handler) : pyslot7<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4, T_arg5, T_arg6>(handler) {}
};

template <class T_return>
struct pyslot<T_return> : public pyslot0<T_return> {
	pyslot(PyObject* handler) : pyslot0<T_return>(handler) {}
};

template <class T_return, class T_arg0>
struct pyslot<T_return, T_arg0> : public pyslot1<T_return, T_arg0> {
	pyslot(PyObject* handler) : pyslot1<T_return, T_arg0>(handler) {}
};

template <class T_return, class T_arg0, class T_arg1>
struct pyslot<T_return, T_arg0, T_arg1> : public pyslot2<T_return, T_arg0, T_arg1> {
	pyslot(PyObject* handler) : pyslot2<T_return, T_arg0, T_arg1>(handler) {}
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2>
struct pyslot<T_return, T_arg0, T_arg1, T_arg2> : public pyslot3<T_return, T_arg0, T_arg1, T_arg2> {
	pyslot(PyObject* handler) : pyslot3<T_return, T_arg0, T_arg1, T_arg2>(handler) {}
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3>
struct pyslot<T_return, T_arg0, T_arg1, T_arg2, T_arg3> : public pyslot4<T_return, T_arg0, T_arg1, T_arg2, T_arg3> {
	pyslot(PyObject* handler) : pyslot4<T_return, T_arg0, T_arg1, T_arg2, T_arg3>(handler) {}
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3, class T_arg4>
struct pyslot<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4> : public pyslot5<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4> {
	pyslot(PyObject* handler) : pyslot5<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4>(handler) {}
};

template <class T_return, class T_arg0, class T_arg1, class T_arg2, class T_arg3, class T_arg4, class T_arg5>
struct pyslot<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4, T_arg5> : public pyslot6<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4, T_arg5> {
	pyslot(PyObject* handler) : pyslot6<T_return, T_arg0, T_arg1, T_arg2, T_arg3, T_arg4, T_arg5>(handler) {}
};


