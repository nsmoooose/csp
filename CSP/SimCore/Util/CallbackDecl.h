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
 * @file CallbackDecl.h
 * @brief Forward declarations for callback and signal templates.
 *
 **/


#ifndef __SIMCORE_UTIL_CALLBACKDECL_H__
#define __SIMCORE_UTIL_CALLBACKDECL_H__

#include <SimData/ScopedPointer.h>

namespace simcore {

class Callback0;
template <typename M> class Callback1;
template <typename M, typename N> class Callback2;

class Signal0;
template <typename M> class Signal1;
template <typename M, typename N> class Signal2;

struct ScopedCallback0: public simdata::ScopedPointer<Callback0> {
	template <class C> ScopedCallback0(C *instance, void (C::*method)());
};

template <typename M>
struct ScopedCallback1: public simdata::ScopedPointer<Callback1<M> > {
	template <class C> ScopedCallback1(C *instance, void (C::*method)(M));
};

template <typename M, typename N>
struct ScopedCallback2: public simdata::ScopedPointer<Callback2<M, N> > {
	template <class C> ScopedCallback2(C *instance, void (C::*method)(M, N));
};

} // namespace simcore

#endif // __SIMCORE_UTIL_CALLBACKDECL_H__
