// Combat Simulator Project - Terrain Tools
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file dem2dat.cpp
 *
 * Converts USGS Digital Elevation Map (DEM) data to a custom
 * binary format (DAT).
 *
 * @author Mark Rose <mkrose@users.sourceforge.net>
 */


#include "dem2dat.h"

void usage() {
	std::cerr << std::endl;
	std::cerr << "Dem2Dat Digital Elevation Map to DAT converter." << std::endl;
	std::cerr << "(C) Copyright 2002, 2003 The Combat Simulator Project." << std::endl;
	std::cerr << "For more information, please visit http://csp.sourceforge.net" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Usage: dem2dat [--pgm] [--meta] [--index=n] infile" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Options:  --pgm          dump elevation map to a pgm image" << std::endl;
	std::cerr << "          --meta         dump DEM metadata" << std::endl;
	std::cerr << "          --index=#      set source index (integer id)" << std::endl;
	std::cerr << std::endl;
}


int main(int argc, char** argv) {
	DEM d;
	char *fn = 0;
	bool dump = false;
	bool pgm = false;
	int index = -1;
	while (--argc > 0) {
		char *c = argv[argc];
		if (*c == '-') {
			if (!strcmp(c, "--pgm")) { pgm = true; continue; }
			if (!strcmp(c, "--meta")) { dump = true; continue; }
			if (!strncmp(c, "--index=", 8)) { index = atoi(c+8); continue; }
			if (!strcmp(c, "--help")) {
				usage();
				::exit(0);
			}
			std::cerr << "Unknown option, try 'dem2dat --help'" << std::endl;
			::exit(0);
		} else {
			fn = c;
		}
	}

	if (fn != 0) {
		d.open(fn, index);
		if (dump) d.dump();
		if (pgm) d.dumpPGM();
		d.dump();
		if (!pgm && !dump) d.saveDAT();
	} else {
		usage();
		::exit(1);
	}
	return 0;
}

