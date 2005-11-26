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

%module DataManager
%{
#include <csp/lib/data/Object.h>
#include <csp/lib/data/DataManager.h>
%}


%import "csp/lib/util/Exception.i"

%exception CSP(DataManager) {
    try {
        $function
    } catch (CSP(IndexError) e) {
        e.details();
        SWIG_exception(SWIG_IndexError, "Path not found");
    } catch (CSP(ObjectMismatch) e) {
        e.details();
        SWIG_exception(SWIG_TypeError, "Object Mismatch");
    } catch (CSP(BadMagic) e) {
        e.details();
        SWIG_exception(SWIG_IOError, "Bad Magic");
    } catch (CSP(BadByteOrder) e) {
        e.details();
        SWIG_exception(SWIG_IOError, "Incorrect Byte Order");
    }
}


%import "csp/lib/data/Object.i"
%import "csp/lib/data/DataArchive.i"

%include "csp/lib/data/DataManager.h"

%exception;
