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
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/DataManager.h>
%}


%import <csp/csplib/util/swig/Exception.i>

%exception csp::DataManager {
    try {
        $function
	} catch (csp::IndexError e) {
        e.details();
        SWIG_exception(SWIG_IndexError, "Path not found");
    } catch (csp::ObjectMismatch e) {
        e.details();
        SWIG_exception(SWIG_TypeError, "Object Mismatch");
    } catch (csp::BadMagic e) {
        e.details();
        SWIG_exception(SWIG_IOError, "Bad Magic");
    } catch (csp::BadByteOrder e) {
        e.details();
        SWIG_exception(SWIG_IOError, "Incorrect Byte Order");
    }
}


%import <csp/csplib/data/swig/Object.i>
%import <csp/csplib/data/swig/DataArchive.i>

%include <csp/csplib/data/DataManager.h>

%exception;
