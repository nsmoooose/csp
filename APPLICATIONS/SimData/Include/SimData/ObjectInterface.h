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
 * @file ObjectInterface.h
 */


#ifndef __SIMDATA_OBJECTINTERFACE_H__
#define __SIMDATA_OBJECTINTERFACE_H__


#include <string>
#include <vector>

#include <SimData/hash_map.h>
#include <SimData/TypeAdapter.h>
#include <SimData/ns-simdata.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA


class Object;


SIMDATA_EXCEPTION(InterfaceError)


/*
 * Simulated Partial Template Specialization
 *
 * Adapted from: metactrl.h
 * by Krzysztof Czarnecki & Ulrich Eisenecker
 */

/* The following functions come from chapter 10 of the indispensable book
 * Generative Programming by Krzysztof Czarnecki & Ulrich Eisenecker
 * (C) Copyright Krzysztof Czarnecki & Ulrich Eisenecker 1998-2000.
 * Permission to copy, use, modify, sell and distribute this software is
 * granted provided this copyright notice appears in all copies. In case of
 * modification, the modified files should carry a notice stating that
 * you changed the files.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 */

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
	#define __PTS_SIM__
#endif

/**
 * class MemberAccessorBase - base class for storing and accessing member 
 * variable references.
 * 
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class MemberAccessorBase 
{
public:
	virtual ~MemberAccessorBase() {}

    typedef HASH_MAPS<const char *, MemberAccessorBase *, HASH<const char*>, eqstr>::Type map;

	
	virtual void set(Object *, TypeAdapter const &) throw(TypeMismatch) { 
		throw TypeMismatch("Cannot set vector<> '" + name + "' directly, use push_back() instead."); 
	}
	virtual void push_back(Object *, TypeAdapter const &) throw(TypeMismatch) { 
		throw TypeMismatch("Cannot call push_back() on non-vector<> variable '" + name + "'.");
	}
	virtual void clear(Object *) throw(TypeMismatch) { 
		throw TypeMismatch("Cannot call clear() on non-vector<> variable '" + name + "'.");
	}
	virtual TypeAdapter const get(Object *) const throw(TypeMismatch) {
		throw TypeMismatch("get() '" + name + "': not supported for variables of type vector<>.");
	}
	bool isRequired() const { return required; };
	std::string getName() const { return name; }
	virtual void pack(Object *, Packer &p) const {
		assert(0);
	}
	virtual void unpack(Object *, UnPacker &p) {
		assert(0);
	}
protected:
	MemberAccessorBase() {}
	std::string name;
	bool required;
};


/**
 * class MemberAccessor - Class for storing and accessing member variable 
 * references.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class C, typename T> 
class MemberAccessor: public MemberAccessorBase 
{
	T C::* member;
public:
	MemberAccessor(T C::*pm, std::string name_, bool required_) {
		member = pm;
		name = name_;
		required = required_;
	}
	virtual TypeAdapter const get(Object *o) const throw(TypeMismatch) {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("get(\"" + name + "\"): Object class does not match interface.");
		}
		return TypeAdapter(object->*member);
	}
	virtual void set(Object *o, TypeAdapter const &v) throw(TypeMismatch) {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("set(\"" + name + "\"): Object class does not match interface.");
		}
		v.set(object->*member);
		//T const &value = v.operator T const();
		//object->*member = value;
	}
	virtual void pack(Object *o, Packer &p) const {
		C * object = dynamic_cast<C *>(o);
		p.pack(object->*member);
	}
	virtual void unpack(Object *o, UnPacker &p) {
		C * object = dynamic_cast<C *>(o);
		p.unpack(object->*member);
	}
};


/**
 * class MemberAccessor - Partially specialized MemberAccessor for std::string<> variables.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
#ifdef __PTS_SIM__
template <class C, typename T> 
class VectorMemberAccessor: public MemberAccessorBase 
{
	T C::* member;
public:
	VectorMemberAccessor(T C::*pm, std::string name_, bool required_) {
		member = pm;
		name = name_;
		required = required_;
	}
	virtual void push_back(Object *o, TypeAdapter const &v) throw(TypeMismatch) {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("push_back(\"" + name + "\"): Object class does not match interface.");
		}
		typename T::value_type value;
		v.set(value);
		(object->*member).push_back(value);
	}
	virtual void clear(Object *o) throw(TypeMismatch) {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("push_back(\"" + name + "\"): Object class does not match interface.");
		}
		(object->*member).clear();
	}
	virtual void pack(Object *o, Packer &p) const {
		C * object = dynamic_cast<C *>(o);
		T &m = object->*member;
		p.pack(static_cast<int>(m.size()));
		typename T::iterator idx;
		for (idx = m.begin(); idx != m.end(); idx++) {
			p.pack(*idx);
		}
	}
	virtual void unpack(Object *o, UnPacker &p) {
		C * object = dynamic_cast<C *>(o);
		T &m = object->*member;
		typename T::value_type temp;
		int n;
		p.unpack(n);
		while (n-- > 0) {
			p.unpack(temp);
			m.push_back(temp);
		}
	}
};

#else

template <class C, typename T> 
class MemberAccessor< C, std::vector<T> >: public MemberAccessorBase 
{
	std::vector<T> C::* member;
public:
	MemberAccessor(std::vector<T> C::*pm, std::string name_, bool required_) {
		member = pm;
		name = name_;
		required = required_;
	}
	virtual void push_back(Object *o, TypeAdapter const &v) throw(TypeMismatch) {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("push_back(\"" + name + "\"): Object class does not match interface.");
		}
		T value;
		v.set(value);
		//T const &value = v.operator T const();
		(object->*member).push_back(value);
	}
	virtual void clear(Object *o) throw(TypeMismatch) {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("push_back(\"" + name + "\"): Object class does not match interface.");
		}
		(object->*member).clear();
	}
	virtual void pack(Object *o, Packer &p) const {
		C * object = dynamic_cast<C *>(o);
		std::vector<T> &m = object->*member;
		p.pack((int)m.size());
		typename std::vector<T>::iterator idx;
		for (idx = m.begin(); idx != m.end(); idx++) {
			p.pack(*idx);
		}
	}
	virtual void unpack(Object *o, UnPacker &p) {
		C * object = dynamic_cast<C *>(o);
		std::vector<T> &m = object->*member;
		T temp;
		int n;
		p.unpack(n);
		m.reserve(n);
		while (n-- > 0) {
			p.unpack(temp);
			m.push_back(temp);
		}
	}
};
#endif


#ifdef __PTS_SIM__

namespace PTS {

	template <typename T>
	char IsVector(std::vector<T> const *);	// no implementation is required

	int IsVector(BaseType const *);	// no implementation is required
	int IsVector(float const *);	// no implementation is required
	int IsVector(double const *);	// no implementation is required
	int IsVector(int const *);	// no implementation is required
	int IsVector(bool const*);	// no implementation is required
	int IsVector(std::string const*);// no implementation is required
	int IsVector(TypeAdapter const *);// no implementation is required

	template <typename T>
	struct ISVECTOR {
		enum { RET = (sizeof(IsVector((T*)0)) == 1) };
	};
	
	struct SelectThen
	{       template<class Then, class Else>
		struct Result
		{       typedef Then RET;
		};
	}; // end SelectThen

	struct SelectElse
	{       template<class Then, class Else>
		struct Result
		{       typedef Else RET;
		};
	}; // end SelectElse

	template<bool Condition>
	struct Selector
	{       typedef SelectThen RET;
	}; // end Selector

	template<>
	struct Selector<false>
	{       typedef SelectElse RET;
	}; // end Selector<false>

	template<bool Condition, class Then, class Else>
	struct IF
	{
		typedef typename Selector<Condition>::RET select;
		typedef typename select::Result<Then,Else>::RET RET;
	}; // IF

	template <class C, typename T>
	struct SELECT_ACCESSOR {
		typedef typename IF<ISVECTOR<T>::RET, 
			simdata::VectorMemberAccessor<C, T>, simdata::MemberAccessor<C, T> >::RET ACCESSOR;
	};
}

#endif // __PTS_SIM__


/**
 * class ObjectInterfaceBase - Pure virtual base class for object interfaces.
 *
 * @author Mark Rose <mrose@stm.lbl.gov>
 */
