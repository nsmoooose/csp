/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
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
#include <SimData/Object.h>
#include <SimData/DataManager.h>
%}


%import "SimData/Exception.i"

//typedef  unsigned long long  hasht;   /* unsigned 8-byte type */
//typedef  int		     int32;

%include exception.i
%exception SIMDATA(DataManager) {
	try {
		$function
	} catch (SIMDATA(IndexError) e) {
		e.details();
		SWIG_exception(SWIG_IndexError, "Path not found");
	} catch (SIMDATA(ObjectMismatch) e) {
		e.details();
		SWIG_exception(SWIG_TypeError, "Object Mismatch");
	} catch (SIMDATA(BadMagic) e) {
		e.details();
		SWIG_exception(SWIG_IOError, "Bad Magic");
	} catch (SIMDATA(BadByteOrder) e) {
		e.details();
		SWIG_exception(SWIG_IOError, "Incorrect Byte Order");
	}
}


%import "SimData/Object.i"
%import "SimData/DataArchive.i"

%include "SimData/DataManager.h"

%exception;

