// Copyright (C) 1999-2002 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

/**
 * @file persist.h
 * @short Persistence library classes.
 **/

#ifndef	CCXX_PERSIST_H_
#define	CCXX_PERSIST_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef	CCXX_STRCHAR_H_
#include <cc++/persist.h>
#endif

#ifdef HAVE_ZLIB_H
#ifndef NO_COMPRESSION
#include <zlib.h>
#endif
#else
#define	NO_COMPRESSION
#endif

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <map>

#ifdef CCXX_NAMESPACES
namespace ost {
#endif

#ifdef	COMMON_STD_EXCEPTION

class CCXX_CLASS_EXPORT PersistException
{
public:
	PersistException(const std::string &what_arg) throw();
	virtual ~PersistException() throw();
	const std::string &getString() const;
};

#else

class PersistException
{
public:
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) PersistException(const std::string& reason);
	CCXX_MEMBER_EXPORT(const std::string&) getString() const;
	CCXX_MEMBER_EXPORT(virtual) ~PersistException();
protected:
	std::string _what;
};

#endif

// This typedef allows us to declare NewBaseObjectFunction now
typedef class BaseObject* (*NewBaseObjectFunction) (void);

/**
 * This class manages the types for generation of the persistent objects.
 * Its data structures are managed automatically by the system. They are
 * implicitly filled by the constructors who declare classes to the system.
 *
 * @author Daniel Silverstone
 * @short Type manager for persistence engine.
 */
class TypeManager
{
public:
	
	/**
	 * This manages a registration to the typemanager - attempting to
	 * remove problems with the optimisers
	 */
	class Registration
	{
	public:
		Registration(const char* name, NewBaseObjectFunction func)
			: myName(name) { TypeManager::add(name,func); }
		virtual ~Registration() { TypeManager::remove(myName.c_str()); }
	private:
		std::string myName;
	};
	
	/**
	 * This adds a new construction function to the type manager
	 */
	CCXX_MEMBER_EXPORT(static void) add(const char* name, NewBaseObjectFunction construction);
	
	/**
	 * And this one removes a type from the managers lists
	 */
	CCXX_MEMBER_EXPORT(static void) remove(const char* name);
	
	/**
	 * This function creates a new object of the required type and
	 * returns a pointer to it. NULL is returned if we couldn't find 
	 * the type
	 */
	CCXX_EXPORT(static BaseObject*) createInstanceOf(const char* name);
	
	typedef std::map<std::string,NewBaseObjectFunction> StringFunctionMap;
};


/*
 * The following defines are used to declare and define the relevant code
 * to allow a class to use the Persistence::Engine code.
 */

#define DECLARE_PERSISTENCE(ClassType)					\
  public:								\
    friend Engine& operator>>(Engine& ar, ClassType *&ob);		\
    friend Engine& operator<<(Engine& ar, ClassType const &ob);	\
    friend BaseObject *createNew##ClassType();				\
    virtual const char* getPersistenceID() const;			\
    static TypeManager::Registration registrationFor##ClassType;

#define IMPLEMENT_PERSISTENCE(ClassType, FullyQualifiedName)		      \
  BaseObject *createNew##ClassType() { return new ClassType; }		      \
  const char* ClassType::getPersistenceID() const {return FullyQualifiedName;} \
  Engine& operator>>(Engine& ar, ClassType &ob)			      \
    { ar >> (BaseObject &) ob; return ar; }				      \
  Engine& operator>>(Engine& ar, ClassType *&ob)			      \
    { ar >> (BaseObject *&) ob; return ar; }				      \
  Engine& operator<<(Engine& ar, ClassType const &ob)			      \
    { ar << (BaseObject const *)&ob; return ar; }			      \
  TypeManager::Registration						      \
        ClassType::registrationFor##ClassType(FullyQualifiedName,	      \
					      createNew##ClassType);

class Engine;

/**
 * BaseObject
 *
 * This object is the base for all Persistent data which is not
 * natively serialised by the Persistence::Engine
 *
 * It registers itself with the Persistence::TypeManager
 * using a global constructor function. A matching deregister call
 * is made in a global destructor, to allow DLL's to use the
 * Persistence::Engine in a main executable.
 *
 * Persistable objects must never maintain bad pointers. If a pointer
 * doesn't point to something valid, it must be NULL.  This is so
 * the persistence engine knows whether to allocate memory for an object
 * or whether the memory has been pre-allocated.
 *
 * @author Daniel Silverstone
 * @short Base class for classes that will be persistent.
 */
