/* SimData: Data Infrastructure for Simulations
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file ObjectInterface.h
 * @brief Internal classes for accessing the public xml interface of
 *   an Object class.
 */


#ifndef __SIMDATA_OBJECTINTERFACE_H__
#define __SIMDATA_OBJECTINTERFACE_H__


#include <string>
#include <vector>
#include <algorithm>

#include <SimData/hash_map.h>
#include <SimData/TypeAdapter.h>
#include <SimData/Namespace.h>
#include <SimData/Archive.h>

/*
 * Simulated Partial Template Specialization
 *
 * Adapted from: metactrl.h
 * by Krzysztof Czarnecki & Ulrich Eisenecker
 */
#include <SimData/PTS.h>



NAMESPACE_SIMDATA


class Object;


SIMDATA_EXCEPTION(InterfaceError)



/** Base class for storing and accessing member variable references.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class OBJECT>
class MemberAccessorBase
{
public:
	virtual ~MemberAccessorBase() {}

	typedef typename HASH_MAPS<std::string, MemberAccessorBase *, hashstring, eqstring>::Type map;

	
	virtual void set(OBJECT *, TypeAdapter const &) throw(TypeMismatch) {
		throw TypeMismatch("Cannot set vector<> '" + name + "' directly, use push_back() instead.");
	}
	virtual void push_back(OBJECT *, TypeAdapter const &) throw(TypeMismatch) {
		throw TypeMismatch("Cannot call push_back() on non-vector<> variable '" + name + "'.");
	}
	virtual void clear(OBJECT *) throw(TypeMismatch) {
		throw TypeMismatch("Cannot call clear() on non-vector<> variable '" + name + "'.");
	}
	virtual TypeAdapter const get(OBJECT *) const throw(TypeMismatch) {
		throw TypeMismatch("get() '" + name + "': not supported for variables of type vector<>.");
	}
	bool isRequired() const { return required; };
	std::string getName() const { return name; }
	/*
	virtual void pack(OBJECT *, Packer &) const {
		assert(0);
	}
	virtual void unpack(OBJECT *, UnPacker &) {
		assert(0);
	}
	*/
	virtual void serialize(OBJECT const *, Writer &) const {
		assert(0);
	}
	virtual void serialize(OBJECT *, Reader &) const {
		assert(0);
	}
	std::string getType() const { return type; }
	virtual unsigned int getMask() const { return 0; }
protected:
	void setType(BaseType &x) { type = x.typeString(); }
	void setType(double &) { type = "builtin::double"; }
	void setType(float &) { type = "builtin::float"; }
	void setType(unsigned int &) { type = "builtin::uint"; }
	void setType(int &) { type = "builtin::int"; }
	void setType(unsigned char &) { type = "builtin::uint8"; }
	void setType(char &) { type = "builtin::int8"; }
	void setType(unsigned short &) { type = "builtin::uint16"; }
	void setType(short &) { type = "builtin::int16"; }
	void setType(bool &) { type = "builtin::bool"; }
	void setType(std::string const &) { type = "builtin::string"; }
	MemberAccessorBase(): type("none") {}
	std::string name;
	std::string help;
	std::string type;
	bool required;
};


