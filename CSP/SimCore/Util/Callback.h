// Combat Simulator Project - CSPSim
// Copyright (C) 2004 The Combat Simulator Project
// http://csp.sourceforge.net
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
 * @file Callback.h
 * @brief Provides adaptors for sigc 2.0 member function slots.
 *
 * The adaptors do not require the target class to subclass sigc::trackable,
 * but are equally safe (unlike sigc::slot_class).  Automatic disconnect
 * is handled by making the callbacks instance variables, so they are
 * destroyed (and thereby disconnected) at the same time the target object
 * is destroyed.
 *
 * At present the signal and callback classes only support slots with a
 * limited number of argument and return value combinations.
 *
 * Sample use:
 *
 * @code
 *
 * class Target {
 *   void bar(double);
 *   Callback1<double> _barHandler;
 * public:
 *   Target(): barHandler(this, &Bar::bar) { }
 *   Callback1<double>& barHandler() { return _barHandler; }
 * };
 *
 * void Source {
 *   Signal1<double> _barSignal;
 *   void signalBar(double x) { _barSignal.emit(x); }
 * public:
 *   Signal1<double>& barSignal() { return _barSignal; }
 * };
 *
 * Source source;
 * Target target;
 * source.barSignal().connect(target.barHandler());
 *
 * @endcode
 *
 **/


#ifndef __SIMCORE_UTIL_CALLBACK_H__
#define __SIMCORE_UTIL_CALLBACK_H__

#include <SimCore/Util/CallbackDecl.h>
#include <sigc++/sigc++.h>

namespace simcore {


// Signals ----------------------------------------------------------------------------

class Signal0: public sigc::signal<void> { };

template <typename M>
class Signal1: public sigc::signal<void, M> { };

template <typename M, typename N>
class Signal2: public sigc::signal<void, M, N> { };

template <typename R>
class Signal0R: public sigc::signal<R> { };

template <typename R, typename M>
class Signal1R: public sigc::signal<R, M> { };


// Callbacks --------------------------------------------------------------------------

class _CallbackAdaptor: public sigc::trackable {
public:
	virtual ~_CallbackAdaptor() {}
};

template <class C>
class _CallbackAdaptor0: public _CallbackAdaptor {
	typedef void (C::*Method)();
	C *_instance;
	Method _method;
public:
	inline void bounce() { (_instance->*_method)(); }
	_CallbackAdaptor0(C *instance, Method method) : _instance(instance), _method(method) { }
};

class Callback0: private simdata::ScopedPointer<_CallbackAdaptor>, public sigc::slot<void> {
public:
	typedef Signal0 Signal;
	template <class C>
	Callback0(C *instance, void (C::*method)()) :
		simdata::ScopedPointer<_CallbackAdaptor>(new _CallbackAdaptor0<C>(instance, method)),
		sigc::slot<void>(sigc::mem_fun(*dynamic_cast<_CallbackAdaptor0<C>*>(get()), &_CallbackAdaptor0<C>::bounce)) { }
};

template <class C, typename M>
class _CallbackAdaptor1: public _CallbackAdaptor {
	typedef void (C::*Method)(M);
	C *_instance;
	Method _method;
public:
	inline void bounce(M m) { (_instance->*_method)(m); }
	_CallbackAdaptor1(C *instance, Method method) : _instance(instance), _method(method) { }
};

template <typename M>
class Callback1: private simdata::ScopedPointer<_CallbackAdaptor>, public sigc::slot<void, M> {
public:
	typedef Signal1<M> Signal;
	template <class C>
	Callback1(C *instance, void (C::*method)(M)) :
		simdata::ScopedPointer<_CallbackAdaptor>(new _CallbackAdaptor1<C, M>(instance, method)),
		sigc::slot<void, M>(sigc::mem_fun(*dynamic_cast<_CallbackAdaptor1<C, M>*>(get()), &_CallbackAdaptor1<C, M>::bounce)) { }
};

template <class C, typename M, typename N>
class _CallbackAdaptor2: public _CallbackAdaptor {
	typedef void (C::*Method)(M, N);
	C *_instance;
	Method _method;
public:
	inline void bounce(M m, N n) { (_instance->*_method)(m, n); }
	_CallbackAdaptor2(C *instance, Method method) : _instance(instance), _method(method) { }
};

template <typename M, typename N>
class Callback2: private simdata::ScopedPointer<_CallbackAdaptor>, public sigc::slot<void, M, N> {
public:
	typedef Signal2<M, N> Signal;
	template <class C>
	Callback2(C *instance, void (C::*method)(M, N)) :
		simdata::ScopedPointer<_CallbackAdaptor>(new _CallbackAdaptor2<C, M, N>(instance, method)),
		sigc::slot<void, M, N>(sigc::mem_fun(*dynamic_cast<_CallbackAdaptor2<C, M, N>*>(get()), &_CallbackAdaptor2<C, M, N>::bounce)) { }
};

template <class C, typename R>
class _CallbackAdaptor0R: public _CallbackAdaptor {
	typedef R (C::*Method)();
	C *_instance;
	Method _method;
public:
	inline R bounce() { return (_instance->*_method)(); }
	_CallbackAdaptor0R(C *instance, Method method) : _instance(instance), _method(method) { }
};

template <typename R>
class Callback0R: private simdata::ScopedPointer<_CallbackAdaptor>, public sigc::slot<R> {
public:
	typedef Signal0R<R> Signal;
	template <class C>
	Callback0R(C *instance, R (C::*method)()) :
		simdata::ScopedPointer<_CallbackAdaptor>(new _CallbackAdaptor0R<C, R>(instance, method)),
		sigc::slot<R>(sigc::mem_fun(*dynamic_cast<_CallbackAdaptor0R<C, R>*>(get()), &_CallbackAdaptor0R<C, R>::bounce)) { }
};


// ScopedPointer Callbacks  -----------------------------------------------------------

template <class C>
ScopedCallback0::ScopedCallback0(C *instance, void (C::*method)())
	: simdata::ScopedPointer<Callback0>(new Callback0(instance, method)) { }

template <typename M>
template <class C>
ScopedCallback1<M>::ScopedCallback1(C *instance, void (C::*method)(M))
	: simdata::ScopedPointer<Callback1<M> >(new Callback1<M>(instance, method)) { }

template <typename M, typename N>
template <class C>
ScopedCallback2<M, N>::ScopedCallback2(C *instance, void (C::*method)(M, N))
	: simdata::ScopedPointer<Callback2<M, N> >(new Callback2<M, N>(instance, method)) { }

template <typename R>
template <class C>
ScopedCallback0R<R>::ScopedCallback0R(C *instance, R (C::*method)())
	: simdata::ScopedPointer<Callback0R<R> >(new Callback0R<R>(instance, method)) { }


} // namespace simcore

#endif // __SIMCORE_UTIL_CALLBACK_H__