class BaseObject
{
public:
	/**
	 * This constructor is used in serialisation processes.
	 * It is called in CreateNewInstance in order to create
	 * an instance of the class to have Read() called on it.
	 */
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) BaseObject();
	
	/**
	 * Default destructor
	 */
	CCXX_MEMBER_EXPORT(virtual) ~BaseObject();
	
	/**
	 * This returns the ID of the persistent object (Its type)
	 */
	CCXX_MEMBER_EXPORT(virtual const char*) getPersistenceID() const;
	
	/**
	 * This method is used to write to the Persistence::Engine
	 * It is not equivalent to the << operator as it writes only the data
	 * and not the object type etc.
	 */
	CCXX_MEMBER_EXPORT(virtual bool) write(Engine& archive) const;

	/**
	 * This method is used to read from a Persistence::Engine
	 * It is not equivalent to the >> operator as it does no 
	 * typesafety or anything.
	 */
	CCXX_MEMBER_EXPORT(virtual bool) read(Engine& archive);
};


/**
 * Engine
 *
 * This class constructs on a standard C++ STL stream and then
 * operates in the mode specified. The stream passed into the
 * constructor must be a binary mode to function properly.
 *
 * @author Daniel Silverstone
 * @short stream serialization of persistent classes.
 */

class Engine
{
public:
	/**
	 * If problems happen which are fatal - expect one of these thrown 
	 * at you
	 */
	class Exception : public PersistException
	{
	public:
		CCXX_MEMBER_EXPORT(CCXX_EMPTY) Exception(const std::string &reason);
	};
	
	/**
	 * These are the modes the Persistence::Engine can work in
	 */
	enum EngineMode
	{
		modeRead,
		modeWrite
	};
	
	/**
	 * Constructs a Persistence::Engine with the specified stream in
	 * the given mode. The stream must be initialised properly prior
	 * to this call or problems will ensue.
	 */
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) Engine(std::iostream& stream, EngineMode mode) THROWS (PersistException);
	
	/**
	 * This Flushes the buffers and closes the Persistence::Engine
	 * this must happen before the underlying stream is shut down
	 */
	CCXX_MEMBER_EXPORT(virtual) ~Engine();


	// Write operations

	/**
	 * writes a BaseObject from a reference.
	 */
	CCXX_MEMBER_EXPORT(void) write(const BaseObject &object) THROWS (Exception)
  { write(&object); };

	/**
	 * writes a BaseObject from a pointer.
	 */
	CCXX_MEMBER_EXPORT(void) write(const BaseObject *object) THROWS (Exception);

	// writes supported primitive types
  // shortcut, to make the following more readable