/** Class for storing and accessing bitmasked member variable references.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class OBJECT, typename T>
class MemberMaskAccessor: public MemberAccessorBase<OBJECT>
{
	// primaries is a bit of a kludge to prevent shared member mask fields
	// from being serialized multiple times.  the first MMA for a masked field
	// is marked as primary, and will be the only one serialized.  we use a
	// simple vector because we expect the most common case to be just one
	// (at at most a few) masked fields of a given type (e.g. int32) per
	// object subclass.
	static std::vector<T OBJECT::*> primaries;
	bool primary;
	T OBJECT::* member;
	T mask;
public:
	MemberMaskAccessor(T OBJECT::*pm, std::string name_, int mask_, bool required_) {
		primary = std::find(primaries.begin(), primaries.end(), pm) == primaries.end();
		if (primary) primaries.push_back(pm);
		member = pm;
		name = name_;
		mask = mask_;
		required = required_;
		{
			T prototype;
			setType(prototype);
		}
	}
	virtual TypeAdapter const get(OBJECT *object) const throw(TypeMismatch) {
		return TypeAdapter(object->*member);
	}
	virtual void set(OBJECT *object, TypeAdapter const &v) throw(TypeMismatch) {
		try {
			if (mask != 0) {
				T value;
				v.set(value);
				if (value != 0) {
					object->*member |= mask;
				} else {
					object->*member &= ~mask;
				}
			} else {
				v.set(object->*member);
			}
		} catch (Exception &e) {
			e.addMessage("MemberMaskAccessor::set(" + name + "):");
			throw;
		}
	}
	virtual void serialize(OBJECT const *object, Writer &writer) const {
		if (primary) writer << (object->*member);
	}
	virtual void serialize(OBJECT *object, Reader &reader) const {
		if (primary) reader >> (object->*member);
	}
	virtual unsigned int getMask() const { return static_cast<unsigned int>(mask); }
};

template <class OBJECT, typename T>
std::vector<T OBJECT::*> MemberMaskAccessor<OBJECT, T>::primaries;


/** Class for storing and accessing member variable references.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class OBJECT, typename T>
class MemberAccessor: public MemberAccessorBase<OBJECT>
{
	T OBJECT::* member;
public:
	MemberAccessor(T OBJECT::*pm, std::string name_, bool required_) {
		member = pm;
		name = name_;
		required = required_;
		{
			T prototype;
			setType(prototype);
		}
	}
	virtual TypeAdapter const get(OBJECT *object) const throw(TypeMismatch) {
		return TypeAdapter(object->*member);
	}
	virtual void set(OBJECT *object, TypeAdapter const &v) throw(TypeMismatch) {
		try {
			v.set(object->*member);
		} catch (Exception &e) {
			e.addMessage("MemberAccessor::set(" + name + "):");
			throw;
		}
	}
	virtual void serialize(OBJECT const *object, Writer &writer) const {
		writer << (object->*member);
	}
	virtual void serialize(OBJECT *object, Reader &reader) const {
		reader >> (object->*member);
	}
};


/** Partially specialized MemberAccessor for std::vector<> variables.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
#ifdef __SIMDATA_PTS_SIM
template <class OBJECT, typename T>
class VectorMemberAccessor: public MemberAccessorBase<OBJECT>
{
	T OBJECT::* member;
public:
	VectorMemberAccessor(T OBJECT::*pm, std::string name_, bool required_) {
		member = pm;
		name = name_;
		required = required_;
		{
			T prototype;
			setType(prototype);
			type = "vector::" + type;
		}
	}
	virtual void push_back(OBJECT *object, TypeAdapter const &v) throw(TypeMismatch) {
		typename T::value_type value;
		try {
			v.set(value);
		} catch (Exception &e) {
			e.addMessage("VectorMemberAccessor::push_back(" + name + "):");
			throw;
		}
		(object->*member).push_back(value);
	}
	virtual void clear(OBJECT *object) throw(TypeMismatch) {
		(object->*member).clear();
	}
	virtual void serialize(OBJECT const *object, Writer &writer) const {
		T &m = object->*member;
		writer << m;
	}
	virtual void serialize(OBJECT *object, Reader &reader) const {
		T &m = object->*member;
		reader >> m;
	}
};

#else // #if !defined(__SIMDATA_PTS_SIM)
template <class OBJECT, typename T>
class MemberAccessor< OBJECT, std::vector<T> >: public MemberAccessorBase<OBJECT>
{
	std::vector<T> OBJECT::* member;
public:
	MemberAccessor(std::vector<T> OBJECT::*pm, std::string name_, bool required_) {
		member = pm;
		name = name_;
		required = required_;
		{
			T prototype;
			setType(prototype);
			type = "vector::" + type;
		}
	}
	virtual void push_back(OBJECT *object, TypeAdapter const &v) throw(TypeMismatch) {
		T value;
		try {
			v.set(value);
		} catch (Exception &e) {
			e.addMessage("MemberAccessor::push_back(" + name + "):");
			throw;
		}
		(object->*member).push_back(value);
	}
	virtual void clear(OBJECT *object) throw(TypeMismatch) {
		(object->*member).clear();
	}
	virtual void serialize(OBJECT const *object, Writer &writer) const {
		std::vector<T> const &m = object->*member;
		writer << m;
	}
	virtual void serialize(OBJECT *object, Reader &reader) const {
		std::vector<T> &m = object->*member;
		reader >> m;
	}
};

/** Specialized MemberMaskAccessor for int bitmasked variables.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class OBJECT>
class MemberAccessor< OBJECT, int >: public MemberMaskAccessor<OBJECT, int> {
public:
	MemberAccessor(int OBJECT::*pm, std::string name_, bool required_):
		MemberMaskAccessor<OBJECT, int>(pm, name_, 0, required_) { }
	MemberAccessor(int OBJECT::*pm, std::string name_, int mask_, bool required_):
		MemberMaskAccessor<OBJECT, int>(pm, name_, mask_, required_) { }
};

/** Specialized MemberMaskAccessor for short int bitmasked variables.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class OBJECT>
class MemberAccessor< OBJECT, short >: public MemberMaskAccessor<OBJECT, short> {
public:
	MemberAccessor(short OBJECT::*pm, std::string name_, bool required_):
		MemberMaskAccessor<OBJECT, short>(pm, name_, 0, required_) { }
	MemberAccessor(short OBJECT::*pm, std::string name_, int mask_, bool required_):
		MemberMaskAccessor<OBJECT, short>(pm, name_, mask_, required_) { }
};

/** Specialized MemberMaskAccessor for char bitmasked variables.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class OBJECT>
class MemberAccessor< OBJECT, char >: public MemberMaskAccessor<OBJECT, char> {
public:
	MemberAccessor(char OBJECT::*pm, std::string name_, bool required_):
		MemberMaskAccessor<OBJECT, char>(pm, name_, 0, required_) { }
	MemberAccessor(char OBJECT::*pm, std::string name_, int mask_, bool required_):
		MemberMaskAccessor<OBJECT, char>(pm, name_, mask_, required_) { }
};

#endif // !defined(__SIMDATA_PTS_SIM)


#ifdef __SIMDATA_PTS_SIM

/** Simulated Partial Template Specialization
 *
 * Adapted from: metactrl.h
 * by Krzysztof Czarnecki & Ulrich Eisenecker
 *
 * The following functions come from chapter 10 of the indispensable book
 * Generative Programming by Krzysztof Czarnecki & Ulrich Eisenecker
 * (C) Copyright Krzysztof Czarnecki & Ulrich Eisenecker 1998-2000.
 * Permission to copy, use, modify, sell and distribute this software is
 * granted provided this copyright notice appears in all copies. In case of
 * modification, the modified files should carry a notice stating that
 * you changed the files.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 */
