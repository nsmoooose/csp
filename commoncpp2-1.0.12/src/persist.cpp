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

#ifdef	COMMON_STD_EXCEPTION

PersistException::PersistException(string const &what) :
Exception(what)
{}

#else

PersistException::PersistException(string const& reason)
  : _what(reason)
{
  // Nothing :)
}

#endif

PersistException::~PersistException()
{
  // Nothing :)
}

string const& PersistException::getString() const
{
  return _what;
}

const char* BaseObject::getPersistenceID() const
{
  return "BaseObject";
}

BaseObject::BaseObject()
{
  // Do nothing
}

BaseObject::~BaseObject()
{
  // Do nothing
}

bool BaseObject::write(Engine& archive) const
{
  // Do nothing
  return true; // Successfully
}

bool BaseObject::read(Engine& archive)
{
  // Do nothing
  return true; // Successfully
}

static TypeManager::StringFunctionMap* theInstantiationFunctions = 0;
static int refCount = 0;

TypeManager::StringFunctionMap& _internal_GetMap()
{
	return *theInstantiationFunctions;
}
void TypeManager::add(const char* name, NewBaseObjectFunction construction)
{
  if (refCount++ == 0)
  {
    theInstantiationFunctions = new StringFunctionMap;
  }
  assert(_internal_GetMap().find(string(name)) == _internal_GetMap().end());
  _internal_GetMap()[string(name)] = construction;
}

void TypeManager::remove(const char* name)
{
  assert(_internal_GetMap().find(string(name)) != _internal_GetMap().end());
  _internal_GetMap().erase(_internal_GetMap().find(string(name)));
  if (--refCount == 0)
  {
    delete theInstantiationFunctions;
	theInstantiationFunctions = 0;
  }
}

BaseObject* TypeManager::createInstanceOf(const char* name)
{
  assert(refCount);
  assert(_internal_GetMap().find(string(name)) != _internal_GetMap().end());
  return (_internal_GetMap()[string(name)])();
}

#ifdef	CCXX_NAMESPACES
};
#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
