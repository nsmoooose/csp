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

void TypeAdapter::set(SimDate & x) const { setBase(x); }

void TypeAdapter::set(GeoPos & x) const { setBase(x); }

void TypeAdapter::set(LLA & x) const { setCoordinate(x); }
void TypeAdapter::set(UTM & x) const { setCoordinate(x); }
void TypeAdapter::set(ECEF & x) const { setCoordinate(x); }

void TypeAdapter::set(Vector3 & x) const { setBase(x); }
void TypeAdapter::set(Matrix3 & x) const { setBase(x); }
void TypeAdapter::set(Real & x) const { setBase(x); }
void TypeAdapter::set(Curve & x) const { setBase(x); }
void TypeAdapter::set(Table & x) const { setBase(x); }
#ifndef __PTS_SIM__
void TypeAdapter::set(Table1 & x) const { setBase(x); }
void TypeAdapter::set(Table2 & x) const { setBase(x); }
void TypeAdapter::set(Table3 & x) const { setBase(x); }
#endif // __PTS_SIM__
void TypeAdapter::set(External & x) const { setBase(x); }
void TypeAdapter::set(Key & x) const { setBase(x); }
void TypeAdapter::set(Path & x) const { setBase(x); }

void TypeAdapter::set(EnumLink &x) const { if (isType(STRING)) x = s; else setBase(x); }

// slightly fancier handling required for path pointers
void TypeAdapter::set(LinkBase &x) const {
	BaseCheck();
	// are we assigning to a pointerbase?
	LinkBase const *p = dynamic_cast<LinkBase const *>(var.o);
	if (p != 0) {
		x = *(const_cast<LinkBase *>(p));
	} else {
		// last chance, is it a path?
		Path const *path = dynamic_cast<Path const *>(var.o);
		TypeCheck(path!=NULL, "dynamic cast of BaseType* to LinkBase failed");
		x = LinkBase(*(const_cast<Path *>(path)), 0);
	}
}

/** Convenience function for dumping object member variables.
 */
std::ostream &operator <<(std::ostream &o, TypeAdapter const &t) {
	if (t.isType(TypeAdapter::INT)) return o << t.getInteger();
	if (t.isType(TypeAdapter::DOUBLE)) return o << t.getFloatingPoint();
	if (t.isType(TypeAdapter::STRING)) return o << t.getString();
	if (t.isType(TypeAdapter::BASE)) return o << "TypeAdapter<Base>";
	return o << "TypeAdapter<UNKNOWN>";
}


NAMESPACE_SIMDATA_END

