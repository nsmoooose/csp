/* Combat Simulator Project
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

%module InterfaceRegistry
%{
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/InterfaceProxy.h>
#include <csp/csplib/data/InterfaceRegistry.h>
%}

%import <csp/csplib/swig/vector.i>

namespace csp {

%newobject InterfaceProxy::createObject;

// silence swig warnings about unknown base class Singleton<InterfaceRegistry>
class InterfaceRegistry;
template <class T> class Singleton;
%template(SingletonInterfaceRegistry) Singleton<InterfaceRegistry>;
%warnfilter(402) InterfaceRegistry;

} // namespace csp


%exception {
	try {
		$action
	} catch (csp::TypeMismatch &e) {
		e.clear();
		SWIG_exception(SWIG_TypeError, e.getMessage().c_str());
	} catch (csp::InterfaceError &e) {
		e.clear();
		SWIG_exception(SWIG_SyntaxError, e.getMessage().c_str());
	}
}

new_vector(pInterfaceProxy, csp::InterfaceProxy*)

%include <csp/csplib/data/InterfaceProxy.h>
%include <csp/csplib/data/InterfaceRegistry.h>

%exception;