class ObjectInterfaceBase {
public:
	virtual ~ObjectInterfaceBase() {}
	/**
	 * Test if a given variable name exists in the interface.
	 *
	 * @return Returns true if the variable name exists in the
	 * interface.
	 */
	virtual bool variableExists(const char *name) const = 0;
	/**
	 * Test if a given variable is tagged as 'required'.  
	 *
	 * @return Returns false if the variable is not marked as
	 * 'required' or does not exist in the interface.
	 */
	virtual bool variableRequired(const char *name) const = 0;

	/**
	 * Get the accessor associated with the specified memeber variable.
	 *
	 * This method is for internal use by the interface classes only.
	 */
	virtual MemberAccessorBase *getAccessor(const char *name) = 0;

	/**
	 * Get the names of all variables defined in the interface.
	 */
	virtual std::vector<std::string> getVariableNames() const = 0;
	
	/**
	 * Get the names of required variables defined in the interface.
	 */
	virtual std::vector<std::string> getRequiredNames() const = 0;
};

template <class C>
class ObjectInterface: public ObjectInterfaceBase {
	
	typedef ObjectInterface<C> Self;
	MemberAccessorBase::map table;

public:
	ObjectInterface() {}
	virtual ~ObjectInterface() {}

	/**
	 * Method to bind a string identifier to a member variable and
	 * store the association.
	 *
	 * Do not call this method directly: use the XML() macro instead.
	 */
	template<typename T>
	Self& def(const char *name, T C::*pm, bool required) throw(InterfaceError) {
		if (variableExists(name)) throw InterfaceError("interface variable \"" + std::string(name) + "\" multiply defined.");
#ifdef __PTS_SIM__
		table[name] = new typename PTS::SELECT_ACCESSOR<C, T>::ACCESSOR(pm, name, required);
#else
		table[name] = new MemberAccessor<C, T>(pm, name, required);
#endif
		return *this;
	}

