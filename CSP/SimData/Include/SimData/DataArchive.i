/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <mkrose@users.sf.net>
 *
 * This file is part of SimData.
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

%module DataArchive
%{
#include <SimData/Object.h>
#include <SimData/DataArchive.h>
%}


%import "SimData/ExceptionBase.i"

//typedef  unsigned long long  hasht;   /* unsigned 8-byte type */
//typedef  int           int32;

namespace std {
    %template(vector_hasht) vector<SIMDATA(hasht)>;
}

/*
%exception SIMDATA(DataArchive) {
    try {
        $function
    } catch (SIMDATA(IndexError) e) {
        e.details(true);
        SWIG_exception(SWIG_IndexError, "Path not found");
    } catch (SIMDATA(ObjectMismatch) e) {
        e.details(true);
        SWIG_exception(SWIG_TypeError, "Object Mismatch");
    } catch (SIMDATA(BadMagic) e) {
        e.details(true);
        SWIG_exception(SWIG_IOError, "Bad Magic");
    } catch (SIMDATA(BadByteOrder) e) {
        e.details(true);
        SWIG_exception(SWIG_IOError, "Incorrect Byte Order");
    } catch (SIMDATA(MissingInterface) e) {
        e.details(true);
        SWIG_exception(SWIG_TypeError, "Missing interface");
    }
}
*/


%import "SimData/Object.i"

%typemap(out) FP {
    $result = PyFile_FromFile($1.f, const_cast<char*>($1.name.c_str()),
                    const_cast<char*>($1.mode.c_str()), 0);
}

%include "SimData/DataArchive.h"

//%exception;
