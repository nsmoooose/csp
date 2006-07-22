/* Combat Simulator Project
 * Copyright (C) 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file TypeAdapter.h
 * @brief Adapter class providing uniform access to built-in types
 *   and BaseType classes.
 */


#ifndef __CSPLIB_DATA_TYPEADAPTER_H__
#define __CSPLIB_DATA_TYPEADAPTER_H__

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif

#include <csp/csplib/data/BaseType.h>
#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Exception.h>
#include <csp/csplib/util/Namespace.h>

#include <cassert>
#include <iosfwd>
#include <string>

CSP_NAMESPACE


CSP_EXCEPTION(TypeMismatch)


/** Dynamically typed wrapper for basic types and objects.
 *
 *  For internal use only.  This class is used to pass typed data to
 *  and from virtual methods in MemberAccessorBase.  The base class
 *  methods cannot be type-specific, so TypeAdapter provides a generic
 *  conduit for values and type information that can be dynamically
 *  tested by MemeberAccessorBase subclasses.  This wrapper supports
 *  ints, doubles, strings, and BaseType classes (including Object and
 *  all Object subclasses).
 *
 *  NOTE: this class must be extended whenever new BaseType classes are
 *  added.
 *
 *  @internal
 */
class CSPLIB_EXPORT TypeAdapter
{
	// WARNING: This list must be kept in sync with the TypeNames definition
	// in TypeAdapter.cpp!
	typedef enum {TYPE_NONE, TYPE_INT, TYPE_DOUBLE, TYPE_STRING,
	              TYPE_Object,  // first BaseType
	              TYPE_ECEF, TYPE_EnumLink, TYPE_External, TYPE_Key, TYPE_LinkBase, TYPE_LinkCore,
	              TYPE_LLA, TYPE_Matrix3, TYPE_Path, TYPE_Quat, TYPE_Real, TYPE_SimDate,
	              TYPE_Table1, TYPE_Table2, TYPE_Table3, TYPE_UTM, TYPE_Vector3} TYPE;
	static const char * TypeNames[];
public:

