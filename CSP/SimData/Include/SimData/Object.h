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
 * @file Object.h
 * @brief Object class and related boilerplate macros.
 */


#ifndef __SIMDATA_OBJECT_H__
#define __SIMDATA_OBJECT_H__


#include <string>

#include <SimData/BaseType.h>
#include <SimData/Export.h>
#include <SimData/HashUtility.h>
#include <SimData/InterfaceProxy.h>
#include <SimData/Referenced.h>


NAMESPACE_SIMDATA


class DataArchive;
class LinkCore;


/** Base class for all classes representing packable data objects.
 *
 *  Derived classes that provide an XML data interface must include one of
 *  the SIMDATA_DECLARE_*OBJECT macros in the class declaration and define
 *  the XML interface using the SIMDATA_XML_* macros in the correspending
 *  source file (outside of the class declaration).
 *
 *  The following methods may be extended in derived classes:
 *  @li @c parseXML    parse loose XML cdata if present
 *  @li @c convertXML  post-process XML data
 *  @li @c postCreate  additional processing after deserialization
 *
 *  Objects should never be copied or handled directly by user code.  Use
 *  Ref<> handles to manipulate them by reference, and Link<> member variables
 *  to load them from data archives.  Objects set as 'static' are singletons
 *  managed by the DataArchive.
 *
 *  @author Mark Rose <mkrose@users.sf.net>
 *  @ingroup BaseTypes
 */
class SIMDATA_EXPORT Object: public virtual Referenced {
	friend class DataArchive;
	friend class LinkCore;

private:
	// Objects should never be copied
	Object(Object const &o);
	Object const &operator=(Object const &);

	void _setPath(hasht);

	hasht _path;

protected:
	/** Initialize an object after deserialization.
	 *
	 *  Called after the newly created object has been deserialized.  Extend (not
	 *  override) this method to do any initial processing of the external data.
	 */
	virtual void postCreate() {}

	/** Internal methos for saving the objects via the xml interface.
	 *  Automatically extended in subclasses by the SIMDATA_DECLARE_*OBJECT
	 *  macros.  Do not explicitly extend or call these methods.
	 */
	virtual void _serialize(Writer&) const { };

	/** Internal methos for loading the objects via the xml interface.
	 *  Automatically extended in subclasses by the SIMDATA_DECLARE_*OBJECT
	 *  macros.  Do not explicitly extend or call these methods.
	 */
	virtual void _serialize(Reader&) { };

	/** Used by SIMDATA_DECLARE_*OBJECT macros
	 */
	typedef InterfaceProxy __simdata_interface_proxy;
	typedef Object __simdata_object_class;

public:

	Object();
	virtual ~Object();

	/** Create a new instance of the class.
	 *
	 *  The method is automatically overridden by the SIMDATA_OBJECT
	 *  macro, so you should never need to extend it manually.
	 */
	virtual Object* _new() const {
		fatal(std::string("Attempt to construct simdata::Object (") + getClassName());
		return 0;
	}

	/** Get the class name as a string.
	 */
	static const char *_getClassName() { return "Object"; }

	/** Get the class name as a string.
	 */
	virtual const char *getClassName() const { return "Object"; }

	/** Get a fingerprint of the class and xml interface.
	 */
	static hasht _getClassHash() { return hasht(1); }

	/** Get a fingerprint of the class and xml interface.
	 */
	virtual hasht getClassHash() const { return hasht(1); }

	/** Test if the object is marked as static (so that one instance will be
	 *  cached and reused when loaded multiple times from a data archive).
	 */
	static bool _isClassStatic() { return false; }

	/** Test if the object is marked as static (so that one instance will be
	 *  cached and reused when loaded multiple times from a data archive).
	 */
	virtual bool isClassStatic() const { return false; }

	/** XML post processing prior to serialization.
	 */
	virtual void convertXML() {}

	/** Parse the character data from an \<Object\> tag.  The default
	 *  implementation throws an exception if any non-whitespace cdata
	 *  is present.
	 */
	virtual void parseXML(const char *cdata) { checkEmptyTag(cdata); }

	/** Serialize additional state to a data target.
	 *
	 *  Extend ths method only if the subclass must save additional state
	 *  not covered by the external variable declarations (which are
	 *  automatically saved and restored).  Call the base class method
	 *  before saving state.
	 *
	 *  Any data written by this method must be read back in the same order
	 *  in the deserializeExtra method, and the superclass implementation
	 *  should be called first.
	 */
	virtual void serialize(Writer&) const;

	/** Serialize additional state from a data source.
	 *
	 *  Extend ths method only if the subclass must load additional state
	 *  not covered by the external variable declarations (which are
	 *  automatically saved and restored).  Call the base class method
	 *  before reading state.
	 *
	 *  Any data loaded by this method must be written in the same order
	 *  in the serializeExtra method, and the superclass implementation
	 *  should be called first.
	 *
	 *  Any additional processing of the data following retrieval
	 *  should be done in the postCreate() method.
	 */
	virtual void serialize(Reader&);

	/** Get a string representation of the object.
	 *
	 *  This method is also used for writing to ostreams
	 *  using operator<<().
	 */
	virtual std::string asString() const;

	/** Return a string representation of the type.
	 *
	 *  The type string for classes derived from Object
	 *  is just the classname.  The implementation in
	 *  Object returns the true classname using getClassName(),
	 *  so this method should not be overridden in derived
	 *  classes.
	 */
	virtual std::string typeString() const { return getClassName(); }

	/** Get the path hash from which the object was instantiated.
	 *
	 *  Given the source DataArchive, this path hash can be
	 *  converted back to a path string using getPathString().
	 */
	hasht getPath() const;

	/** Hash function for converting data archive paths to path hashes.
	 */
	//static hasht _getHash(const char* c);

	/** Expose the postCreate method.   Experimental --- use with great caution.
	 *  Ordinarily postCreate is called only by DataArchive after deserializing
	 *  an object.  This method is intended to assist with object creation directly
	 *  from XML data (typically in Python).
	 */
	void _postCreate() { postCreate(); }
};


SIMDATA_EXPORT std::ostream &operator <<(std::ostream &o, Object const &obj);


NAMESPACE_SIMDATA_END


#endif //__SIMDATA_OBJECT_H__

