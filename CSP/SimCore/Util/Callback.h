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
 * @brief Provides an adaptor for sigc++-1.2 member function slots.
 *
 * The adaptors do not require the target class to subclass SigC::Object,
 * but are equally safe (unlike SigC::slot_class).  Automatic disconnect
 * is handled by making the callbacks instance variables, so they are
 * destroyed (and thereby disconnect) at the same time the target object
 * is destroyed.
 *
 * At present the signal and callback classes only support slots with no
 * return value and a limited number of arguements.
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


// Callbacks --------------------------------------------------------------------------

template <class C>
class _CallbackAdaptor0: public SigC::Object {
	typedef void (C::*Method)();
	C *_instance;
	Method _method;
public:
	inline void bounce() { (_instance->*_method)(); }
	_CallbackAdaptor0(C *instance, Method method) : _instance(instance), _method(method) { }
};

class Callback0: private simdata::ScopedPointer<SigC::Object>, public SigC::Slot0<void> {
public:
	template <class C>
	Callback0(C *instance, void (C::*method)()) :
		simdata::ScopedPointer<SigC::Object>(new _CallbackAdaptor0<C>(instance, method)),
		SigC::Slot0<void>(SigC::slot(*dynamic_cast<_CallbackAdaptor0<C>*>(get()), &_CallbackAdaptor0<C>::bounce)) { }
};

template <class C, typename M>
class _CallbackAdaptor1: public SigC::Object {
	typedef void (C::*Method)(M);
	C *_instance;
	Method _method;
public:
	inline void bounce(M m) { (_instance->*_method)(m); }
	_CallbackAdaptor1(C *instance, Method method) : _instance(instance), _method(method) { }
};

template <typename M>
class Callback1: private simdata::ScopedPointer<SigC::Object>, public SigC::Slot1<void, M> {
public:
	template <class C>
	Callback1(C *instance, void (C::*method)(M)) :
		simdata::ScopedPointer<SigC::Object>(new _CallbackAdaptor1<C, M>(instance, method)),
		SigC::Slot1<void, M>(SigC::slot(*dynamic_cast<_CallbackAdaptor1<C, M>*>(get()), &_CallbackAdaptor1<C, M>::bounce)) { }
};

template <class C, typename M, typename N>
class _CallbackAdaptor2: public SigC::Object {
	typedef void (C::*Method)(M, N);
	C *_instance;
	Method _method;
public:
	inline void bounce(M m, N n) { (_instance->*_method)(m, n); }
	_CallbackAdaptor2(C *instance, Method method) : _instance(instance), _method(method) { }
};

template <typename M, typename N>
class Callback2: private simdata::ScopedPointer<SigC::Object>, public SigC::Slot2<void, M, N> {
public:
	template <class C>
	Callback2(C *instance, void (C::*method)(M, N)) :
		simdata::ScopedPointer<SigC::Object>(new _CallbackAdaptor2<C, M, N>(instance, method)),
		SigC::Slot2<void, M, N>(SigC::slot(*dynamic_cast<_CallbackAdaptor2<C, M, N>*>(get()), &_CallbackAdaptor2<C, M, N>::bounce)) { }
};


// Signals ----------------------------------------------------------------------------

class Signal0: public SigC::Signal0<void> { };

template <typename M>
class Signal1: public SigC::Signal1<void, M> { };

template <typename M, typename N>
class Signal2: public SigC::Signal2<void, M, N> { };


// ScopedPointer Callbacks  -----------------------------------------------------------

template <class C>
ScopedCallback0::ScopedCallback0(C *instance, void (C::*method)())
	: simdata::ScopedPointer<Callback0>(new Callback0(instance, method)) { }

template <typename M>
template <class C>
ScopedCallback1<M>::ScopedCallback1<M>(C *instance, void (C::*method)(M))
	: simdata::ScopedPointer<Callback1<M> >(new Callback1<M>(instance, method)) { }

template <typename M, typename N>
template <class C>
ScopedCallback2<M, N>::ScopedCallback2<M, N>(C *instance, void (C::*method)(M, N))
	: simdata::ScopedPointer<Callback2<M, N> >(new Callback2<M, N>(instance, method)) { }


} // namespace simcore

#endif // __SIMCORE_UTIL_CALLBACK_H__

