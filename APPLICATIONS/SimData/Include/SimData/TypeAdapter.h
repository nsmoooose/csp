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
 * @file TypeAdapter.h
 */


#ifndef __TYPEADAPTER_H__
#define __TYPEADAPTER_H__

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning (disable : 4290 4786)
#endif

#include <string>
#include <iostream>

#include <SimData/Export.h>
#include <SimData/Exception.h>
#include <SimData/ns-simdata.h>


NAMESPACE_SIMDATA

class BaseType;
class Spread;
class Vector3;
class Matrix3;
class Curve;
class Table;
class PathPointerBase;
class External;
class Enum;
class SimDate;
class ListBase;
class Object;


EXCEPTION(TypeMismatch)

	
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
 
class TypeAdapter 
{
public:
	typedef enum {NONE, INT, DOUBLE, STRING, BASE} TYPE; 
	static const char * TypeNames[];

	TypeAdapter(): type(NONE) {}
	TypeAdapter(int x): type(INT) { var.i = x; }
	TypeAdapter(double x): type(DOUBLE) { var.d = x; }
	TypeAdapter(std::string const &x): type(STRING) { s = x; }
	TypeAdapter(const char *x): type(STRING) { s = x; }
	TypeAdapter(BaseType const *x): type(BASE) { var.o = x; }
	TypeAdapter(BaseType const &x): type(BASE) { var.o = &x; }

	// objects

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

	int getInteger() const { IntCheck(); return var.i; }
	double getFloatingPoint() const { DoubleCheck(); return var.d; }
	std::string getString() const { StringCheck(); return s; }
	BaseType const &getBaseType() const { BaseCheck(); return *(var.o); }
	
	template<typename T>
	void getBaseTypeAs(T * &t) const {
		BaseCheck();
		T const *cp = dynamic_cast<T const *>(var.o);
		TypeCheck(cp!=NULL, "dynamic cast of BaseType* failed");
		t = const_cast<T *>(cp);
	}

	template <typename T> 
	void setBase(T & x) const {
		BaseCheck();
		T const *p = dynamic_cast<T const *>(var.o);
		TypeCheck(p!=NULL, "dynamic cast of BaseType* failed");
		T *nc = const_cast<T *>(p);
		x = *nc;
	}
	void set(int &x) const;
	void set(bool &x) const;
	void set(float &x) const;
	void set(double &x) const;
	void set(unsigned int &x) const;
	void set(std::string &x) const;
	void set(SimDate &x) const;
	void set(Vector3 &x) const;
	void set(Matrix3 &x) const;
	void set(Curve &x) const;
	void set(Table &x) const;
	void set(PathPointerBase &x) const;
	void set(External &x) const;
	void set(Spread &x) const;
	void set(Enum &x) const;
	void set(ListBase &x) const;
	void set(Object &x) const;
	
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


NAMESPACE_END // namespace simdata


#endif // __TYPEADAPTER_H__

