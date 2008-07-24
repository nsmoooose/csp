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
 * @file Key.cpp
 * @brief 32-bit hash key identifier class
 */

#include <csp/csplib/data/Key.h>
#include <csp/csplib/data/Archive.h>

#include <sstream>
#include <iomanip>


namespace csp {


const Key &Key::operator=(std::string const &id) {
	_key = newhash4_cstring(id.c_str());
	if (_key == 0) _key = 1;  // 0 reserved for uninitialized
	return *this;
}

bool Key::operator==(std::string const &id) const {
	return *this == Key(id);
}

void Key::serialize(Reader &reader) {
	reader >> _key;
}

void Key::serialize(Writer &writer) const {
	writer << _key;
}

std::string Key::asString() const {
	std::ostringstream ss;
	ss << "Key<0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << _key << ">";
	return ss.str();
}

std::ostream &operator <<(std::ostream &o, Key const &k) { return o << k.asString(); }

} // namespace csp