namespace PTS {

	template <typename T>
	char IsVector(std::vector<T> const *);	// no implementation is required

	int IsVector(BaseType const *);	// no implementation is required
	int IsVector(float const *);	// no implementation is required
	int IsVector(double const *);	// no implementation is required
	int IsVector(int const *);	// no implementation is required
	int IsVector(bool const*);	// no implementation is required
	int IsVector(char const*);	// no implementation is required
	int IsVector(short const*);	// no implementation is required
	int IsVector(std::string const*);// no implementation is required
	int IsVector(TypeAdapter const *);// no implementation is required

	template <typename T>
	struct ISVECTOR {
		enum { RET = (sizeof(IsVector((T*)0)) == 1) };
	};
	
	template <class C, typename T>
	struct SELECT_ACCESSOR {
		typedef typename meta::IF<ISVECTOR<T>::RET,
			simdata::VectorMemberAccessor<C, T>, simdata::MemberAccessor<C, T> >::RET ACCESSOR;
	};
}

#endif // __SIMDATA_PTS_SIM


/** Pure virtual base class for object interfaces.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
class ObjectInterfaceBase {

public:
	virtual ~ObjectInterfaceBase() {}

	/** Test if a given variable name exists in the interface.
	 *
	 *  @return Returns true if the variable name exists in the
	 *  interface.
	 */
	virtual bool variableExists(std::string const &name) const = 0;

	/** Test if a given variable is tagged as 'required'.
	 *
	 *  @return Returns false if the variable is not marked as
	 *  'required' or does not exist in the interface.
	 */
	virtual bool variableRequired(std::string const &name) const = 0;

	/** Get a type identifier string for a variable.
	 *
	 *  @return Returns the type identifier string.
	 */
	virtual std::string variableType(std::string const &name) const = 0;

	/** Get the names of all variables defined in the interface.
	 */
	virtual std::vector<std::string> getVariableNames() const = 0;
	
	/** Get the names of required variables defined in the interface.
	 */
	virtual std::vector<std::string> getRequiredNames() const = 0;

	/** Get the value of an interface variable.
	 */
	virtual const TypeAdapter get(Object *o, std::string const &name) const = 0;
	
	/** Set the value of an interface variable.
	 */
	virtual void set(Object *o, std::string const &name, const TypeAdapter &v) const = 0;
	
	/** Append a value to an interface variable list.
	 */
	virtual void push_back(Object *o, std::string const &name, const TypeAdapter &v) const = 0;
	
	/** Clear an interface variable list.
	 */
	virtual void clear(Object *o, std::string const &name) const = 0;

