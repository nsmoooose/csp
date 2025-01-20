#pragma once
// Combat Simulator Project
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
 *   csp::callback<void, double> _bar_callback;
 * public:
 *   Target() { _bar_callback.init(this, &Bar::bar); }
 *   csp::callback<void, double> &callback() { return _bar_callback; }
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

#include <csp/csplib/util/Namespace.h>
#include <sigc++/sigc++.h>

namespace csp {

/** Base class for all callbacks.  Not for public use.
 */
class callbackbase: public sigc::trackable {
protected:
	sigc::connection _connection;
	sigc::connection capture(sigc::connection const &connection) { _connection = connection; return connection; }
	~callbackbase() { _connection.disconnect(); }
};

template <typename T_return, typename... T_arg>
class callback;

template <typename T_return, typename... T_arg>
class callback<T_return(T_arg...)>: public callbackbase, public sigc::slot<T_return(T_arg...)> {
public:
	callback() { }

	template <class F>
	callback(F const &functor): sigc::slot<T_return(T_arg...)>(functor) {
	}

	template <class Obj>
	void init(Obj *o, T_return (Obj::*m)(T_arg...)) {
		sigc::slot<T_return(T_arg...)>::operator=( sigc::mem_fun(*o, m) );
	}

	sigc::connection bind(sigc::signal<T_return(T_arg...)> &signal) {
		return capture(signal.connect(*this));
	}
};

} // namespace csp
