// Combat Simulator Project - CSPSim
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


#ifndef __AIRCRAFTFLIGHTSENSORS_H__
#define __AIRCRAFTFLIGHTSENSORS_H__

#include <System.h>

class AircraftFlightSensors: public System {
public:
	SIMDATA_OBJECT(AircraftFlightSensors, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(AircraftFlightSensors, System)
	END_SIMDATA_XML_INTERFACE

protected:
	double onUpdate(double dt);
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);
	virtual void getInfo(InfoList &info) const;

	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<simdata::Vector3>::CRef b_Velocity;
	DataChannel<simdata::Vector3>::Ref b_WindVelocity;
	DataChannel<double>::Ref b_Temperature;
	DataChannel<double>::Ref b_Pressure;
	DataChannel<double>::Ref b_Density;
	DataChannel<double>::Ref b_Mach;
	DataChannel<double>::Ref b_CAS;

private:
	double m_Distance;
};

#endif // __AIRCRAFTFLIGHTSENSORS_H__
