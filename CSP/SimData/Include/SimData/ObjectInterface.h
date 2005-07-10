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
#include <sstream>

#include <SimData/hash_map.h>
#include <SimData/TypeAdapter.h>
#include <SimData/Namespace.h>
#include <SimData/Archive.h>
#include <SimData/Link.h>
#include <SimData/HashUtility.h>


NAMESPACE_SIMDATA


class Object;


SIMDATA_EXCEPTION(InterfaceError)


template <typename T>
struct MemberValidator
{
	static inline void validate(T const &, bool /*required*/, bool /*write*/) { }
};


template <typename T>
struct MemberValidator< Link<T> >
{
	static void validate(Link<T> const &link, bool required, bool write) {
		if (required) {
			if (link.isNone() && link.isNull()) {
				if (write) {
					throw SerializeError("Attempt to save " + link.typeString() + " with no path and no object");
				} else {
					throw SerializeError("Attempt to load " + link.typeString() + " with no path and no object");
				}
			}
		}
	}
};

template <typename T>
struct MemberValidator< std::vector< Link<T> > >
{
	static void validate(std::vector< Link<T> > const &link, bool /*required*/, bool write) {
		typename std::vector< Link<T> >::const_iterator iter = link.begin();
		for (unsigned idx = 1; iter != link.end(); ++iter, ++idx) {
			if (iter->isNone() && iter->isNull()) {
				std::ostringstream os;
				os << "(" << idx << " of " << link.size() << ")";
				if (write) {
					throw SerializeError("Attempt to save " + iter->typeString() + "::vector with no path and no object " + os.str());
				} else {
					throw SerializeError("Attempt to load " + iter->typeString() + "::vector with no path and no object " + os.str());
				}
			}
		}
	}
};


/** Base class for storing and accessing member variable references.
 *
 *  @internal
 *  @author Mark Rose <mkrose@users.sf.net>
 */
template <class OBJECT>
class MemberAccessorBase
{
public:
	virtual ~MemberAccessorBase() {}

	typedef typename HASH_MAPS<std::string, MemberAccessorBase *, hashstring, eqstring>::Type map;

	
	virtual void set(OBJECT *, TypeAdapter const &) {
		throw TypeMismatch("Cannot set vector<> '" + name + "' directly, use push_back() instead.");
	}
	virtual void push_back(OBJECT *, TypeAdapter const &) {
		throw TypeMismatch("Cannot call push_back() on non-vector<> variable '" + name + "'.");
	}
	virtual void clear(OBJECT *) {
		throw TypeMismatch("Cannot call clear() on non-vector<> variable '" + name + "'.");
	}
	virtual TypeAdapter const get(OBJECT *) const {
		throw TypeMismatch("get() '" + name + "': not supported for variables of type vector<>.");
	}
	bool isRequired() const { return required; };
	std::string getName() const { return name; }
	virtual void serialize(OBJECT const *, Writer &) const { assert(0); }
	virtual void serialize(OBJECT *, Reader &) const { assert(0); }
	std::string getType() const { return type; }
	virtual unsigned int getMask() const { return 0; }
protected:
	template <class BASETYPE>
	void setType(BASETYPE const &x) { type = x.typeString(); }
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
 *  @author Mark Rose <mkrose@users.sf.net>
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
		this->name = name_;
		this->mask = mask_;
		this->required = required_;
		{
			T prototype;
			setType(prototype);
		}
	}
	virtual TypeAdapter const get(OBJECT *object) const {
		return TypeAdapter(object->*member);
	}
	virtual void set(OBJECT *object, TypeAdapter const &v) {
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
			e.addMessage("MemberMaskAccessor::set(" + this->name + "):");
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
 *  @author Mark Rose <mkrose@users.sf.net>
 */
template <class OBJECT, typename T>
class MemberAccessor: public MemberAccessorBase<OBJECT>
{
	T OBJECT::* member;
public:
	MemberAccessor(T OBJECT::*pm, std::string name_, bool required_) {
		member = pm;
		this->name = name_;
		this->required = required_;
		{
			T prototype;
			setType(prototype);
		}
	}
	virtual TypeAdapter const get(OBJECT *object) const {
		return TypeAdapter(object->*member);
	}
	virtual void set(OBJECT *object, TypeAdapter const &v) {
		try {
			v.set(object->*member);
		} catch (Exception &e) {
			e.addMessage("MemberAccessor::set(" + this->name + "):");
			throw;
		}
	}
	virtual void serialize(OBJECT const *object, Writer &writer) const {
		MemberValidator<T>::validate(object->*member, this->required, true /*write*/);
		writer << (object->*member);
	}
	virtual void serialize(OBJECT *object, Reader &reader) const {
		reader >> (object->*member);
		MemberValidator<T>::validate(object->*member, this->required, false /*write*/);
	}
};


/** Partially specialized MemberAccessor for std::vector<> variables.
 *
 *  @internal
 *  @author Mark Rose <mkrose@users.sf.net>
 */
template <class OBJECT, typename T>
class MemberAccessor< OBJECT, std::vector<T> >: public MemberAccessorBase<OBJECT>
{
	typedef std::vector<T> VT;
	VT OBJECT::* member;
public:
	MemberAccessor(std::vector<T> OBJECT::*pm, std::string name_, bool required_) {
		member = pm;
		this->name = name_;
		this->required = required_;
		{
			T prototype;
			setType(prototype);
			this->type = "vector::" + this->type;
		}
	}
	virtual void push_back(OBJECT *object, TypeAdapter const &v) {
		T value;
		try {
			v.set(value);
		} catch (Exception &e) {
			e.addMessage("MemberAccessor::push_back(" + this->name + "):");
			throw;
		}
		(object->*member).push_back(value);
	}
	virtual void clear(OBJECT *object) {
		(object->*member).clear();
	}
	virtual void serialize(OBJECT const *object, Writer &writer) const {
		VT const &m = object->*member;
		MemberValidator<VT>::validate(m, this->required, true /*write*/);
		writer << m;
	}
	virtual void serialize(OBJECT *object, Reader &reader) const {
		VT &m = object->*member;
		reader >> m;
		MemberValidator<VT>::validate(m, this->required, false /*write*/);
	}
};

/** Specialized MemberMaskAccessor for int bitmasked variables.
 *
 *  @internal
 *  @author Mark Rose <mkrose@users.sf.net>
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
 *  @author Mark Rose <mkrose@users.sf.net>
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
 *  @author Mark Rose <mkrose@users.sf.net>
 */
template <class OBJECT>
class MemberAccessor< OBJECT, char >: public MemberMaskAccessor<OBJECT, char> {
public:
	MemberAccessor(char OBJECT::*pm, std::string name_, bool required_):
		MemberMaskAccessor<OBJECT, char>(pm, name_, 0, required_) { }
	MemberAccessor(char OBJECT::*pm, std::string name_, int mask_, bool required_):
		MemberMaskAccessor<OBJECT, char>(pm, name_, mask_, required_) { }
};


/** Pure virtual base class for object interfaces.
 *
 *  @internal
 *  @author Mark Rose <mkrose@users.sf.net>
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

	/** Compute a fingerprint that includes the class name and the names and
	 *  types of all variables in the interface.
	 */
	virtual fprint32 signature() const = 0;

	/** Get the name of the class for this interface.
	 */
	virtual const char *getClassName() const = 0;

	/** Get the hash of the class for this interface.
	 */
	virtual hasht getClassHash() const = 0;

friend class InterfaceProxy;
};


