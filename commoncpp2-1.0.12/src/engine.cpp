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

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/persist.h>
#include "assert.h"

#ifdef	CCXX_NAMESPACES
namespace ost {
using namespace std;
#endif

#ifndef NO_COMPRESSION
const uint32 MAX_BUFFER = 16384;
#endif

Engine::Exception::Exception(const std::string &reason):
	PersistException(reason) 
{
}

/**
 * NullObject is a const uint32 which is the ID streamed to disk
 * if an attempt to stream a NULL Persistence::BaseObject or
 * Derivative is made...
 */

const uint32 NullObject = 0xffffffff;

Engine::Engine(std::iostream& stream, EngineMode mode) THROWS (PersistException)
	: myUnderlyingStream(stream), myOperationalMode(mode)
{
  // Nothing else to initialise for now
#ifndef NO_COMPRESSION
  myZStream.zalloc = (alloc_func)NULL;
  myZStream.zfree = (free_func)NULL;
  myZStream.opaque = (voidpf)NULL;
  myCompressedDataBuffer = new uint8[MAX_BUFFER];
  myUncompressedDataBuffer = new uint8[MAX_BUFFER];
  myLastUncompressedDataRead = myUncompressedDataBuffer;
  if (myOperationalMode == modeRead)
	{
	  myZStream.next_in = myCompressedDataBuffer;
	  myZStream.next_out = myUncompressedDataBuffer;
	  myZStream.avail_in = 0;
	  myZStream.avail_out = MAX_BUFFER;
	  int err = inflateInit(&myZStream);
	  if (err != Z_OK) {
		  THROW (PersistException(string("zLib didn't initialise for inflating.")));
	  }
	}
  else
	{
	  myZStream.next_in = myUncompressedDataBuffer;
	  myZStream.next_out = myCompressedDataBuffer;
	  myZStream.avail_in = 0;
	  myZStream.avail_out = MAX_BUFFER;
	  int err = deflateInit(&myZStream,9); // TODO: tweak compression level
	  if (err != Z_OK) {
		  THROW ( PersistException(string("zLib didn't initialise for deflating.")));
	  }
	}
#endif
}


Engine::~Engine()
{
  // Flush compression buffers etc here.
#ifndef NO_COMPRESSION
  if (myOperationalMode == modeRead)
	{
	  inflateEnd(&myZStream);
	}
  else
	{
	  int zret = Z_OK;
	  while (myZStream.avail_in > 0 || zret == Z_OK)
		{
		  zret = deflate(&myZStream,Z_FINISH);
		  if (myZStream.avail_out >= 0)
			{
			  myUnderlyingStream.write((char*)myCompressedDataBuffer,MAX_BUFFER - myZStream.avail_out);
			  myZStream.next_out = myCompressedDataBuffer;
			  myZStream.avail_out = MAX_BUFFER;
			}
		}
	  deflateEnd(&myZStream);
	}
  delete [] myCompressedDataBuffer;
  delete [] myUncompressedDataBuffer;
#endif
}


void Engine::writeBinary(const uint8* data, const uint32 size) 
	THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeWrite);
#ifdef NO_COMPRESSION
  myUnderlyingStream.write((const char *)data,size);
#else
  // Compress the data here and doit :)
  uint32 written = 0;
  while (written < size)
	{
	  // transfer as much information as we can into the input buffer.
	  if (myZStream.avail_in < MAX_BUFFER)
		{
		  uint32 toAdd = size - written;
		  if (toAdd > (MAX_BUFFER - myZStream.avail_in))
			toAdd = (MAX_BUFFER - myZStream.avail_in);
		  memcpy(myZStream.next_in + myZStream.avail_in, data+written,toAdd);
		  written += toAdd;
		  myZStream.avail_in += toAdd;
		}
	  if (myZStream.avail_in < MAX_BUFFER)
		return; // We have not filled the buffer, so let's carry on streaming
	  // We have a full input buffer, so we compressit.
	  while (myZStream.avail_in > 0)
		{
		  deflate(&myZStream,0);
		  if (myZStream.avail_out == 0)
			{
			  // We filled the output buffer, let's stream it
			  myUnderlyingStream.write((char*)myCompressedDataBuffer,MAX_BUFFER);
			  myZStream.next_out = myCompressedDataBuffer;
			  myZStream.avail_out = MAX_BUFFER;
			}
		  // Repeat whilst the input buffer isn't flushed
		}
	  // Now we have flushed the input buffer we can reset it
	  myZStream.avail_in = 0;
	  myZStream.next_in = myUncompressedDataBuffer;
	}
