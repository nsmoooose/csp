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

#include <SimData/Object.h>

#include <sstream>


NAMESPACE_SIMDATA


//INIT_OBJECTS

/*
void releaseObject(Object* ptr, int force) {
	if (!ptr) return;
	if (!force && ptr->isStatic()) return;
	printf("delete %p\n", ptr);
	delete ptr;
}
*/

// archive serializaiton
void Object::pack(Packer& p) const {
	p.pack(_static);
}

void Object::unpack(UnPacker& p) {
	p.unpack(_static);
} 

// static management (don't touch!)
void Object::setStatic(int s) { 
	_static = s;
}

int Object::isStatic() const { 
	return _static; 
}

Object::Object(): BaseType(), _static(0), _count(0), _path(0) {
}

// Objects should never be copied
// Object(const Object& o) { assert(0); }

Object::Object(const Object& o): BaseType(), _count(0) {
	_path = o._path;
	_static = o._static; 
}

#ifndef SWIG
Object& Object::operator=(const Object& o) { 
	//assert(0); 
	_path = o._path;
	_static = o._static;
	return *this;
}
#endif

Object::~Object() { 
	//printf("~Object %p\n", this); 
}

/*
 * Should access to the reference count be protected by a
 * mutex, or just let the users of the object deal with
 * mt issues?  Since reference counts may be changed at
 * the start and end of methods due to Pointer 
 * instantiation and destruction, it may be wise to wrap
 * the reference counting operations here.
 */

// reference counting helpers (may move to proxy class)
unsigned Object::ref() const { 
	//printf("ref(%p): %d\n", this, _count); 
	return ++_count; 
}

unsigned Object::deref() const { 
	//printf("deref(%p): %d\n", this, _count); 
	assert(_count != 0); 
	return --_count; 
}

std::string Object::asString() const {
	std::ostringstream tag;
	tag << "<" << getClassName() << " Object instance [" << getClassHash() << "] at " << (void*)this << ">";
	return tag.str();
}

unsigned Object::getCount() const { 
	return _count; 
}

hasht Object::_getHash(const char* c) {
	return hash_string(c); 
}

void Object::_setPath(hasht path) {
	_path = path;
}

hasht Object::getPath() const {
	return _path;
}


NAMESPACE_END // namespace simdata