#define CCXX_ENGINEWRITE_REF(valref) writeBinary((const uint8*)&valref,sizeof(valref))
	void write(int8 i)   THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(uint8 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(int16 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(uint16 i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(int32 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(uint32 i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(int64 i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(uint64 i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(float i)  THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
	void write(double i) THROWS (Exception) { CCXX_ENGINEWRITE_REF(i); }
#undef CCXX_ENGINEWRITE_REF
	CCXX_MEMBER_EXPORT(void) write(const std::string& str) THROWS (Exception);

	// Every write operation boils down to one or more of these
	CCXX_MEMBER_EXPORT(void) writeBinary(const uint8* data, const uint32 size) THROWS (Exception);


	// Read Operations

 	/**
	 * reads a BaseObject into a reference overwriting the object.
	 */
	CCXX_MEMBER_EXPORT(void) read(BaseObject &object) THROWS (Exception);

  /**
	 * reads a BaseObject into a pointer allocating memory for the object if necessary.
	 */
	CCXX_MEMBER_EXPORT(void) read(BaseObject *&object) THROWS (Exception);

	// reads supported primitive types
  // shortcut, to make the following more readable
#define CCXX_ENGINEREAD_REF(valref) readBinary((uint8*)&valref,sizeof(valref))
	void read(int8& i)   THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(uint8& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(int16& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(uint16& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(int32& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(uint32& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(int64& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(uint64& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(float& i)  THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
	void read(double& i) THROWS (Exception) { CCXX_ENGINEREAD_REF(i); }
#undef CCXX_ENGINEREAD_REF

	CCXX_MEMBER_EXPORT(void) read(std::string& str) THROWS (Exception);

	// Every read operation boild down to one or more of these
	CCXX_MEMBER_EXPORT(void) readBinary(uint8* data, uint32 size) THROWS (Exception);

private:
  /**
   * reads the actual object data into a pre-instantiated object pointer
   * by calling the read function of the derived class.
   */
  void readObject(BaseObject* object);

  /**
   * reads in a class name, and caches it into the ClassMap.
   */
  const std::string readClass();


	/**
	 * The underlying stream
	 */
	std::iostream& myUnderlyingStream;
	
	/**
	 * The mode of the engine. read or write
	 */
	EngineMode myOperationalMode;

	/**
	 * Typedefs for the Persistence::BaseObject support
	 */
	typedef std::vector<BaseObject*>           ArchiveVector;
	typedef std::map<BaseObject const*, int32> ArchiveMap;
	typedef std::vector<std::string>                ClassVector;
	typedef std::map<std::string, int32>            ClassMap;
	
	ArchiveVector myArchiveVector;
	ArchiveMap myArchiveMap;
	ClassVector myClassVector;
	ClassMap myClassMap;

	// Compression support
#ifndef NO_COMPRESSION
	z_stream myZStream;
	uint8* myCompressedDataBuffer;
	uint8* myUncompressedDataBuffer;
	uint8* myLastUncompressedDataRead;
#endif
};

// Standard >> and << stream operators for BaseObject
/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, BaseObject &ob)      THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, BaseObject *&ob)      THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, BaseObject const &ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, BaseObject const *ob) THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, int8& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int8 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint8& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint8 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, int16& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int16 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint16& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint16 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, int32& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int32 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint32& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint32 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, int64& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int64 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint64& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint64 ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, float& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, float ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, double& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, double ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, std::string& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, std::string ob)  THROWS (Engine::Exception);

/** @relates Engine */
CCXX_EXPORT(Engine&) operator >>( Engine& ar, bool& ob) THROWS (Engine::Exception);
/** @relates Engine */
CCXX_EXPORT(Engine&) operator <<( Engine& ar, bool ob)  THROWS (Engine::Exception);

/**
 * The following are templated classes
 */

/**
 * @relates Engine
 * serialize a vector of some serializable content to
 * the engine
 */
template<class T>
Engine& operator <<( Engine& ar, typename std::vector<T> const& ob) THROWS (Engine::Exception)
{
	ar << (uint32)ob.size();
	for(uint i=0; i < ob.size(); ++i)
		ar << ob[i];
	return ar;
}

/**
 * @relates Engine
 * deserialize a vector of deserializable content from
 * an engine.
 */
template<class T>
Engine& operator >>( Engine& ar, typename std::vector<T>& ob) THROWS (Engine::Exception)
{
	ob.clear();
	uint32 siz;
	ar >> siz;
	ob.resize(siz);
	for(uint32 i=0; i < siz; ++i)
		ar >> ob[i];
	return ar;
}

/**
 * @relates Engine
 * serialize a deque of some serializable content to
 * the engine
 */
template<class T>
Engine& operator <<( Engine& ar, typename std::deque<T> const& ob) THROWS (Engine::Exception)
{
	ar << (uint32)ob.size();
  for(typename std::deque<T>::const_iterator it=ob.begin(); it != ob.end(); ++it)
		ar << *it;
	return ar;
}

/**
 * @relates Engine
 * deserialize a deque of deserializable content from
 * an engine.
 */
template<class T>
Engine& operator >>( Engine& ar, typename std::deque<T>& ob) THROWS (Engine::Exception)
{
	ob.clear();
	uint32 siz;
	ar >> siz;
	//ob.resize(siz);
	for(uint32 i=0; i < siz; ++i)
  {
    T node;
    ar >> node;
    ob.push_back(node);
		//ar >> ob[i];
  }
	return ar;
}

/**
 * @relates Engine
 * serialize a map with keys/values which both are serializeable
 * to an engine.
 */
template<class Key, class Value>
Engine& operator <<( Engine& ar, typename std::map<Key,Value> const & ob) THROWS (Engine::Exception)
{
	ar << (uint32)ob.size();
	for(typename std::map<Key,Value>::const_iterator it = ob.begin();it != ob.end();++it)
		ar << it->first << it->second;
	return ar;
}

/**
 * @relates Engine
 * deserialize a map with keys/values which both are serializeable
 * from an engine.
 */
template<class Key, class Value>
Engine& operator >>( Engine& ar, typename std::map<Key,Value>& ob) THROWS (Engine::Exception)
{
	ob.clear();
	uint32 siz;
	ar >> siz;
	for(uint32 i=0; i < siz; ++i) {
		Key a;
		ar >> a;
		ar >> ob[a];
	}
	return ar;
}

#ifdef	CCXX_NAMESPACES
};
#endif

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */




