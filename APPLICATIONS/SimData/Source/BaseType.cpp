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

#include <string>

#include <SimData/BaseType.h>


NAMESPACE_SIMDATA


BaseType::~BaseType() {
}

void BaseType::pack(Packer&) const {
}

void BaseType::unpack(UnPacker&) {
}

void BaseType::parseXML(const char* cdata) {
	std::string s(cdata); 
	if (s.find_first_not_of("\t \r\n") == std::string::npos) return;
	throw ParseException("WARNING: #cdata ignored");
}


void BaseType::convertXML() {
}


std::string BaseType::asString() const {
	return "";
}

std::string BaseType::typeString() const {
	return "";
}

std::ostream &operator <<(std::ostream &o, BaseType const &t) {
	return o << t.asString();
}

NAMESPACE_SIMDATA_END

