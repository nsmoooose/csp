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
 * @file Key.cpp
 * @brief 32-bit hash key identifier class
 */

#include <SimData/Key.h>
#include <SimData/Archive.h>

#include <sstream>
#include <iomanip>


NAMESPACE_SIMDATA


const Key &Key::operator=(std::string const &id) {
	_key = newhash4_cstring(id.c_str());
	return *this;
}

bool Key::operator==(std::string const &id) const {
	return *this == Key(id);
}

void Key::serialize(Reader &reader) {
	int key;
	reader >> key;
	_key = static_cast<uint32>(key);
}

void Key::serialize(Writer &writer) const {
	writer << static_cast<int>(_key);
}

std::string Key::asString() const {
	std::stringstream ss;
	ss << "Key<0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << _key << ">";
	return ss.str();
}


NAMESPACE_SIMDATA_END

