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

#include <SimData/Key.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA


const Key &Key::operator=(std::string const &id) {
	_key = newhash4_cstring(id.c_str());
	return *this;
}

bool Key::operator==(std::string const &id) const {
	return *this == Key(id);
}

void Key::pack(Packer& p) const {
	p.pack(static_cast<int>(_key));
}

void Key::unpack(UnPacker& p) {
	int k;
	p.unpack(k);
	_key = static_cast<u4>(k);
}

std::string Key::asString() const {
	char buff[32];
	sprintf(buff, "Key<%08X>", _key);
	return buff;
}


NAMESPACE_END // namespace simdata

