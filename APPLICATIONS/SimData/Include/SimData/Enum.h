 /* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
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
 * @file Enum.h
 * @author Mark Rose <mrose@stm.lbl.gov>
 *
 * Enumeration classes for C++ and Python.
 *
 * Examples:
 
 	// C++ example /////////////////////////////////////////////////////
	
	#include <SimData/Enum.h>
	class Foo {
	public:
		static const simdata::Enumeration Items;
		simdata::Enum<Items> m_Snack;
		void eat();
		Foo(): m_Snack("cherry") { ... }
		...
	};
	// in the corresponding .cpp file:
	simdata::Enumeration Foo::Items("apple orange=2 cherry");
	void Foo::eat() {
		std::cout << "Eating one " << m_Snack.getToken() << std::endl;
		if (m_Snack == "cherry") {
			std::cout << "yummy!\n";
			assert(m_Snack == 3);
		}
		m_Snack.cycle();
	}


        # Sample code using Enumerations in Python #########################
	
	import SimData
	items = SimData.Enumeration("apple=0 orange=2 cherry=4")
	x = items.apple
	print x, x.next()
	assert(x == items.cherry.previous(2))
	for item in items.each():
		print item.getToken()
	assert(x.getToken() == items.eachString()[0])
	assert(items.first() == items.last().getValue() - 4)
	assert(items.orange >= 2)
	assert(items.cherry == "cherry")
	assert(items.cherry > "orange")

 */


#ifndef __SIMDATA_ENUM_H__
#define __SIMDATA_ENUM_H__

#include <SimData/Namespace.h>
#include <SimData/BaseType.h>
#include <SimData/Ref.h>
#include <SimData/Exception.h>

#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <iostream>
#include <sstream>



NAMESPACE_SIMDATA


class EnumLink;
class Enumeration;


/**
 * Base class for Enum exceptions.
 */
SIMDATA_EXCEPTION(EnumError);

/**
 * Enum exception for values not defined in the associated enumeration set.
 */
SIMDATA_SUBEXCEPTION(EnumIndexError, EnumError);

/**
 * Enum exception for errors related to mixing multiple enumeration sets.
 */
SIMDATA_SUBEXCEPTION(EnumTypeError, EnumError);



/**
 * Core Enumuration
 *
 * This is an internal class used to store the integer and string
 * indices of the Enumeration.  It is reference counted, and can
 * persist beyond the lifetime of the Enumeration that created it
 * as long as EnumLink's are still using it.
 *
 * You should never create an instance of this class directly;
 * use Enumeration instead.
 */
class EnumerationCore: public Referenced {
friend class Enumeration;
	typedef std::pair<std::string, int> Element;
	typedef std::vector<Element> Elements;
	/// value to index lookup
	std::map<int, int> __i2idx;
	/// string to index lookup
	std::map<std::string, int> __s2idx;
	/// elements
	Elements __elements;
	/// Parse enum string into lookup tables.
	void __init(std::string const &s) {
		std::stringstream ss(s); 
		std::string token; 
		int value = 0;
		for (int idx = 0; ss >> token; idx++) {
			std::size_t eq = token.find("=");
			if (eq != std::string::npos) {
				value = atoi(std::string(token, eq+1, std::string::npos).c_str());
				token = std::string(token, 0, eq);
			}
			if (__i2idx.find(value) != __i2idx.end()) {
				std::stringstream msg;
				msg << "Enumeration value '" << value << "' multiply defined in '" << s << "'";
				throw EnumError(msg.str());
			}
			if (__s2idx.find(token) != __s2idx.end()) {
				std::stringstream msg;
				msg << "Enumeration token '" << token << "' multiply defined in '" << s << "'";
				throw EnumError(msg.str());
			}
			__elements.push_back(Element(token, value));
			__i2idx[value] = idx;
			__s2idx[token] = idx;
			value++;
		}
		if (__elements.size() <= 0) throw EnumError("Empty Enumeration");
	}
	Element const &getElementByIndex(int idx) const {
		assert(validIndex(idx));
		return __elements[idx];
	}
	std::string getTokenByIndex(int idx) const { return getElementByIndex(idx).first; }
	int getValueByIndex(int idx) const { return getElementByIndex(idx).second; }
	int getIndexByValue(int value) const {
		std::map<int, int>::const_iterator iter = __i2idx.find(value);
		if (iter == __i2idx.end()) {
			std::stringstream msg;
			msg << value;
			throw EnumIndexError(msg.str());
		}
		return iter->second;
	}
	std::string getTokenByValue(int value) const {
		return __elements[getIndexByValue(value)].first;
	}
	int getIndexByToken(std::string const &token) const {
		std::map<std::string, int>::const_iterator iter = __s2idx.find(token);
		if (iter == __s2idx.end()) {
			throw EnumIndexError(token);
		}
		return iter->second;
	}
	int getValueByToken(std::string const &token) const {
		return __elements[getIndexByToken(token)].second;
	}
	int size() const { return __elements.size(); }
	bool containsToken(std::string const &token) const {
		return __s2idx.find(token) != __s2idx.end(); 
	}
	bool containsValue(int value) const {
		return __i2idx.find(value) != __i2idx.end(); 
	}
	bool validIndex(int idx) const {
		return idx >= 0 && idx <= size();
	}
	/// Not defined; should never be called.
	EnumerationCore();
	/// Construct a new enum from a white-space-separated string.
	EnumerationCore(std::string const &s): Referenced() {
		__init(s);
	}
};