friend class InterfaceProxy;
protected:
	//virtual	void pack(Object *o, Packer &p) const = 0;
	//virtual void unpack(Object *o, UnPacker &p) const = 0;
};


/** Class-specialized object interface.
 *
 *  @internal
 *  @author Mark Rose <mrose@stm.lbl.gov>
 */
template <class C>
class ObjectInterface: public ObjectInterfaceBase {

	typename MemberAccessorBase<C>::map table;

	typedef ObjectInterface<C> Self;

	void __not_found(std::string const &name) const throw (InterfaceError) {
		throw InterfaceError("Variable '"+name+"' not found in interface to class '" + C::_getClassName()+"'");
	}

	MemberAccessorBase<C> *getAccessor(std::string const &name) const {
		typename MemberAccessorBase<C>::map::const_iterator idx = table.find(name);
		if (idx == table.end()) return 0;
		return idx->second;
	}

public:
	ObjectInterface() {}
	virtual ~ObjectInterface() {}

	/** Method to bind a string identifier to a member variable and
	 *  store the association.
	 *
	 *  @note Do not call this method directly: use the SIMDATA_XML()
	 *        macro instead.
	 */
	template<typename T>
	Self& def(std::string const &name, T C::*pm, bool required) throw(InterfaceError) {
		if (variableExists(name)) throw InterfaceError("interface variable \"" + std::string(name) + "\" multiply defined in class '" + C::_getClassName() + "'.");
#ifdef __SIMDATA_PTS_SIM
		table[name] = new typename PTS::SELECT_ACCESSOR<C, T>::ACCESSOR(pm, name, required);
#else
		table[name] = new MemberAccessor<C, T>(pm, name, required);
#endif
		return *this;
	}
	template<typename T>
	Self& def(std::string const &name, T C::*pm, int mask, bool required) throw(InterfaceError) {
		if (variableExists(name)) throw InterfaceError("interface variable \"" + std::string(name) + "\" multiply defined in class '" + C::_getClassName() + "'.");
#ifdef __SIMDATA_PTS_SIM
		table[name] = new typename PTS::SELECT_ACCESSOR<C, T>::ACCESSOR(pm, name, required);
#else
		table[name] = new MemberAccessor<C, T>(pm, name, mask, required);
#endif
		return *this;
	}

