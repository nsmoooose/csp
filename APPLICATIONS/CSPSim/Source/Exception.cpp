// Combat Simulator Project - FlightSim Demo
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
 * @file Exception.cpp
 *
 **/


#include "Exception.h"
#include "Log.h"

#include <cstdio>
#include <iostream>

#include "DemeterException.h"
#include <SimData/ExceptionBase.h>


namespace csp {

void FatalException(Exception &e, std::string const &location) {
	DataError *pDataError = dynamic_cast<DataError*> (&e);
	CSP_LOG(APP, ERROR, "CSPSim: caught exception in " << location << ": " << e.getMessage());
	std::cerr << "\n";
	e.details();
	std::cerr << "\n";
	if (pDataError) {
		std::cerr << "Please check that the data paths in CSPSim.ini are set to reasonable\n"
		          << "values (CSPSim.ini is usually found in CSPSim/Data).  If you appear\n"
			  << "to be missing a necessary data file, please check the CSP download\n"
			  << "page for the latest data releases or ask for assistance on the CSP\n"
			  << "forums:\n"
			  << "                http://csp.sourceforge.net/forum\n";
	}
	else {
		std::cerr << "CSPSim: caught an exception.  Please report this along with\n"
		          << "as much information as possible about what was happening at \n"
			  << "the time of the error to the CSP forum at\n"
			  << "              http://csp.sourceforge.net/forum\n";
	}
	std::cerr << "\n";
	::exit(1);
}

void DemeterFatalException(DemeterException &e,  std::string const &location) {
	CSP_LOG(APP, ERROR, "CSPSim: caught Demeter exception in " 
	                            << location << ": " << e.GetErrorMessage());
	std::cerr << "\n";
	std::cerr << "CSPSim: caught an Demeter exception.  Please report this along\n"
		  << "with as much information as possible about what was happening at \n"
		  << "the time of the error to the CSP forum at\n"
		  << "                http://csp.sourceforge.net/forum\n";
	std::cerr << "\n";
	::exit(1);
}

void SimDataFatalException(simdata::Exception &e,  std::string const &location) {
	CSP_LOG(APP, ERROR, "CSPSim: caught SimData exception in " 
	                            << location << ": " << e.getMessage());
	std::cerr << "\n";
	e.details();
	std::cerr << "\n";
	std::cerr << "CSPSim: caught an SimData exception.  Please report this along\n"
		  << "with as much information as possible about what was happening at \n"
		  << "the time of the error to the CSP forum at\n"
		  << "                http://csp.sourceforge.net/forum\n"
		  << "If you built CSPSim from CVS, you may wish to check that your\n"
		  << "installed version of SimData is also the latest available from\n"
		  << "CVS.\n";
	std::cerr << "\n";
	::exit(1);
}

void OtherFatalException(std::string const &location) {
	CSP_LOG(APP, ERROR, "CSPSim: caught unknown exception in " << location << ".");
	std::cerr << "\n";
	std::cerr << "CSPSim: caught an (unknown) exception.  Please report this along\n"
	          << "with as much information as possible about what was happening at \n"
		  << "the time of the error to the CSP forum at\n"
		  << "                http://csp.sourceforge.net/forum\n";
	std::cerr << "\n";
	::exit(1);
}

} // namespace csp