/**
 * A class representing an ordered set of unique items that
 * can be indexed by number or token.
 *
 * Enumerations are usually created as static const class
 * members, together with Enum<> members that use the 
 * static Enumeration as the template argument.  For example:
 *
 *     class Foo {
 *     public:
 *         static const Enumeration MyEnumeration;
 *         Enum<MyEnumeration> MyEnum;
 *         ...
 *     };
 *
 * When defining the Enumeration, specify the items in order
 * as a white-space separated string, as in:
 *
 *     Enumeration Foo::MyEnumeration("apple orange cherry");
 */
class Enumeration {
friend class EnumLink;
	/// Reference counted smart-pointer to the associated EnumerationCore.
	Ref<EnumerationCore const> __core;

	/// Null constructor used by EnumLink.
	Enumeration() { }

	/// Copy constructor, just share the EnumerationCore.
	Enumeration(Enumeration const &e) {
		__core = e.__core;
		assert(__core.valid()); 
	}

	/// Check if we are unbound (no EnumerationCore).
	bool operator !() const { return !__core; }

	/// Check if we are bound (have an EnumerationCore).
	bool valid() const { return __core.valid(); }

	/// Copy by sharing the EnumerationCore.
	Enumeration const &operator =(Enumeration const &e) {
		__core = e.__core;
		assert(__core.valid()); 
		return *this;
	}

	int getIndexByToken(std::string const &token) const {
		assert(__core.valid());
		return __core->getIndexByToken(token);
	}

	int getIndexByValue(int value) const {
		assert(__core.valid());
		return __core->getIndexByValue(value);
	}

	std::string getTokenByIndex(int idx) const {
		assert(__core.valid());
		return __core->getTokenByIndex(idx);
	}

	int getValueByIndex(int idx) const {
		assert(__core.valid());
		return __core->getValueByIndex(idx);
	}


	inline const std::vector<EnumLink> __range(int idx1, int idx2) const;

	void __checkIndex(int idx) const {
		assert(__core.valid() && __core->validIndex(idx));
	}

	inline EnumLink makeEnum(int idx) const;

public:
	/// Equality of Enumerations means sharing the same EnumerationCore.
	bool operator ==(Enumeration const &e) const { return __core == e.__core; }

	/// Equality of Enumerations means sharing the same EnumerationCore.
	bool operator !=(Enumeration const &e) const { return __core != e.__core; }

	/// Construct a new Enumeration, creating and binding to a new EnumerationCore.
	Enumeration(std::string const &s) {
		__core = new EnumerationCore(s);
	}

	/// Lookup an element's token by its value.
	std::string getToken(int value) const {
		assert(__core.valid());
		return __core->getTokenByValue(value);
	}

	/// Lookup an element's value by its token.
	int getValue(std::string const &token) const {
		assert(__core.valid());
		return __core->getValueByToken(token);
	}

	/// Get the number of items in the Enumeration set.
	int size() const{
		assert(__core.valid());
		return __core->size();
	}

