#pragma once
// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file F16InputSystem.h
 *
 * @todo F16InputSystem is not currently included in the build. Is it still needed?
 **/

#include <csp/cspsim/systems/AircraftInputSystem.h>

namespace csp {

class F16InputSystem: public AircraftInputSystem {
public:
	CSP_DECLARE_OBJECT(F16InputSystem)

	F16InputSystem();

public:
	virtual void setAirbrake(double);
	virtual void OpenAirbrake();
	virtual void CloseAirbrake();
	virtual void IncAirbrake();
	virtual void DecAirbrake();

};

} // namespace csp
