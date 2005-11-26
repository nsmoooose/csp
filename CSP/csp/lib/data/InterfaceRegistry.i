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
#include <csp/lib/data/ObjectInterface.h>
#include <csp/lib/data/InterfaceProxy.h>
#include <csp/lib/data/InterfaceRegistry.h>
#include <csp/lib/util/Namespace.h>
%}

%import "csp/lib/swig/vector.i"
%include "csp/lib/util/Namespace.h"

CSP_NAMESPACE

%newobject InterfaceProxy::createObject;

// silence swig warnings about unknown base class Singleton<InterfaceRegistry>
class InterfaceRegistry;
template <class T> class Singleton;
%template(SingletonInterfaceRegistry) Singleton<InterfaceRegistry>;
%warnfilter(402) InterfaceRegistry;

CSP_NAMESPACE_END


%exception {
	try {
		$action
	} catch (CSP(TypeMismatch) &e) {
		e.clear();
		SWIG_exception(SWIG_TypeError, e.getMessage().c_str());
	} catch (CSP(InterfaceError) &e) {
		e.clear();
		SWIG_exception(SWIG_SyntaxError, e.getMessage().c_str());
	}
}

new_vector(pInterfaceProxy, CSP(InterfaceProxy)*)

%include "csp/lib/data/InterfaceProxy.h"
%include "csp/lib/data/InterfaceRegistry.h"

%exception;

