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
 * @file Enum.h
 * @author Mark Rose <mrose@stm.lbl.gov>
 */


#ifndef __ENUM_H__
#define __ENUM_H__

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#pragma warning (disable : 4786)
# endif

#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

#include <SimData/BaseType.h>
#include <SimData/HashUtility.h>


NAMESPACE_SIMDATA

class Packer;
class UnPacker;

/*
 * Examples:
 *
 * Enumeration Registers("EAX EBX ECX EDX");
 *
 * Enum reg(Registers);
 *
 * reg = "EAX";
 * if (reg == "EBX") reg.cycle();
 * Enum::iterator i;
 * for (i = Registers.begin(); i != Registers.end(); i++) if (*i==reg) break;
 *
 */

/**
 * Base class for Enum exceptions.
 */
class EnumError {
protected:
	std::string _msg;
	bool _armed;
public:
	EnumError(const std::string s=""): _msg(s), _armed(true) {}
	void disarm() { _armed = false; }
	~EnumError() { if (_armed) std::cerr << _msg << std::endl; }
	const std::string& getError() { return _msg; }
};


/**
 * Enum exception for values not defined in the associated enumeration set.
 */
class EnumIndexError: public EnumError {
public:
	EnumIndexError(const std::string s=""): EnumError() { _msg = "Enumeration::IndexError: " + s; }
};


/**
 * Enum exception for errors related to mixing multiple enumeration sets.
 */
class EnumTypeError: public EnumError {
public:
	EnumTypeError(const std::string s=""): EnumError() { _msg = "Enumeration::TypeError: " + s; }
};



class Enumeration;


/**
 * A class representing a particular value from an associated enumeration set.
 */
class Enum: public BaseType {
	friend class Enumeration;
	Enum(const Enumeration& parent, const std::string &s, int idx);
protected:
	const Enumeration* _parent;
	std::string _name;
	int _id;
	void __assign(const Enum* i);
public:
	Enum();
	Enum(const Enumeration& parent, int idx=0);
	Enum(const Enumeration& parent, const std::string &s);
	virtual ~Enum();
#ifndef SWIG
	const std::string& operator=(const std::string& s);
	const Enum& operator=(const Enum& i);
#endif // SWIG
	int asInt() const;
	std::string asString() const;
	void cycle();
	void cycleBack();
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	virtual void parseXML(const char* cdata);
	std::string __repr__();
	
	bool __eq__(const std::string& x);
	bool __ne__(const std::string& x);
	bool __le__(const std::string& x);
	bool __ge__(const std::string& x);
	bool __lt__(const std::string& x);
	bool __gt__(const std::string& x);
	bool __eq__(const Enum& x);
	bool __ne__(const Enum& x);
	bool __le__(const Enum& x);
	bool __ge__(const Enum& x);
	bool __lt__(const Enum& x);
	bool __gt__(const Enum& x);
	void fromString(const std::string& s) { *this = s; }
	void fromEnum(const Enum& e) { *this = e; }
	Enum __copy__() { return Enum(*_parent, _id); }
	
};



/**
 * An enumeration class representing a set of possible values indexed by strings.
 */
class Enumeration {
	string_map _map;
	std::vector<Enum> _enums;
	std::vector<std::string> _strings;
	Enumeration() { }
public:
	explicit Enumeration(const char* items);
	const std::vector<Enum> each() const;
	const std::vector<std::string> eachString() const;
	const std::vector<std::string> getLabels() const;
	int size() const;
	int _getID(const std::string&s) const;
	const std::string& _getName(int idx) const;
#ifndef SWIG
	const Enum& operator[](int n) const;
	const Enum& operator[](const std::string& s) const;
#endif // SWIG
	const std::vector<Enum> irange(const Enum& a, const Enum& b) const;
	bool contains(const Enum& x) const;
	bool contains(int x) const;
	bool contains(const std::string& x) const;
	
	// Python special methods
	const std::vector<Enum> __getslice__(int a, int b) const;
	int __len__() const;
	bool __contains__(const Enum& i) const;
	bool __contains__(int i) const;
	bool __contains__(const std::string& i) const;
	const Enum& __getitem__(int n) const;
	const Enum& __getitem__(const std::string& s) const;
	const Enum& __getattr_c__(const std::string& s) const;

	std::string asString() const { return "<simdata::Enumeration>"; }
	
/* 
 * Allow enumeration values to be indexed as instance variables in
 * Python.
 */
#ifdef SWIG
	%insert("shadow") %{
	def __getattr__(*args): 
		self, name = args
		if name in ["this", "thisown", "__str__"]: 
			return _swig_getattr(self, Enumeration, name)
		return apply(_cSimData.Enumeration___getattr_c__,args)
	%}
#endif // SWIG
	
};


/*
 * Equality and inequality operators are already defined as
 * specialized methods for Python.  These are just for C++.
 */
#ifndef SWIG
inline bool operator==(const Enum& a, const std::string &b) {
	return a.asString() == b;
}		
inline bool operator!=(const Enum& a, const std::string &b) {
	return a.asString() != b;
}		
inline bool operator==(const Enum& a, const Enum& b) {
	return a.asString() == b.asString();
}		
inline bool operator!=(const Enum& a, const Enum& b) {
	return a.asString() != b.asString();
}		
#endif // SWIG


NAMESPACE_END // namespace simdata

#endif //__ENUM_H__
