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
#include <map>

#include <SimData/BaseType.h>


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
class SIMDATA_EXPORT EnumError {
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
class SIMDATA_EXPORT EnumIndexError: public EnumError {
public:
	EnumIndexError(const std::string s=""): EnumError() { _msg = "Enumeration::IndexError: " + s; }
};


/**
 * Enum exception for errors related to mixing multiple enumeration sets.
 */
class SIMDATA_EXPORT EnumTypeError: public EnumError {
public:
	EnumTypeError(const std::string s=""): EnumError() { _msg = "Enumeration::TypeError: " + s; }
};



class Enumeration;
class EnumerationCore;


/**
 * A class representing a particular value from an associated enumeration set.
 * 
 * Enums are bound to Enumeration objects (which act as ordered sets).  Enums 
 * can assume any value in their parent Enumeration.  The each value has both
 * a numeric and a string representation.
 */
class SIMDATA_EXPORT Enum: public BaseType {
friend class Enumeration;
friend class EnumerationCore;
private:
	Enum(const EnumerationCore& parent, const std::string &s, int idx);
	const EnumerationCore* _core;
protected:
	std::string _name;
	int _id;
	void __assign(const Enum* i);
public:
	/**
	 * Create an unbound Enum.
	 */
	Enum();
	
	/**
	 * Copy constructor.
	 */
	Enum(const Enum &);
	
	/**
	 * Create an Enum bound to an Enumeration.
	 *
	 * @param parent the associated Enumeration object.
	 * @param idx the numerical index of the value in the Enumeration.
	 */
	Enum(const Enumeration& parent, int idx=0);

	/**
	 * Create an Enum bound to an Enumeration.
	 *
	 * @param parent the associated Enumeration object.
	 * @param s the string representing a value in the Enumeration.
	 */
	Enum(const Enumeration& parent, const std::string &s);

	/**
	 * Destructor.
	 */
	virtual ~Enum();
	
#ifndef SWIG
	/**
	 * Assign from a string.
	 *
	 * @param s must be one of the strings in the parent Enumeration.
	 */
	const std::string& operator=(const std::string& s);

	/**
	 * Assign from another Enumeration.
	 *
	 * If this Enum is unbound, it will assume the parent of the assigned
	 * Enum.  If both Enums are bound, they must have the same parent 
	 * Enumeration.
	 */
	const Enum& operator=(const Enum&);
	
	/**
	 * Assign from an integer index.
	 *
	 * @param x must be in the range of the parent Enumeration.
	 */
	int operator=(int x);
#endif // SWIG

	/**
	 * Get the numeric index of this Enum in its parent Enumeration.
	 */
	int asInt() const;

	/**
	 * Get the string representation of this Enum.
	 */
	std::string asString() const;

	/**
	 * Set this Enum to the next value in its parent Enumeration, wrapping
	 * back to zero if necessary.
	 */
	void cycle();
	
	/**
	 * Set this Enum to the previous value in its parent Enumeration, wrapping
	 * to the end if necessary.
	 */
	void cycleBack();

	/**
	 * Serialize to a data archive.
	 */
	virtual void pack(Packer&) const;
	
	/**
	 * Deserialize from a data archive.
	 */
	virtual void unpack(UnPacker&);

