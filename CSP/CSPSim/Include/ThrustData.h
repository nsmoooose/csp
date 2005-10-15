// Combat Simulator Project
// Copyright (C) 2002, 2005 The Combat Simulator Project
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
 * @file ThrustData.h
 *
 **/

#ifndef __THRUSTDATA_H__
#define __THRUSTDATA_H__

#include <SimData/LUT.h>
#include <SimData/Object.h>


/** Static thrust data for an aircraft engine.  Data is stored in three tables:
 *  idle, military, and afterburner thrust.  Each table is a function of altitude
 *  and mach number.  Actual thrust is computed by interpolating between these
 *  tables as a function of engine rpm.
 */
class ThrustData: public simdata::Object {
	simdata::Table2 m_IdleThrust, m_MilitaryThrust, m_AfterburnerThrust;
	bool m_HasAfterburner;

public:
	SIMDATA_DECLARE_STATIC_OBJECT(ThrustData)

	double getMil(double altitude, double mach) const;
	double getIdle(double altitude, double mach) const;
	double getAb(double altitude, double mach) const;
	bool hasAfterburner() const { return m_HasAfterburner; }

	ThrustData();
	virtual ~ThrustData();

protected:
	virtual void postCreate();
};


#endif // __THRUSTDATA_H__

