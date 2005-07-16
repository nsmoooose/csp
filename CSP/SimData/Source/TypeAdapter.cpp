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


/**
 * @file TypeAdapter.cpp
 * @brief Adapter class providing uniform access to built-in types and BaseType classes
 */

#include <SimData/TypeAdapter.h>
#include <SimData/Types.h>
#include <SimData/Object.h>

#include <sstream>


NAMESPACE_SIMDATA


const char *TypeAdapter::TypeNames[] = {
	"NONE", "Integer", "Double", "String", "Object", "ECEF", "EnumLink",
	"External", "Key", "LinkBase", "LinkCore", "LLA", "Matrix3", "Path", "Quat", "Real",
	"SimDate", "Table1", "Table2", "Table3", "UTM", "Vector3"};

void TypeAdapter::failConvert(const char *typestr) const {
	throw TypeMismatch(__repr__() + ": Cannot convert to " + typestr);
}

template <typename T>
void TypeAdapter::setBase(T & x, TYPE t) const {
	if (type != t) failConvert(TypeNames[t]);
	assert(type >= TYPE_Object);
	assert(var.o);
	T const *p = reinterpret_cast<T const *>(var.o);
	x = *const_cast<T*>(p);
}

template <>
void TypeAdapter::setBase<EnumLink>(EnumLink & x, TYPE) const {
	if (type == TYPE_EnumLink) {
		assert(var.o);
		x = *(const_cast<EnumLink*>(reinterpret_cast<EnumLink const *>(var.o)));
	} else if (type == TYPE_STRING) {
		x = getString();
	} else if (type == TYPE_INT) {
		x = getInteger();
	} else {
		failConvert("EnumLink");
	}
}

template <>
void TypeAdapter::setBase<Path>(Path & x, TYPE) const {
	if (type == TYPE_Path) {
		assert(var.o);
		x = *(const_cast<Path*>(reinterpret_cast<Path const *>(var.o)));
	} else if (type == TYPE_LinkBase) {
		assert(var.o);
		x = *(const_cast<LinkBase*>(reinterpret_cast<LinkBase const *>(var.o)));
	} else if (type == TYPE_STRING) {
		x = Path(getString());
	} else {
		failConvert("Path");
	}
}


#define BASETYPE_SET(BASETYPE) \
	void TypeAdapter::set(BASETYPE &x) const { setBase(x, TYPE_##BASETYPE); }

BASETYPE_SET(ECEF)
BASETYPE_SET(EnumLink)
BASETYPE_SET(External)
BASETYPE_SET(Key)
BASETYPE_SET(LinkBase)
BASETYPE_SET(LLA)
BASETYPE_SET(Matrix3)
BASETYPE_SET(Path)
BASETYPE_SET(Quat)
BASETYPE_SET(Real)
BASETYPE_SET(SimDate)
BASETYPE_SET(Table1)
BASETYPE_SET(Table2)
BASETYPE_SET(Table3)
BASETYPE_SET(UTM)
BASETYPE_SET(Vector3)

#undef BASETYPE_SET

/*
void TypeAdapter::set(SimDate & x) const { setBase(x); }
void TypeAdapter::set(LLA & x) const { setCoordinate(x); }
void TypeAdapter::set(UTM & x) const { setCoordinate(x); }
void TypeAdapter::set(ECEF & x) const { setCoordinate(x); }
void TypeAdapter::set(Vector3 & x) const { setBase(x); }
void TypeAdapter::set(Matrix3 & x) const { setBase(x); }
void TypeAdapter::set(Real & x) const { setBase(x); }
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
*/

const std::string TypeAdapter::__str__() const {
	std::ostringstream os;
	switch (type) {
		case TYPE_INT: os << getInteger(); break;
		case TYPE_DOUBLE: os << getFloatingPoint(); break;
		case TYPE_STRING: os << getString(); break;
		default: os << __repr__();
	}
	return os.str();
}

/** Convenience function for dumping object member variables.
 */
std::ostream &operator <<(std::ostream &o, TypeAdapter const &t) {
	return o << t.__str__();
}


NAMESPACE_SIMDATA_END