/** Class-specialized object interface.
 *
 *  @internal
 *  @author Mark Rose <mkrose@users.sf.net>
 */
template <class C>
class ObjectInterface: public ObjectInterfaceBase {

	typename MemberAccessorBase<C>::map table;

	typedef ObjectInterface<C> Self;

	void __not_found(std::string const &name) const {
		throw InterfaceError("Variable '" + name + "' not found in interface to class '" + C::_getClassName() + "'");
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
	Self& def(std::string const &name, T C::*pm, bool required) {
		if (variableExists(name)) throw InterfaceError("interface variable \"" + name + "\" multiply defined in class '" + C::_getClassName() + "'.");
		table[name] = new MemberAccessor<C, T>(pm, name, required);
		return *this;
	}
	template<typename T>
	Self& def(std::string const &name, T C::*pm, int mask, bool required) {
		if (variableExists(name)) throw InterfaceError("interface variable \"" + name + "\" multiply defined in class '" + C::_getClassName() + "'.");
		table[name] = new MemberAccessor<C, T>(pm, name, mask, required);
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

	/** Serialize an object to a writer stream.
	 */
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

	/** Serialize an object from a writer stream.
	 */
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

	/** Compute a fingerprint that includes the class name and the names and
	 *  types of all variables in the interface.
	 */
	virtual fprint32 signature() const {
		static fprint32 fp = 0;
		if (fp == 0) {
			fp = fingerprint(C::_getClassName());
			typename MemberAccessorBase<C>::map::const_iterator idx;
			for (idx = table.begin(); idx != table.end(); ++idx) {
				fp = make_ordered_fingerprint(fp, fingerprint(idx->first + "!" + idx->second->getType()));
			}
		}
		return fp;
	}

	/** Get the name of the class for this interface.
	 */
	virtual const char *getClassName() const { return C::_getClassName(); }

	/** Get the hash of the class for this interface.
	 */
	virtual hasht getClassHash() const { return C::_getClassHash(); }
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_OBJECTINTERFACE_H__