	/**
	 * For internal use by the XML parser.
	 */
	virtual void parseXML(const char* cdata);

	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator>(std::string const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator<(std::string const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator>=(std::string const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator<=(std::string const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator==(std::string const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator!=(std::string const &x) const;
	
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator>(int x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator<(int x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator>=(int x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator<=(int x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator==(int x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator!=(int x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator>(Enum const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator<(Enum const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator>=(Enum const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator<=(Enum const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator==(Enum const &x) const;
	/**
	 * Compare with the index of another Enumeration element.
	 */
	bool operator!=(Enum const &x) const;

	/**
	 * Assignment helper for Python
	 */
	void fromString(const std::string& s) { *this = s; }

	/**
	 * Assignment helper for Python
	 */
	void fromEnum(const Enum& e) { *this = e; }
	
	/**
	 * Assignment helper for Python
	 */
	void fromInt(int x) { *this = x; }
	
};

// Python special methods
#ifdef SWIG
%extend Enum {
	/**
	 * String representation for Python
	 */
	std::string __repr__() { return self->asString(); }
	/**
	 * Shallow copy operation for Python
	 */
	//Enum __copy__() { return SIMDATA(Enum)(*(self->getParent()), self->asInt()); }
}
#endif


class Enumeration;


/**
 * class EnumerationCore
 *
 * This class is used internally by both Enum and Enumeration to store
 * the string and integer indices of the enumeration.  It is reference
 * counted so that it will persist as long as any outstanding Enums
 * point to it, even after the main Enumeration object has been finalized.
 */
class EnumerationCore {
friend class Enumeration;
friend class Enum;
private:
	typedef std::map<std::string, int> string_map;
	string_map _map;
	std::vector<std::string> _strings;
	mutable int _refs;
	
	/**
	 * Construct an empty Enumeration core.
	 */
	EnumerationCore(): _refs(1) {}

	/**
	 * Finalize the object and free internal storage.
	 */
	~EnumerationCore();

	/**
	 * Construct a new EnumerationCore.
	 *
	 * This should only be called by the Enumeration constructor.
	 *
	 * @param enum_str a space-separated string of enumeration labels.
	 * @param enums storage for the newly constructed Enums
	 */
	EnumerationCore(const std::string &enum_str, std::vector<Enum> &enums);

	/**
	 * Increase reference count
	 */
	void ref() const { _refs++; }

	/** 
	 * Decrease reference count
	 *
	 * @return true if reference count is zero
	 */
	bool deref() const { _refs--; return (_refs <= 0); }
	
	/**
	 * Get the string to index map.
	 */
	string_map const &map() const { return _map; }

	/**
	 * Get the index to string map (vector).
	 */
	std::vector<std::string> const &strings() const { return _strings; }

	/**
	 * Return the number of items in the Enumeration.
	 */
	int size() const { return _strings.size(); }
	
	/**
	 * Get the index of an element in the Enumeration by its string 
	 * identifier.
	 */
	
	int getID(const std::string&) const;
	/**
	 * Get the string identifier corresponding to a particular element
	 * in the Enumeration.
	 */
	const std::string& getName(int idx) const;
};

/**
 * class Enumeration
 *
 * An enumeration class representing a ordered set of possible values 
 * indexed by strings or integers.
 */
class SIMDATA_EXPORT Enumeration {
friend class Enum;
private:
	/*
	typedef std::map<std::string, int> string_map;
	string_map _map;
	std::vector<Enum> _enums;
	std::vector<std::string> _strings;
	*/
	std::vector<Enum> _enums;
	EnumerationCore *_core;
	Enumeration(): _core(0) { }

protected:
	EnumerationCore *getCore() { return _core; }

public:
	/** 
	 * Create an enumeration from a space-separated string of values.
	 *
	 * @param items space-separated list (e.g. "ONE TWO THREE")
	 */
	explicit Enumeration(const char* items);
	
	/**
	 * Finalizer
	 */
	~Enumeration();
	
	/**
	 * Return an ordered vector of Enums in this Enumeration.
	 */
	const std::vector<Enum> each() const;
	
	/**
	 * Return an ordered vertor of string values in this Enumeration.
	 */
	const std::vector<std::string> eachString() const;
	
	/**
	 * Return an ordered vertor of string values in this Enumeration.
	 */
	const std::vector<std::string> getLabels() const;
	
	/**
	 * Return the number of items in the Enumeration.
	 */
	int size() const;
	
	/**
	 * Get the index of an element in the Enumeration by its string 
	 * identifier.
	 */
	int _getID(const std::string&) const;
	
	/**
	 * Get the string identifier corresponding to a particular element
	 * in the Enumeration.
	 */
	const std::string& _getName(int idx) const;

#ifndef SWIG
	/**
	 * Get the Enum representing an element by its integer index.
	 */
	const Enum& operator[](int n) const;
	/**
	 * Get the Enum representing an element by its string identifier.
	 */
	const Enum& operator[](const std::string& s) const;
#endif // SWIG
	
	/**
	 * Get a list of all Enums in the range between two Enums 
	 * (inclusive).
	 *
	 * @param a the first Enum in the returned list
	 * @param b the last Enum in the returned list
	 */
	const std::vector<Enum> irange(const Enum& a, const Enum& b) const;

	/**
	 * Get a list of all Enums in the range between two indices
	 * (non-inclusive).
	 *
	 * @param a index of first element (negative counts from the end)
	 * @param b the list will include items up to, but not including,
	 * this index (negative counts from the end)
	 */
	const std::vector<Enum> range(int a, int b) const;

	/**
	 * Test for inclusion in the Enumeration set.
	 */
	bool contains(const Enum& x) const;

	/**
	 * Test for inclusion in the Enumeration set.
	 */
	bool contains(int x) const;
	
	/**
	 * Test for inclusion in the Enumeration set.
	 */
	bool contains(const std::string& x) const;
	
	/**
	 * Simple string identifier for Enumerations
	 */
	std::string asString() const { return "<simdata::Enumeration>"; }
	
#ifdef SWIG
	// Allow enumeration values to be indexed as instance variables in
	// Python.
	%insert("shadow") %{
	def __getattr__(*args): 
		self, name = args
		if name.startswith('__') or name in ("this", "thisown"):
			return _swig_getattr(self, Enumeration, name)
		return apply(_cSimData.Enumeration___getattr_c__,args)
	%}
#endif // SWIG
	
};

#ifdef SWIG
%extend Enumeration {
	// translation for various Python special functions
	std::string __repr__() { return self->asString(); }
	int __len__() const { return self->size(); }
	bool __contains__(const Enum& i) const { return self->contains(i); }
	bool __contains__(int i) const { return self->contains(i); }
	bool __contains__(const std::string& i) const { return self->contains(i); }
	const Enum __getitem__(int n) const { return self->operator[](n); }
	const Enum __getitem__(const std::string& s) const { return self->operator[](s); }
	const Enum __getattr_c__(const std::string& s) const { return self->operator[](s); }
	const std::vector<Enum> __getslice__(int a, int b) const {
		return self->range(a, b);
	}
}
#endif // SWIG
	


NAMESPACE_END // namespace simdata

#endif //__ENUM_H__
