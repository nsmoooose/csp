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


#include <SimData/Pack.h>


NAMESPACE_SIMDATA


#if 0
void Packable::pack(Packer&) const {
}

void Packable::unpack(UnPacker&) {
}

void Packable::parseXML(const char* cdata) {
	std::string s(cdata); 
	if (s.find_first_not_of("\t \r\n") == std::string::npos) return;
	throw ParseException("WARNING: #cdata ignored");
}


void Packable::convertXML() {
}
#endif


NAMESPACE_END // namespace simdata