	TypeAdapter(): type(TYPE_NONE) {}
	explicit TypeAdapter(int x): type(TYPE_INT) { var.i = x; }
	explicit TypeAdapter(double x): type(TYPE_DOUBLE) { var.d = x; }
	explicit TypeAdapter(std::string const &x): type(TYPE_STRING) { s = x; }
#ifndef SWIG
	explicit TypeAdapter(const char *x): type(TYPE_STRING) { s = x; }
#endif // SWIG

#ifdef SWIG
#  define BASETYPE_ADAPTER(BASETYPE) \
	explicit TypeAdapter(CSP(BASETYPE) const &x): type(TYPE_##BASETYPE) { var.o = reinterpret_cast<void const*>(&x); }
#else
#  define BASETYPE_ADAPTER(BASETYPE) \
	explicit TypeAdapter(BASETYPE const *x): type(TYPE_##BASETYPE) { var.o = reinterpret_cast<void const*>(x); } \
	explicit TypeAdapter(BASETYPE const &x): type(TYPE_##BASETYPE) { var.o = reinterpret_cast<void const*>(&x); }
#endif // SWIG

	BASETYPE_ADAPTER(ECEF);
	BASETYPE_ADAPTER(EnumLink);
	BASETYPE_ADAPTER(External);
	BASETYPE_ADAPTER(Key);
	BASETYPE_ADAPTER(LinkCore);
	BASETYPE_ADAPTER(LinkBase);
	BASETYPE_ADAPTER(LLA);
	BASETYPE_ADAPTER(Matrix3);
	BASETYPE_ADAPTER(Object);
	BASETYPE_ADAPTER(Path);
	BASETYPE_ADAPTER(Quat);
	BASETYPE_ADAPTER(Real);
	BASETYPE_ADAPTER(SimDate);
	BASETYPE_ADAPTER(Table1);
	BASETYPE_ADAPTER(Table2);
	BASETYPE_ADAPTER(Table3);
	BASETYPE_ADAPTER(UTM);
	BASETYPE_ADAPTER(Vector3);

#undef BASETYPE_ADAPTER

#ifndef SWIG
	TypeAdapter(TypeAdapter const &x): type(x.type) {
		switch(type) {
			case TYPE_INT:
				var.i = x.var.i;
				break;
			case TYPE_DOUBLE:
				var.d = x.var.d;
				break;
			case TYPE_STRING:
				s = x.s;
				break;
			default:
				var.o = x.var.o;
				break;
		}
	}

	// objects

	int getInteger() const { IntCheck(); return var.i; }
	double getFloatingPoint() const { DoubleCheck(); return var.d; }
	std::string getString() const { StringCheck(); return s; }
	
	template <typename T>
	void setCoordinate(T & x) const {
		assert(var.o);
		if (type == TYPE_LLA) {
			x = *reinterpret_cast<LLA const *>(var.o);
		} else if (type == TYPE_UTM) {
			x = *reinterpret_cast<UTM const *>(var.o);
		} else if (type == TYPE_ECEF) {
			x = *reinterpret_cast<ECEF const *>(var.o);
		} else {
			TypeCheck(false, "Setting non-geopos type {LLA,UTM,ECEF} in TypeAdapter::setCoordinate");
		}
	}

	void set(SimDate & x) const;

	void set(LLA & x) const;
	void set(UTM & x) const;
	void set(ECEF & x) const;

	void set(Vector3 & x) const;
	void set(Matrix3 & x) const;
	void set(Quat & x) const;
	void set(Real & x) const;
	void set(Table1 & x) const;
	void set(Table2 & x) const;
	void set(Table3 & x) const;
	void set(External & x) const;
	void set(Key & x) const;
	void set(Path & x) const;

	void set(EnumLink &x) const;
	void set(LinkBase &x) const;

	void set(short &x) const { IntCheck(); x = static_cast<short>(var.i); }
	void set(char &x) const { IntCheck(); x = static_cast<char>(var.i); }
	void set(int &x) const { IntCheck(); x = static_cast<int>(var.i); }
	void set(bool &x) const { IntCheck(); x = (var.i != 0); }
	void set(float &x) const { DoubleCheck(); x = static_cast<float>(var.d); }
	void set(double &x) const { DoubleCheck(); x = static_cast<double>(var.d); }
	void set(unsigned int &x) const { IntCheck(); x = static_cast<unsigned int>(var.i); }
	void set(std::string &x) const { StringCheck(); x = s; }

	// Implemented in Link.h
	template <typename Q>
	inline void set(Link<Q> &x) const;

#endif // SWIG

	const std::string __repr__() const {
		return std::string("TypeAdapter<") + TypeNames[type] + ">";
	}

	const std::string __str__() const;

private:
	TYPE type;
	// can't keep std::string in the union unfortunately, since it
	// has a non-default constructor.
	std::string s;
	// basic types and all object pointers should go in the union.
	union {
		int i;
		double d;
		void const *o;  // for BaseTypes
	} var;

	bool isType(TYPE t) const { return type==t; }

	void failConvert(const char *typestr) const;

	void TypeCheck(bool test, std::string msg) const {
		if (!(test)) {
			msg = __repr__() + ": " + msg;
			throw TypeMismatch(msg);
		}
	}
	void IntCheck() const {
		TypeCheck(type==TYPE_INT, "integer type expected");
	}
	void StringCheck() const {
		TypeCheck(type==TYPE_STRING, "string type expected");
	}
	void DoubleCheck() const {
		TypeCheck(type==TYPE_DOUBLE, "floating point type expected");
	}

	template <typename T> void setBase(T&, TYPE) const;
};

CSPLIB_EXPORT std::ostream &operator <<(std::ostream &o, TypeAdapter const &t);


CSP_NAMESPACE_END


#endif // __CSPLIB_DATA_TYPEADAPTER_H__

