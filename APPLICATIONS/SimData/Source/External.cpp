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

#include <SimData/External.h>
#include <SimData/Pack.h>
#include <SimData/FileUtility.h>


NAMESPACE_SIMDATA


External::~External() {
}

const External &External::operator=(std::string const &path) {
	setSource(path.c_str());
	return *this;
}

const External &External::operator=(External const &path) {
	_native_path = path._native_path;
	_path = path._path;
	return *this;
}

void External::setSource(const char* path) {
	_native_path.assign(path);
	_path = ospath::normalize(path);
}

const std::string& External::getSource() const {
	return _native_path;
}

void External::pack(Packer& p) const {
	p.pack(_path);
}

void External::unpack(UnPacker& p) {
	p.unpack(_path);
	_native_path = ospath::filter(_path.c_str());
}

std::string External::asString() const {
	return _path;
}


NAMESPACE_END // namespace simdata

