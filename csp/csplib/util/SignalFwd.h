#pragma once
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

/** 
 * @file Referenced.h
 *
 * Forward declarations of sigc::signal and sigc::slot wrappers.
 *
 *  libsigc++-2.0 signal and slot templates cannot be forward declared
 *  because default template arguments are specified in the class
 *  definitions.  sigc could easily be restructured to allow forward
 *  declarations, simply by moving the default template arguments to
 *  forward declarations in a separate header file.
 *
 *  The forward declarations here are provided as a workaround.  The
 *  signal and slot classes (in the 'csp' namespace) are subclasses
 *  of the corresponding sigc entities, and can be used anywhere a
 *  sigc object is expected.  With this header it is possible to
 *  use slot and signal pointers and references in class declarations
 *  without including all of sigc in the header.
 *
 *  For example:
 *
 *  @code
 *  handler.h:
 *    #include <csp/csplib/util/SignalFwd.h>
 *    struct Handler {
 *      Handler();
 *      ~Handler();
 *      void defaultCallback(int) {}
 *      csp::slot<void, int> *callback;
 *    };
 *
 *  handler.cpp:
 *    #include "handler.h"
 *    #include <csp/csplib/util/Signal.h>
 *    #include <sigc++/functors/mem_fun.h>
 *    Handler::Handler(): callback(new csp::slot<void, int>) {
 *      *callback = sigc::mem_fun(this, &Handler::defaultCallback);
 *    }
 *    Handler::~Handler() { delete callback; }
 *  @endcode
 */

// We need one sigc forward declaration to make this work.  sigc::nil
// is defined in exactly this way in sigc++/functors/functor_trait.h,
// but we don't need other declarations in that header.
namespace sigc { struct nil; }

namespace csp {

// signals

template <class ret, class arg1=sigc::nil, class arg2=sigc::nil, class arg3=sigc::nil, class arg4=sigc::nil, class arg5=sigc::nil, class arg6=sigc::nil, class arg7=sigc::nil>
class signal;

template <class ret>
class signal<ret, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1>
class signal<ret, arg1, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2>
class signal<ret, arg1, arg2, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3>
class signal<ret, arg1, arg2, arg3, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3, class arg4>
class signal<ret, arg1, arg2, arg3, arg4, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5>
class signal<ret, arg1, arg2, arg3, arg4, arg5, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5, class arg6>
class signal<ret, arg1, arg2, arg3, arg4, arg5, arg6, sigc::nil>;

// slots

template <class ret, class arg1=sigc::nil, class arg2=sigc::nil, class arg3=sigc::nil, class arg4=sigc::nil, class arg5=sigc::nil, class arg6=sigc::nil, class arg7=sigc::nil>
class slot;

template <class ret>
class slot<ret, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1>
class slot<ret, arg1, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2>
class slot<ret, arg1, arg2, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3>
class slot<ret, arg1, arg2, arg3, sigc::nil, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3, class arg4>
class slot<ret, arg1, arg2, arg3, arg4, sigc::nil, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5>
class slot<ret, arg1, arg2, arg3, arg4, arg5, sigc::nil, sigc::nil>;

template <class ret, class arg1, class arg2, class arg3, class arg4, class arg5, class arg6>
class slot<ret, arg1, arg2, arg3, arg4, arg5, arg6, sigc::nil>;

} // namespace csp
