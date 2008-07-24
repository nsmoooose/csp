// Combat Simulator Project
// Copyright (C) 2002, 2003 The Combat Simulator Project
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
 * @file AircraftFlightSensors.h
 *
 **/


#ifndef __CSPSIM_SYSTEMS_AIRCRAFTFLIGHTSENSORS_H__
#define __CSPSIM_SYSTEMS_AIRCRAFTFLIGHTSENSORS_H__

#include <csp/cspsim/System.h>
#include <csp/csplib/data/Vector3.h>

namespace csp {

class AircraftFlightSensors: public System {
public:
	CSP_DECLARE_OBJECT(AircraftFlightSensors)

	AircraftFlightSensors();

protected:
	virtual double onUpdate(double dt);
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);
	virtual void getInfo(InfoList &info) const;

	DataChannel<Vector3>::CRefT b_Position;
	DataChannel<Vector3>::CRefT b_Velocity;
	DataChannel<Vector3>::RefT b_WindVelocity;
	DataChannel<double>::RefT b_Temperature;
	DataChannel<double>::RefT b_Pressure;
	DataChannel<double>::RefT b_Density;
	DataChannel<double>::RefT b_Mach;
	DataChannel<double>::RefT b_CAS;
	DataChannel<double>::RefT b_PressureAltitude;
	DataChannel<double>::RefT b_VerticalVelocity;

private:
	double m_Distance;
};

} // namespace csp

#endif // __CSPSIM_SYSTEMS_AIRCRAFTFLIGHTSENSORS_H__

