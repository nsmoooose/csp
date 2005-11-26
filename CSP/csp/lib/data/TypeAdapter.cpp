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


/**
 * @file TypeAdapter.cpp
 * @brief Adapter class providing uniform access to built-in types and BaseType classes
 */

#include <csp/lib/data/TypeAdapter.h>
#include <csp/lib/data/Types.h>
#include <csp/lib/data/Object.h>

#include <sstream>

CSP_NAMESPACE


const char *TypeAdapter::TypeNames[] = {
	"NONE", "Integer", "Double", "String", "Object", "ECEF", "EnumLink",
	"Key", "LinkBase", "LinkCore", "LLA", "Matrix3", "Path", "Quat", "Real",
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


CSP_NAMESPACE_END

