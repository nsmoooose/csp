// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

#ifndef __CSPLIB_UTIL_SIGNAL_H__
#define __CSPLIB_UTIL_SIGNAL_H__

#include <csp/csplib/util/SignalFwd.h>

#include <sigc++/signal.h>
#include <sigc++/functors/slot.h>

CSP_NAMESPACE

template <class ret>
struct signal<ret, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::signal<ret> { };

template <class ret, class arg1>
struct signal<ret, arg1, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::signal<ret, arg1> { };

template <class ret, class arg1, class arg2>
struct signal<ret, arg1, arg2, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::signal<ret, arg1, arg2> { };

template <class ret, class arg1, class arg2, class arg3>
struct signal<ret, arg1, arg2, arg3, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::signal<ret, arg1, arg2, arg3> { };

template <class ret, class arg1, class arg2, class arg3, class arg4>
struct signal<ret, arg1, arg2, arg3, arg4, sigc::nil, sigc::nil, sigc::nil>: public sigc::signal<ret, arg1, arg2, arg3, arg4> { };

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5>
struct signal<ret, arg1, arg2, arg3, arg4, arg5, sigc::nil, sigc::nil>: public sigc::signal<ret, arg1, arg2, arg3, arg4, arg5> { };

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5, class arg6>
struct signal<ret, arg1, arg2, arg3, arg4, arg5, arg6, sigc::nil>: public sigc::signal<ret, arg1, arg2, arg3, arg4, arg5, arg6> { };

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5, class arg6, class arg7>
struct signal: public sigc::signal<ret, arg1, arg2, arg3, arg4, arg5, arg6, arg7> { };


template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5, class arg6, class arg7>
struct slot: public sigc::slot<ret, arg1, arg2, arg3, arg4, arg5, arg6, arg7> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1, arg2, arg3, arg4, arg5, arg6, arg7>(f) {}
};

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5, class arg6>
struct slot<ret, arg1, arg2, arg3, arg4, arg5, arg6, sigc::nil>: public sigc::slot<ret, arg1, arg2, arg3, arg4, arg5, arg6> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1, arg2, arg3, arg4, arg5, arg6>(f) {}
};

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5>
struct slot<ret, arg1, arg2, arg3, arg4, arg5, sigc::nil, sigc::nil>: public sigc::slot<ret, arg1, arg2, arg3, arg4, arg5> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1, arg2, arg3, arg4, arg5>(f) {}
};

template <class ret, class arg1, class arg2, class arg3, class arg4>
struct slot<ret, arg1, arg2, arg3, arg4, sigc::nil, sigc::nil, sigc::nil>: public sigc::slot<ret, arg1, arg2, arg3, arg4> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1, arg2, arg3, arg4>(f) {}
};

template <class ret, class arg1, class arg2, class arg3>
struct slot<ret, arg1, arg2, arg3, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::slot<ret, arg1, arg2, arg3> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1, arg2, arg3>(f) {}
};

template <class ret, class arg1, class arg2>
struct slot<ret, arg1, arg2, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::slot<ret, arg1, arg2> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1, arg2>(f) {}
};

template <class ret, class arg1>
struct slot<ret, arg1, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::slot<ret, arg1> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret, arg1>(f) {}
};

template <class ret>
struct slot<ret, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>: public sigc::slot<ret> {
	slot() {}
	template <class functor> slot(const functor &f): sigc::slot<ret>(f) {}
};

CSP_NAMESPACE_END

#endif  // __CSPLIB_UTIL_SIGNAL_H__