	/// Return a string representation of the Enumeration (used by SWIG/Python).
	std::string __repr__() const {
		assert(__core.valid());
		std::stringstream ss;
		ss << "<Enumeration:";
		for (int i = 0; i < size(); i++) {
			ss << " " << __core->getTokenByIndex(i) 
			   << "=" << __core->getValueByIndex(i);
		}
		ss << ">";
		return ss.str();
	}

	/// Return an ordered vector of Enums in this Enumeration.
	inline const std::vector<EnumLink> each() const;
	
	/// Return an ordered vector of string values in this Enumeration.
	inline const std::vector<std::string> eachToken() const;

	/// Return an ordered vector of string values in this Enumeration.
	inline const std::vector<int> eachValue() const;
	
	/// Get a list of Enums in the range between two Enums (inclusive).
	inline const std::vector<EnumLink> range(const EnumLink& a, const EnumLink& b) const;

	/// Get a list of Enums in the range between two strings (inclusive).
	inline const std::vector<EnumLink> range(std::string const &a, std::string const &b) const;

	/// Get a list of Enums in the range between two values (inclusive).
	inline const std::vector<EnumLink> range(int a, int b) const;

	/// Test for inclusion in the Enumeration set.
	inline bool contains(const EnumLink& x) const;

	/// Test for inclusion in the Enumeration set.
	bool contains(int value) const { 
		assert(__core.valid());
		return __core->containsValue(value);
	}
	
	/// Test for inclusion in the Enumeration set.
	bool contains(const std::string& token) const {
		assert(__core.valid());
		return __core->containsToken(token);
	}

	inline EnumLink last() const;
	inline EnumLink first() const;

#ifndef SWIG
	/// accessor for lookup by value
	inline EnumLink operator[](int value) const;
	/// accessor for lookup by token
	inline EnumLink operator[](std::string const &token) const;
#else
%extend {
	/// value accessor for SWIG/Python (mapped to __getitem__)
	EnumLink get(int idx) { return (*self)[idx]; }
	/// token accessor for SWIG/Python (mapped to __getitem__)
	EnumLink get(std::string const &s) { return (*self)[s]; }
}
	/// Add the __getitem__ operator by hand to map [] indexing
	/// to get(...).  Also overload __getattr__ to allow direct
	/// access to Enumeration set members (e.g. myenumeration.foo)
%insert(shadow) %{
	def __getitem__(self, idx):
		return self.get(idx)
	def __getattr__(self, attr):
		if self.contains(attr):
			return self.get(attr)
		return self.__dict__[attr]
%}
#endif
};




/**
 * EnumLink implements the most of Enum interface and binds to a parent
 * Enumeration.  This class is used internally to return Enum instances
 * from an Enumeration, which is unable to directly create templated Enum 
 * instances.  EnumLinks can also be used to create Enums on the fly by
 * specifying an existing Enumeration in the constructor.  EnumLink's and 
 * Enum templates can be used interchangeably.
 */
class EnumLink: public BaseType {
friend class Enumeration;
	/// the associated enumeration
	Enumeration __E;
protected:
	/// integer index of the enum
	int _idx;

	/// Get the number of items in the associated Enumeration.
	int __size() const { return __E.size(); }

	/// The default constructor creates an unbound EnumLink.
	EnumLink(): __E(), _idx(0) {}

	int getIndex() const { return _idx; }

	EnumLink(int idx, Enumeration const &E): __E(E), _idx(idx) { 
		E.__checkIndex(idx); 
	}

	EnumLink makeEnum(int idx) const {
		return EnumLink(idx, __E);
	}

public:
	// Create a new EnumLink bound to an existing Enumeration.
	EnumLink(Enumeration const &E): __E(E), _idx(0) { }

	// Create a new EnumLink bound to an existing Enumeration.
	EnumLink(Enumeration const &E, int value): __E(E), _idx(0) { 
		_idx = __E.getIndexByValue(value); 
	}

	// create a new EnumLink bound to an existing Enumeration.
	EnumLink(Enumeration const &E, std::string const &token): __E(E), _idx(0) { 
		_idx = __E.getIndexByToken(token); 
	}

	/// Serialize to a data archive.
	virtual void pack(Packer&) const;
	
	/// Deserialize from a data archive.
	virtual void unpack(UnPacker&);

