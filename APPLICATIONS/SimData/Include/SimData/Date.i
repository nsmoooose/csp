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

%module Date
%{
#include "SimData/Date.h"
%}

%include "std_string.i"

NAMESPACE_SIMDATA
%rename(__repr__) Date::asString() const;
%rename(__repr__) Zulu::asString() const;
%rename(__repr__) DateZulu::asString() const;
%rename(__repr__) SimDate::asString() const;
%rename(__cmp__) Date::compare(Date const &) const;
%rename(__cmp__) SimDate::compare(SimDate const &) const;
NAMESPACE_SIMDATA_END

%include "SimData/Date.h"

