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


/**
 * @file TypeAdapter.cpp
 */

#include <SimData/TypeAdapter.h>
#include <SimData/Types.h>
#include <SimData/Object.h>


NAMESPACE_SIMDATA


const char *TypeAdapter::TypeNames[] = {"NONE", "Integer", "Double", "String", "Base"};


/**
 * Convenience function for dumping object member variables.
 */
std::ostream &operator <<(std::ostream &o, TypeAdapter const &t) {
	if (t.isType(TypeAdapter::INT)) return o << t.getInteger();
	if (t.isType(TypeAdapter::DOUBLE)) return o << t.getFloatingPoint();
	if (t.isType(TypeAdapter::STRING)) return o << t.getString();
	if (t.isType(TypeAdapter::BASE)) return o << "TypeAdapter<Base>";
	return o << "TypeAdapter<UNKNOWN>";
}


NAMESPACE_END // namespace simdata

