/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file TypeAdapter.h
 * @brief Adapter class providing uniform access to built-in types
 *   and BaseType subclasses.
 */


#ifndef __SIMDATA_TYPEADAPTER_H__
#define __SIMDATA_TYPEADAPTER_H__

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif

#include <SimData/Export.h>
#include <SimData/ExceptionBase.h>
#include <SimData/Namespace.h>
#include <SimData/Enum.h>
#include <SimData/Path.h>

//#include <SimData/Types.h>
#include <SimData/Link.h>
#include <SimData/PTS.h>

#include <string>
#include <iostream>


NAMESPACE_SIMDATA

class BaseType;
class Real;
class Vector3;
class Matrix3;
class Curve;
class Table;
class LinkBase;
class External;
class Key;
class SimDate;
class ListBase;
class GeoPos;
class LLA;
class UTM;
class ECEF;
class Object;

#ifndef __PTS_SIM__
template <int N, typename X>
class LUT;

typedef LUT<1,float> Table1;
typedef LUT<2,float> Table2;
typedef LUT<3,float> Table3;
#endif // __PTS_SIM__

SIMDATA_EXCEPTION(TypeMismatch)

	
/** Dynamically typed wrapper for basic types and objects.
 *
 *  For internal use only.  This class is used to pass typed data as
 *  parameters to virtual member functions (which are not compatible
 *  with templates).  It works for a number of basic types, and for
 *  objects derived from the Object base class.
 *
 *  @author Mark Rose <mrose@stm.lbl.gov>
 *  @internal
 */
class SIMDATA_EXPORT TypeAdapter
{
public:
	typedef enum {NONE, INT, DOUBLE, STRING, BASE} TYPE;
	static const char * TypeNames[];

	TypeAdapter(): type(NONE) {}
	explicit TypeAdapter(int x): type(INT) { var.i = x; }
	explicit TypeAdapter(double x): type(DOUBLE) { var.d = x; }
	explicit TypeAdapter(std::string const &x): type(STRING) { s = x; }
	explicit TypeAdapter(const char *x): type(STRING) { s = x; }
	explicit TypeAdapter(BaseType const *x): type(BASE) { var.o = x; }
	explicit TypeAdapter(BaseType const &x): type(BASE) { var.o = &x; }
	TypeAdapter(TypeAdapter const &x): type(x.type) {
		switch(type) {
			case INT:
				var.i = x.var.i;
				break;
			case DOUBLE:
				var.d = x.var.d;
				break;
			case STRING:
				s = x.s;
				break;
			case BASE:
				var.o = x.var.o;
				break;
			default:
				break;
		}
	}

	// objects

	int getInteger() const { IntCheck(); return var.i; }
	double getFloatingPoint() const { DoubleCheck(); return var.d; }
	std::string getString() const { StringCheck(); return s; }
	BaseType const &getBaseType() const { BaseCheck(); return *(var.o); }
	
	template<typename T>
	void getBaseTypeAs(T * &t) const {
		T proto;
		BaseCheck();
		T const *cp = dynamic_cast<T const *>(var.o);
		TypeCheck(cp!=NULL, "dynamic cast of BaseType* to " + proto.typeString() +
				    "failed in TypeAdapter::getBaseTypeAs");
		t = const_cast<T *>(cp);
	}

	template <typename T>
	void setBase(T & x) const {
		BaseCheck();
		T const *p = dynamic_cast<T const *>(var.o);
		TypeCheck(p!=NULL, "dynamic cast of BaseType* to " + x.typeString() +
				   " failed in TypeAdapter::setBase");
		T *nc = const_cast<T *>(p);
		x = *nc;
	}

	template <typename T>
	void setCoordinate(T & x) const {
		BaseCheck();
		LLA const *lla = dynamic_cast<LLA const *>(var.o);
		if (lla != 0) {
			x = *lla;
			return;
		}
		UTM const *utm = dynamic_cast<UTM const *>(var.o);
		if (utm != 0) {
			x = *utm;
			return;
		}
		ECEF const *ecef = dynamic_cast<ECEF const *>(var.o);
		TypeCheck(ecef!=NULL, "dynamic casts of BaseType* to {LLA,UTM,ECEF} failed in TypeAdapter::setCoordinate");
		x = *ecef;
	}

	/*
	template <typename T>
	void set(T & x) const { setBase(x); }
	*/

	void set(SimDate & x) const;

	void set(GeoPos & x) const;

	void set(LLA & x) const;
	void set(UTM & x) const;
	void set(ECEF & x) const;

	void set(Vector3 & x) const;
	void set(Matrix3 & x) const;
	void set(Real & x) const;
	void set(Curve & x) const;
	void set(Table & x) const;
#ifndef __PTS_SIM__
	void set(Table1 & x) const;
	void set(Table2 & x) const;
	void set(Table3 & x) const;
#endif // __PTS_SIM__
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

	template <typename Q>
	void set(Link<Q> &x) const {
		// first try to assign as an object reference
		Q const *q = dynamic_cast<Q const*>(var.o);
		if (q != 0) {
			x = const_cast<Q*>(q);
		} else {
			// if not, try as a pointerbase or path
			set((LinkBase &)x);
		}
	}
			
	bool isType(TYPE t) const { return type==t; }

	const std::string __repr__() const {
		std::string repr;
		if (type==BASE) {
			if (var.o == 0) {
				repr += "BaseType, NULL";
			} else {
				repr += "BaseType, " + var.o->typeString();
			}
		} else {
			repr += TypeNames[type];
		}
		return "TypeAdapter<" + repr + ">";
	}

private:
	TYPE type;
	// can't keep std::string in the union unfortunately, since it
	// has a non-default constructor.
	std::string s;
	// basic types and all object pointers should go in the union.
	union {
		int i;
		double d;
		BaseType const *o;
	} var;

	void TypeCheck(bool test, std::string msg) const throw(TypeMismatch) {
		if (!(test)) {
			msg = __repr__() + ": " + msg;
			throw TypeMismatch(msg);
		}
	}
	void IntCheck() const {
		TypeCheck(type==INT, "integer type expected");
	}
	void StringCheck() const {
		TypeCheck(type==STRING, "string type expected");
	}
	void DoubleCheck() const {
		TypeCheck(type==DOUBLE, "floating point type expected");
	}
	void BaseCheck() const {
		TypeCheck(type==BASE, "base type expected");
	}

};

std::ostream &operator <<(std::ostream &o, TypeAdapter const &t);


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_TYPEADAPTER_H__

