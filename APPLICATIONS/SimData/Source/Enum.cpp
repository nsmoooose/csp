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


#include <SimData/Enum.h>
#include <SimData/Pack.h>


NAMESPACE_SIMDATA


// class Enum

void EnumLink::pack(Packer& p) const {
	p.pack(getToken());
}

void EnumLink::unpack(UnPacker& p) {
	std::string token;
	p.unpack(token);
	set(token);
}

void EnumLink::parseXML(const char* cdata) {
	std::string token(cdata);
	set(token);
}

std::string EnumLink::asString() const {
	return __repr__();
}

std::string EnumLink::typeString() const {
	return "type::Enum";
}


NAMESPACE_SIMDATA_END


/*
USING_SIMDATA
Enumeration AirSources("OFF NORM DUMP RAM");
Enumeration Power("POWER OFF ON");

main() {
	EnumLink source(AirSources);
	EnumLink power(Power);
	source = "NORM";
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycle();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycle();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycle();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycle();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycleBack();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycleBack();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycleBack();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycleBack();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;
	source.cycleBack();
	std::cout << "source = << source.getName() << ", " << source.getIndex() << std::endl;

	power = "OFF";
	if (power == Power["OFF"]) printf("equal\n");
};
*/