	/// For internal use by the XML parser.
	virtual void parseXML(const char* cdata);
	
	/// Return a type label.
	virtual std::string typeString() const;

	/// Return a string representation.
	virtual std::string asString() const;

	/// copy constructor
	EnumLink(EnumLink const &e): BaseType() {
		__E = e.__E;
		_idx = e._idx;
	}

	/// Cyclically advance the index (default is by one).
	void cycle(int n=1) { _idx = (_idx+n+abs(n)*__size()) % __size(); }

	/// Cyclically reduce the index (default is by one).
	void cycleBack(int n=1) { _idx = (_idx-n+abs(n)*__size()) % __size(); }
	
	/// Set this enum by token.
	void set(std::string const &token) { _idx = __E.getIndexByToken(token); }

	/// Set this enum by integer index.
	void set(int value) { _idx = __E.getIndexByValue(value); }

	/// Get the integer index of this enum.
	int getValue() const { return __E.getValueByIndex(_idx); }

	// Get the token of this enum.
	std::string getToken() const { return __E.getTokenByIndex(_idx); }

	/// Return a string representation (for SWIG/Python).
	std::string __repr__() const { 
		std::stringstream repr;
		repr << "<Enum:" << getToken() << "=" << getValue() << ">"; 
		return repr.str();
	}

	/// Return a new Enum for the next item in the Enumeration (cyclic).
	EnumLink next(int n=1) const { return makeEnum((_idx+n+abs(n)*__size())%__size()); }

	/// Return a new Enum for to the previous item in the Enumeration (cyclic).
	EnumLink prev(int n=1) const { return makeEnum((_idx-n+abs(n)*__size())%__size()); }

	/// Return the last item in the Enumeration.
	EnumLink last() const { return makeEnum(__size()-1); }

	/// Return the first item in the Enumeration.
	EnumLink first() const { return makeEnum(0); }

	/// Return a reference to the associated Enumeration.
	Enumeration const &getEnumeration() const { return __E; }

#ifndef SWIG
	/// Coerce this enum to its integer index.
	operator int() const { return getValue(); }

	/// Coerce this enum to its token.
	operator std::string() const { return getToken(); }

	/// Directly assign by integer index.
	EnumLink const &operator =(int value) { set(value); return *this; }

	/// Directly assign by token.
	EnumLink const &operator =(std::string const &token) { set(token); return *this; }

	/// copy
	EnumLink const &operator=(EnumLink const &e) { 
		if (!__E) {
			__E = e.__E;
		} else {
			if (__E != e.__E) throw EnumTypeError();
		}
		if (__E.valid()) { _idx = e._idx; }
		return *this;
	}
#endif

	/// Compare with the value of another Enumeration element.
	bool operator>(std::string const &token) const { return getValue() > __E.getValue(token); }

	/// Compare with the value of another Enumeration element.
	bool operator<(std::string const &token) const { return getValue() < __E.getValue(token); }

	/// Compare with the value of another Enumeration element.
	bool operator>=(std::string const &token) const { return getValue() >= __E.getValue(token); }

	/// Compare with the value of another Enumeration element.
	bool operator<=(std::string const &token) const { return getValue() <= __E.getValue(token); }

	/// Compare with the value of another Enumeration element.
	bool operator==(std::string const &token) const { return getValue() == __E.getValue(token); }

	/// Compare with the value of another Enumeration element.
	bool operator!=(std::string const &token) const { return getValue() != __E.getValue(token); }
	
	/// Compare with the value of another Enumeration element.
	bool operator>(int value) const { return getValue() > value; }

	/// Compare with the value of another Enumeration element.
	bool operator<(int value) const { return getValue() < value; };

	/// Compare with the value of another Enumeration element.
	bool operator>=(int value) const { return getValue() >= value; };

	/// Compare with the value of another Enumeration element.
	bool operator<=(int value) const { return getValue() <= value; };

	/// Compare with the value of another Enumeration element.
	bool operator==(int value) const { return getValue() == value; };

	/// Compare with the value of another Enumeration element.
	bool operator!=(int value) const { return getValue() != value; };

	/// Compare with the value of another Enumeration element.
	bool operator>(EnumLink const &x) const { return *this > x.getValue(); }