#endif
}

void Engine::readBinary(uint8* data, uint32 size) THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeRead);
#ifdef NO_COMPRESSION
  myUnderlyingStream.read((char *)data,size);
#else
  uint32 read = 0;
  while (read < size)
	{
	  // If we have any data left in the uncompressed buffer - use it
	  if (myLastUncompressedDataRead < myZStream.next_out)
		{
		  uint32 toRead = size - read;
		  if (toRead > (uint32)(myZStream.next_out - myLastUncompressedDataRead))
			toRead = (myZStream.next_out - myLastUncompressedDataRead);
		  memcpy(data+read,myLastUncompressedDataRead,toRead);
		  myLastUncompressedDataRead += toRead;
		  read += toRead;
		}
	  if (read == size)
		return; // We have read all we need to
	  // Reset the stream for the next block of data
	  myLastUncompressedDataRead = myUncompressedDataBuffer;
	  myZStream.next_out = myUncompressedDataBuffer;
	  myZStream.avail_out = MAX_BUFFER;
	  // Next we have to deal such that, until we have a full output buffer,
	  // (Or we run out of input)
	  if (myUnderlyingStream.good())
		{
		  while (myUnderlyingStream.good() && myZStream.avail_out > 0)
			{
			  // Right then, if we have run out of input, fetch another chunk
			  if (myZStream.avail_in == 0)
				{
				  myZStream.next_in = myCompressedDataBuffer;
				  myUnderlyingStream.read((char*)myCompressedDataBuffer,MAX_BUFFER);
				  myZStream.avail_in = myUnderlyingStream.gcount();
				}
			  inflate(&myZStream,0);
			}
		}
	  else
		{
		  // Oh dear - we ran out of input on the buffer.
		  // Maybe we can still inflate some
		  inflate(&myZStream,0);
		  if (myZStream.avail_out == MAX_BUFFER)
//			  THROW (PersistException(string("Oh dear - ran out of input")));
			  THROW (Exception(string("Oh dear - ran out of input")));
		}
	}
#endif
}

/*
 * note, does not (yet?) throw an exception, but interface
 * prepared ..
 */
void Engine::write(const BaseObject *object) THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeWrite);

  // Pre-step, if object is NULL, then don't serialise it - serialise a
  // marker to say that it is null.
  // as ID's are uint32's, NullObject will do nicely for the task
  if (object == NULL)
	{
	  uint32 id = NullObject;
	  write(id);
	  return;
	}
  
  // First off - has this Object been serialised already?
  ArchiveMap::const_iterator itor = myArchiveMap.find(object);
  if (itor == myArchiveMap.end())
	{
	  // Unfortunately we need to serialise it - here we go ....
	  uint32 id = myArchiveMap.size();
	  myArchiveMap[object] = id; // bumps id automatically for next one
	  write(id);
	  ClassMap::const_iterator classItor = myClassMap.find(object->getPersistenceID());
	  if (classItor == myClassMap.end())
		{
		  uint32 classId = myClassMap.size();
		  myClassMap[object->getPersistenceID()] = classId;
		  write(classId);
		  write(object->getPersistenceID());
		}
	  else
		{
		  write(classItor->second);
		}
	  std::string majik;
	  majik = "OBST";
	  write(majik);
	  object->write(*this);
	  majik = "OBEN";
	  write(majik);
	}
  else
	{
	  // This object has been serialised, so just pop its ID out
	  write(itor->second);
	}
}

/*
 * reads in a BaseObject into a reference (pre-instantiated object)
 */
