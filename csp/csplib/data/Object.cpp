/* Combat Simulator Project
 * Copyright 2002, 2003, 2004 Mark Rose <mkrose@users.sourceforge.net>
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
 * @file Object.cpp
 * @brief Object class and related boilerplate macros.
 */


#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Archive.h>

#include <sstream>


CSP_NAMESPACE


void Object::serialize(Reader &reader) {
	_serialize(reader);
}

void Object::serialize(Writer &writer) const {
	_serialize(writer);
}

Object::Object(): Referenced(), _path(0) {
}

Object::~Object() {
	//std::cerr << "~Object 0x" << std::hex << static_cast<unsigned int>(this) << "\n";
}

std::string Object::asString() const {
	std::ostringstream tag;
	tag << "<" << getClassName() << " Object instance [" << getClassHash() << "] at " << (void*)this << ">";
	return tag.str();
}

void Object::_setPath(hasht path) {
	_path = path;
}

hasht Object::getPath() const {
	return _path;
}

std::ostream &operator <<(std::ostream &o, Object const &obj) { return o << obj.asString(); }

CSP_NAMESPACE_END

