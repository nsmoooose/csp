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
 * @brief Provides member function callback adaptors that work with sigc 2.0
 *   signals and disconnect automatically when deleted.
 *
 * The callback adaptors do not require the target class to subclass
 * sigc::trackable, but are equally safe when used properly.  To be safe,
 * it is vital that the callback lifetime be shorter than the target lifetime.
 * This ensures that the signal is disconnected before the target instance
 * is destroyed.  This lifetime requirement is easily satisfied by making
 * callbacks member variables of the target instance, as in the example
 * below.
 *
 * Sample use:
 *
 * @code
 *
 * class Target {
 *   void bar(double);
 *   simcore::callback<void, double> _bar_callback;
 * public:
 *   Target(): _bar_callback(this, &Bar::bar) { }
 *   simcore::callback<void, double> &callback() { return _bar_callback; }
 * };
 *
 * void Source {
 *   sigc::signal<void, double> _bar_signal;
 *   void bar(double x) { _bar_signal.emit(x); }
 * public:
 *   sigc::signal<void, double>& signal() { return _bar_signal; }
 * };
 *
 * Source source;
 *
 * {
 *   Target target;
 *   // the syntax for connecting signals to callbacks is reversed relative to
 *   // sigc slots: callback.bind(signal) instead of signal.connect(slot).
 *   target.callback().bind(source.signal());
 *   source.bar(3.0);  // calls target.bar()
 * }  // target out of scope, signal safely disconnected
 *
 * source.bar(4.0);  // does nothing
 *
 * @endcode
 *
 **/


#ifndef __SIMCORE_UTIL_CALLBACK_H__
#define __SIMCORE_UTIL_CALLBACK_H__

#include <sigc++/sigc++.h>

namespace simcore {


// callbacks --------------------------------------------------------------------------


/** Base class for all callbacks.  Not for public use.
 */
class callbackbase: public sigc::trackable {
protected:
	sigc::connection _connection;
	sigc::connection capture(sigc::connection const &connection) { _connection = connection; return connection; }
	~callbackbase() { _connection.disconnect(); }
};

/** Raw callback class for handling signals with no arguments.  Use callback<> instead.
 */
template <class ret>
struct callback0: public callbackbase, private sigc::slot<ret> {
	template <class F> callback0(F const &functor): sigc::slot<ret>(functor) { }
	sigc::connection bind(sigc::signal0<ret> &signal) { return capture(signal.connect(*this)); }
};

/** Raw callback class for handling signals with one argument.  Use callback<> instead.
 */
template <class ret, class arg1>
struct callback1: public callbackbase, private sigc::slot<ret, arg1> {
	template <class F> callback1(F const &functor): sigc::slot<ret, arg1>(functor) { }
	sigc::connection bind(sigc::signal1<ret, arg1> &signal) { return capture(signal.connect(*this)); }
};

/** Raw callback class for handling signals with two arguments.  Use callback<> instead.
 */
template <class ret, class arg1, class arg2>
struct callback2: public callbackbase, private sigc::slot<ret, arg1, arg2> {
	template <class F> callback2(F const &functor): sigc::slot<ret, arg1, arg2>(functor) { }
	sigc::connection bind(sigc::signal2<ret, arg1, arg2> &signal) { return capture(signal.connect(*this)); }
};

/** Raw callback class for handling signals with three arguments.  Use callback<> instead.
 */
template <class ret, class arg1, class arg2, class arg3>
struct callback3: public callbackbase, private sigc::slot<ret, arg1, arg2, arg3> {
	template <class F> callback3(F const &functor): sigc::slot<ret, arg1, arg2, arg3>(functor) { }
	sigc::connection bind(sigc::signal3<ret, arg1, arg2, arg3> &signal) { return capture(signal.connect(*this)); }
};

/** Raw callback class for handling signals with four arguments.  Use callback<> instead.
 */
template <class ret, class arg1, class arg2, class arg3, class arg4>
struct callback4: public callbackbase, private sigc::slot<ret, arg1, arg2, arg3, arg4> {
	template <class F> callback4(F const &functor): sigc::slot<ret, arg1, arg2, arg3, arg4>(functor) { }
	sigc::connection bind(sigc::signal3<ret, arg1, arg2, arg3, arg4> &signal) { return capture(signal.connect(*this)); }
};


/** Convenience class for declaring a callback with arbitrary return value and argument types.
 *  Up to four arguments are supported.
 *  TODO Extend to as many arguments as sigc supports.
 */
template <class ret, class arg1=sigc::nil, class arg2=sigc::nil, class arg3=sigc::nil, class arg4=sigc::nil>
struct callback { };

/** Template specialization for no arguments.
 */
template <class ret>
struct callback<ret>: public callback0<ret> {
	template <class F> callback(F const &functor): callback0<ret>(functor) { }
	template <class obj> callback(obj *o, ret (obj::*m)()): callback0<ret>(sigc::mem_fun(*o, m)) { }
};

/** Template specialization for one argument.
 */
template <class ret, class arg1>
struct callback<ret, arg1>: public callback1<ret, arg1> {
	template <class F> callback(F const &functor): callback1<ret, arg1>(functor) { }
	template <class obj> callback(obj *o, ret (obj::*m)(arg1)): callback1<ret, arg1>(sigc::mem_fun(*o, m)) { }
};

/** Template specialization for two arguments.
 */
template <class ret, class arg1, class arg2>
struct callback<ret, arg1, arg2>: public callback2<ret, arg1, arg2> {
	template <class F> callback(F const &functor): callback2<ret, arg1, arg2>(functor) { }
	template <class obj> callback(obj *o, ret (obj::*m)(arg1, arg2)): callback2<ret, arg1, arg2>(sigc::mem_fun(*o, m)) { }
};

/** Template specialization for three arguments.
 */
template <class ret, class arg1, class arg2, class arg3>
struct callback<ret, arg1, arg2, arg3>: public callback3<ret, arg1, arg2, arg3> {
	template <class F> callback(F const &functor): callback3<ret, arg1, arg2, arg3>(functor) { }
	template <class obj> callback(obj *o, ret (obj::*m)(arg1, arg2, arg3)): callback3<ret, arg1, arg2, arg3>(sigc::mem_fun(*o, m)) { }
};

/** Template specialization for four arguments.
 */
template <class ret, class arg1, class arg2, class arg3, class arg4>
struct callback<ret, arg1, arg2, arg3, arg4>: public callback4<ret, arg1, arg2, arg3, arg4> {
	template <class F> callback(F const &functor): callback4<ret, arg1, arg2, arg3, arg4>(functor) { }
	template <class obj> callback(obj *o, ret (obj::*m)(arg1, arg2, arg3, arg4)): callback4<ret, arg1, arg2, arg3, arg4>(sigc::mem_fun(*o, m)) { }
};

} // namespace simcore

#endif // __SIMCORE_UTIL_CALLBACK_H__