	inline Self& pass() { return *this; }

	/** Test if a given variable name exists in the interface.
	 *
	 *  @return Returns true if the variable name exists in the
	 *  interface.
	 */
	virtual bool variableExists(std::string const &name) const {
		return table.find(name) != table.end();
	}

	/** Test if a given variable is tagged as 'required'.
	 *
	 *  @return Returns false if the variable is not marked as
	 *  'required' or does not exist in the interface.
	 */
	virtual bool variableRequired(std::string const &name) const {
		typename MemberAccessorBase<C>::map::const_iterator idx = table.find(name);
		if (idx == table.end()) return false;
		return idx->second->isRequired();
	}

	/** Get a type identifier string for a variable.
	 *
	 *  @return Returns the type identifier string.
	 */
	virtual std::string variableType(std::string const &name) const {
		typename MemberAccessorBase<C>::map::const_iterator idx = table.find(name);
		if (idx == table.end()) __not_found(name);
		return idx->second->getType();
	}

	/** Get the names of all variables defined in the interface.
	 */
	virtual std::vector<std::string> getVariableNames() const {
		std::vector<std::string> names;
		typename MemberAccessorBase<C>::map::const_iterator idx;
		for (idx = table.begin(); idx != table.end(); idx++) {
			names.push_back(idx->first);
		}
		return names;
	}

	/** Get the names of required variables defined in the interface.
	 */
	virtual std::vector<std::string> getRequiredNames() const {
		std::vector<std::string> names;
		typename MemberAccessorBase<C>::map::const_iterator idx;
		for (idx = table.begin(); idx != table.end(); idx++) {
			if (idx->second->isRequired()) {
				names.push_back(idx->first);
			}
		}
		return names;
	}

	/** Get the value of an interface variable.
	 */
	virtual const TypeAdapter get(Object *o, std::string const &name) const {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("get(\"" + name + "\"): Object class does not match interface.");
		}
		return getAccessor(name)->get(object);
	}
	
	/** Set the value of an interface variable.
	 */
	virtual void set(Object *o, std::string const &name, const TypeAdapter &v) const {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("set(\"" + name + "\"): Object class does not match interface.");
		}
		getAccessor(name)->set(object, v);
	}
	
	/** Append a value to an interface variable list.
	 */
	virtual void push_back(Object *o, std::string const &name, const TypeAdapter &v) const {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("push_back(\"" + name + "\"): Object class does not match interface.");
		}
		getAccessor(name)->push_back(object, v);
	}
	
	/** Clear an interface variable list.
	 */
	virtual void clear(Object *o, std::string const &name) const {
		C * object = dynamic_cast<C *>(o);
		if (object == NULL) {
			throw TypeMismatch("clear(\"" + name + "\"): Object class does not match interface.");
		}
		getAccessor(name)->clear(object);
	}

	void serialize(C const *object, Writer &writer) const {
		typename MemberAccessorBase<C>::map::const_iterator idx;
		try {
			for (idx = table.begin(); idx != table.end(); ++idx) {
				idx->second->serialize(object, writer);
			}
		} catch (Exception &e) {
			e.addMessage("error serializing (write) " + std::string(C::_getClassName()) + "::" + idx->second->getName());
			throw;
		}
	}

	void serialize(C *object, Reader &reader) const {
		typename MemberAccessorBase<C>::map::const_iterator idx;
		try {
			for (idx = table.begin(); idx != table.end(); ++idx) {
				idx->second->serialize(object, reader);
			}
		} catch (Exception &e) {
			e.addMessage("error serializing (read) " + std::string(C::_getClassName()) + "::" + idx->second->getName());
			throw;
		}
	}
};


NAMESPACE_SIMDATA_END


#endif // __SIMDATA_OBJECTINTERFACE_H__

