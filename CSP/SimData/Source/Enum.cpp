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
 * @file Enum.cpp
 * @brief Enumeration classes for C++ and Python.
 * @author Mark Rose <mrose@stm.lbl.gov>
 */


#include <SimData/Enum.h>
#include <SimData/Archive.h>


NAMESPACE_SIMDATA


// class Enum

void EnumLink::serialize(Reader &reader) {
	std::string token;
	reader >> token;
	set(token);
}

void EnumLink::serialize(Writer &writer) const {
	writer << getToken();
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



