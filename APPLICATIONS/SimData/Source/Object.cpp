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
#include <SimData/Archive.h>

#include <sstream>


NAMESPACE_SIMDATA


void Object::serialize(Archive&) {
}

Object::Object(): Referenced(), BaseType(), _path(0) {
}

Object::~Object() { 
	//std::cerr << "~Object 0x" << std::hex << static_cast<unsigned int>(this) << "\n"; 
}

std::string Object::asString() const {
	std::ostringstream tag;
	tag << "<" << getClassName() << " Object instance [" << getClassHash() << "] at " << (void*)this << ">";
	return tag.str();
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


NAMESPACE_SIMDATA_END

