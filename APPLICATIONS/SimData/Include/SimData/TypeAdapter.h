/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
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
 */


#ifndef __SIMDATA_TYPEADAPTER_H__
#define __SIMDATA_TYPEADAPTER_H__

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#pragma warning (disable : 4290)
#endif

#include <SimData/Export.h>
#include <SimData/Exception.h>
#include <SimData/Namespace.h>
#include <SimData/Enum.h>
#include <SimData/Path.h>

#include <SimData/Types.h>
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

	
/**
 * class TypeAdapter - Dynamically typed wrapper for basic types and objects.
 * 
 * For internal use only.  This class is used to pass typed data as
 * parameters to virtual member functions (which are not compatible
 * with templates).  It works for a number of basic types, and for
 * objects derived from the Object base class. 
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
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

	/*
	 * XXX 
	 * These implicit casts are not currently used.  The templated
	 * versions, in particular, are rather dangerous (and break under
	 * GCC-3.3).  After a short testing period this cast code can be
	 * be completely removed.  --MR 7/03
	 
	template <typename T>
	operator T const &() const { 
		BaseCheck();
		T const *p = dynamic_cast<T const *>(var.o);
		TypeCheck(p!=NULL, "dynamic cast of BaseType* failed");
		return *p;
	}
	
	template <typename T>
	operator T &() const { 
		BaseCheck();
		T const *p = dynamic_cast<T const *>(var.o);
		TypeCheck(p!=NULL, "dynamic cast of BaseType* failed");
		T *nc = const_cast<T *>(p);
		return *nc;
	}
	
	// basic types
	operator int const() const { IntCheck(); return var.i; }

	// can add range checking if desired
	operator short int const() const { IntCheck(); return (short int)var.i; }
	operator unsigned int const() const { IntCheck(); return (unsigned int)var.i; }
	operator unsigned short int const() const { IntCheck(); return (unsigned short int)var.i; }
	operator char const() const { IntCheck(); return (char)var.i; }
	operator unsigned char const() const { IntCheck(); return (unsigned char)var.i; }

	operator double const() const { DoubleCheck(); return var.d; }
	operator float const() const { DoubleCheck(); return static_cast<float>(var.d); }

	operator std::string const() const { StringCheck(); return s; }
	*/

	int getInteger() const { IntCheck(); return var.i; }
	double getFloatingPoint() const { DoubleCheck(); return var.d; }
	std::string getString() const { StringCheck(); return s; }
	BaseType const &getBaseType() const { BaseCheck(); return *(var.o); }
	
	template<typename T>
	void getBaseTypeAs(T * &t) const {
		BaseCheck();
		T const *cp = dynamic_cast<T const *>(var.o);
		TypeCheck(cp!=NULL, "dynamic cast of BaseType* failed in TypeAdapter::getBaseTypeAs");
		t = const_cast<T *>(cp);
	}

	template <typename T> 
	void setBase(T & x) const {
		BaseCheck();
		T const *p = dynamic_cast<T const *>(var.o);
		TypeCheck(p!=NULL, "dynamic cast of BaseType* failed in TypeAdapter::setBase");
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
		TypeCheck(ecef!=NULL, "dynamic cast of BaseType* failed in TypeAdapter::setCoordinate");
		x = *ecef;
	}

	/*
	template <typename T> 
	void set(T & x) const { setBase(x); }
	*/

	inline void set(SimDate & x) const { setBase(x); }

	inline void set(GeoPos & x) const { setBase(x); }

	inline void set(LLA & x) const { setCoordinate(x); }
	inline void set(UTM & x) const { setCoordinate(x); }
	inline void set(ECEF & x) const { setCoordinate(x); }

	inline void set(Vector3 & x) const { setBase(x); }
	inline void set(Matrix3 & x) const { setBase(x); }
	inline void set(Real & x) const { setBase(x); }
	inline void set(Curve & x) const { setBase(x); }
	inline void set(Table & x) const { setBase(x); }
#ifndef __PTS_SIM__
	inline void set(Table1 & x) const { setBase(x); }
	inline void set(Table2 & x) const { setBase(x); }
	inline void set(Table3 & x) const { setBase(x); }
#endif // __PTS_SIM__
	inline void set(External & x) const { setBase(x); }
	inline void set(Key & x) const { setBase(x); }
	inline void set(Path & x) const { setBase(x); }
	// list

	void set(short &x) const { IntCheck(); x = static_cast<short>(var.i); }
	void set(char &x) const { IntCheck(); x = static_cast<char>(var.i); }

	void set(int &x) const { IntCheck(); x = static_cast<int>(var.i); }
	void set(bool &x) const { IntCheck(); x = (var.i != 0); }
	void set(float &x) const { DoubleCheck(); x = static_cast<float>(var.d); }
	void set(double &x) const { DoubleCheck(); x = static_cast<double>(var.d); }
	void set(unsigned int &x) const { IntCheck(); x = static_cast<unsigned int>(var.i); }
	void set(std::string &x) const { StringCheck(); x = s; }
	void set(EnumLink &x) const { if (isType(STRING)) x = s; else setBase(x); }
	// slightly fancier handling required for path pointers
	void set(LinkBase &x) const {
		BaseCheck();
		// are we assigning to a pointerbase?
		LinkBase const *p = dynamic_cast<LinkBase const *>(var.o);
		if (p != 0) {
			x = *(const_cast<LinkBase *>(p));
		} else {
			// last chance, is it a path?
			Path const *p = dynamic_cast<Path const *>(var.o);
			TypeCheck(p!=NULL, "dynamic cast of BaseType* to LinkBase failed");
			x = LinkBase(*(const_cast<Path *>(p)), 0);
		}
	}
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

	const std::string __repr__() const { return std::string("TypeAdapter<") + TypeNames[type] + ">"; }

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
		if (!(test)) throw TypeMismatch(__repr__() + ": " + msg);
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