	/// Compare with the value of another Enumeration element.
	bool operator<(EnumLink const &x) const { return *this < x.getValue(); }

	/// Compare with the value of another Enumeration element.
	bool operator>=(EnumLink const &x) const { return *this >= x.getValue(); }

	/// Compare with the value of another EnumLinkeration element.
	bool operator<=(EnumLink const &x) const { return *this <= x.getValue(); }

	/// Compare with the value of another EnumLinkeration element.
	bool operator==(EnumLink const &x) const { return *this == x.getValue(); }

	/// Compare with the value of another EnumLinkeration element.
	bool operator!=(EnumLink const &x) const { return *this != x.getValue(); }
};

/**
 * Enum template.  Use this class to create Enum instances that are bound
 * to an Enumeration during static construction.  This early binding to
 * the variable type has the advantage that the Enumeration associated with
 * an Enum<> member variable can be accessed without instantiating the
 * parent class.  This feature is used by the ObjectInterface to enhance
 * the introspection capabilities of SimData.  An additional advantage is
 * that Enum<> members do not have to be explicitly bound in the class
 * ctor (which is somewhat error prone).
 */
template <Enumeration const &E>
class Enum: public EnumLink {
public:
	Enum(std::string const &token): EnumLink(E, token) {}
	Enum(): EnumLink(E) {}
	Enum(int value): EnumLink(E, value) {}
	Enum(EnumLink const &e): EnumLink(e) {}
#ifndef SWIG
	EnumLink const & operator=(EnumLink const &e) { return EnumLink::operator=(e); }
	EnumLink const &operator =(int value) { return EnumLink::operator=(value); }
	EnumLink const &operator =(std::string &token) { return EnumLink::operator=(token); }
#endif
	/// Return a string representation (for SWIG/Python).
	std::string __repr__() const { return EnumLink::__repr__(); }
};


// inline Enumeration methods

inline EnumLink Enumeration::makeEnum(int idx) const { return EnumLink(idx, *this); }

inline EnumLink Enumeration::operator[](int value) const { return makeEnum(getIndexByValue(value)); }

inline EnumLink Enumeration::operator[](std::string const &token) const { return makeEnum(getIndexByToken(token)); }

inline const std::vector<EnumLink> Enumeration::__range(int idx1, int idx2) const {
	assert(__core.valid());
	std::vector<EnumLink> v;
	if (idx2 >= idx1) {
		v.reserve(idx2-idx1+1);
	}
	for (int idx = idx1; idx <= idx2; idx++) {
		v.push_back(makeEnum(idx));
	}
	return v;
}

inline const std::vector<EnumLink> Enumeration::range(const EnumLink& a, const EnumLink& b) const {
	if (a.__E != *this || b.__E != *this) throw EnumTypeError();
	return __range(a.getIndex(), b.getIndex());
}

inline const std::vector<EnumLink> Enumeration::range(const std::string& a, const std::string& b) const {
	return __range(getIndexByToken(a), getIndexByToken(b));
}

inline const std::vector<EnumLink> Enumeration::range(int a, int b) const {
	return __range(getIndexByValue(a), getIndexByValue(b));
}

inline const std::vector<EnumLink> Enumeration::each() const {
	std::vector<EnumLink> v;
	v.reserve(size());
	for (int idx = 0; idx < size(); idx++) {
		v.push_back(makeEnum(idx));
	}
	return v;
}

inline const std::vector<std::string> Enumeration::eachToken() const {
	assert(__core.valid());
	std::vector<std::string> v;
	v.reserve(size());
	for (int idx = 0; idx < size(); idx++) {
		v.push_back(__core->getTokenByIndex(idx));
	}
	return v;
}

inline const std::vector<int> Enumeration::eachValue() const {
	assert(__core.valid());
	std::vector<int> v;
	v.reserve(size());
	for (int idx = 0; idx < size(); idx++) {
		v.push_back(__core->getValueByIndex(idx));
	}
	return v;
}

inline bool Enumeration::contains(const EnumLink& x) const { return x.__E == *this; }

inline EnumLink Enumeration::last() const {
	assert(__core.valid());
	return makeEnum(size()-1);
}

inline EnumLink Enumeration::first() const {
	assert(__core.valid());
	return makeEnum(0);
}


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_ENUM_H__