void Engine::read(BaseObject &object) THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeRead);
  uint32 id = 0;
  read(id);
  assert(id != NullObject); // id should never be null for a reference

  // Do we already have this object in memory?
  if (id < myArchiveVector.size())
	{
	  object = *(myArchiveVector[id]);
	  return;
	}

  // Okay - read the identifier for the class in...
  // we won't need it later since this object is already allocated
  readClass();
  
	// Okay then - we can read data straight into this object
  readObject(&object);
}

/*
 * reads in a BaseObject into a pointer allocating if the pointer is NULL
 */
void Engine::read(BaseObject *&object) THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeRead);
  uint32 id = 0;
  read(id);
  // Is the ID a NULL object?
  if (id == NullObject)
	{
	  object = NULL;
	  return;
	}

  // Do we already have this object in memory?
  if (id < myArchiveVector.size())
	{
	  object = myArchiveVector[id];
	  return;
	}

  // Okay - read the identifier for the class in...
  std::string className = readClass();
  
  // is the pointer already initialized? if so then no need to reallocate
  if (object != NULL)
  {
    readObject(object);
    return;
  }

  // Create the object (of the relevant type)
  object = TypeManager::createInstanceOf(className.c_str());
  if (object)
	{
	  // Okay then - we can make this object
    readObject(object);
	}
  else
	  THROW (Exception((std::string("Unable to instantiate object of class ")+className).c_str()));
}

/*
 * reads the actual object data in
 */
void Engine::readObject(BaseObject* object)
{
	// Okay then - we can make this object
	myArchiveVector.push_back(object);
	std::string majik;
	read(majik);
	assert(majik == string("OBST"));
	object->read(*this);
	read(majik);
	assert(majik == string("OBEN"));
}

/*
 * reads the class information in
 */
const string Engine::readClass()
{
  // Okay - read the identifier for the class in...
  uint32 classId = 0;
  read(classId);
  std::string className;
  if (classId < myClassVector.size())
	{
	  className = myClassVector[classId];
	}
  else
	{
	  // Okay the class wasn't known yet - save its name
	  read(className);
	  myClassVector.push_back(className);
	}

  return className;
}


/*
 * note, does not (yet?) throw an exception, but interface
 * prepared ..
 */
void Engine::write(const std::string& str) THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeWrite);
  uint32 len = str.length();
  write(len);
  writeBinary((uint8*)str.c_str(),len);
}

void Engine::read(std::string& str) THROWS (Engine::Exception)
{
  assert(myOperationalMode == modeRead);
  uint32 len = 0;
  read(len);
  uint8 *buffer = new uint8[len+1];
  readBinary(buffer,len);
  buffer[len] = 0;
  str = (char*)buffer;
  delete[] buffer;
}

#define CCXX_RE(ar,ob)   ar.read(ob); return ar
#define CCXX_WE(ar,ob)   ar.write(ob); return ar

CCXX_EXPORT(Engine&) operator >>( Engine& ar, BaseObject &ob) THROWS (Engine::Exception) {CCXX_RE(ar,ob);}
CCXX_EXPORT(Engine&) operator >>( Engine& ar, BaseObject *&ob) THROWS (Engine::Exception) {CCXX_RE(ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, BaseObject const &ob) THROWS (Engine::Exception) {CCXX_WE(ar,&ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, BaseObject const *ob) THROWS (Engine::Exception) {CCXX_WE(ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int8& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int8 ob) THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint8& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint8 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int16& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int16 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint16& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint16 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int32& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int32 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint32& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint32 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, int64& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, int64 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, uint64& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, uint64 ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, float& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, float ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, double& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, double ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, std::string& ob) THROWS (Engine::Exception) {CCXX_RE (ar,ob);}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, std::string ob)  THROWS (Engine::Exception) {CCXX_WE (ar,ob);}

CCXX_EXPORT(Engine&) operator >>( Engine& ar, bool& ob) THROWS (Engine::Exception) {
	uint32 a; ar.read(a); ob=a==1;return ar;
}
CCXX_EXPORT(Engine&) operator <<( Engine& ar, bool ob) THROWS (Engine::Exception) {
	uint32 a=ob?1:0; ar.write(a); return ar;
}

#undef CCXX_RE
#undef CCXX_WE

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