	virtual bool variableExists(const char *name) const {
		return table.find(name) != table.end();
	}

	virtual bool variableRequired(const char *name) const {
		MemberAccessorBase::map::const_iterator idx = table.find(name);
		if (idx == table.end()) return false;
		return idx->second->isRequired();
	}

	virtual MemberAccessorBase *getAccessor(const char *name) {
		MemberAccessorBase::map::const_iterator idx = table.find(name);
		if (idx == table.end()) return 0;
		return idx->second;
	}

	virtual std::vector<std::string> getVariableNames() const {
		std::vector<std::string> names;
		MemberAccessorBase::map::const_iterator idx;
		for (idx = table.begin(); idx != table.end(); idx++) {
			names.push_back(idx->first);
		}
		return names;
	}

	virtual std::vector<std::string> getRequiredNames() const {
		std::vector<std::string> names;
		MemberAccessorBase::map::const_iterator idx;
		for (idx = table.begin(); idx != table.end(); idx++) {
			if (idx->second->isRequired()) {
				names.push_back(idx->first);
			}
		}
		return names;
	}

	virtual	void pack(Object *o, Packer &p) const {
		MemberAccessorBase::map::const_iterator idx;
		for (idx = table.begin(); idx != table.end(); idx++) {
			idx->second->pack(o, p);
		}
	}

	virtual void unpack(Object *o, UnPacker &p) const {
		MemberAccessorBase::map::const_iterator idx;
		for (idx = table.begin(); idx != table.end(); idx++) {
			idx->second->unpack(o, p);
		}
	}
};


NAMESPACE_END // namespace simdata


#endif // __SIMDATA_OBJECTINTERFACE_H__

