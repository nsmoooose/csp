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
 * @file AircraftSimpleFCS.h
 *
 **/


#ifndef __CSPSIM_AIRCRAFTSIMPLEFCS_H__
#define __CSPSIM_AIRCRAFTSIMPLEFCS_H__

#include <csp/cspsim/System.h>
#include <csp/csplib/util/ScopedPointer.h>

#include <algorithm>
#include <cmath>

CSP_NAMESPACE

class AircraftSimpleFCS: public System {
public:
	CSP_DECLARE_OBJECT(AircraftSimpleFCS)

	AircraftSimpleFCS();

protected:
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);
	
	virtual void convertXML();
	virtual void postCreate();
	virtual double onUpdate(double dt);
	virtual void getInfo(InfoList &info) const;

private:
	class Deflection;

	ScopedPointer<Deflection> m_Elevator;
	ScopedPointer<Deflection> m_Aileron;
	ScopedPointer<Deflection> m_Rudder;
	ScopedPointer<Deflection> m_Airbrake;

	double m_ElevatorLimit0,m_ElevatorLimit1;
	double m_AileronLimit0, m_AileronLimit1;
	double m_RudderLimit;
	double m_AirbrakeLimit;
	double m_ElevatorRate;
	double m_AileronRate;
	double m_RudderRate;
	double m_AirbrakeRate;
	double m_MaxG;
	double m_MinG;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_AICRAFTSIMPLEFCS_H__

