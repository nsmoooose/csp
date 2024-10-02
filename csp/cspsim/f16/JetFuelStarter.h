#pragma once
// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file JetFuelStarter.h
 *
 **/

#include <csp/csplib/data/Object.h>

namespace csp {

class JetFuelStarter: public Object {
public:
	CSP_DECLARE_OBJECT(JetFuelStarter)

	JetFuelStarter();

	virtual double getFuelDemand(double dt) const;
	virtual void update(double dt, double fuel);
	virtual void start(double accumulator, double altitude, double airspeed);
	virtual void deactivate();
	virtual double getDrive() const;
	virtual bool isActive() const;

protected:
	void enable(bool on);
	bool isEnabled() const;

private:
	bool ignite(double accumulator, double altitude, double airspeed);

	// state
	bool m_Enabled;
	double m_Temperature;
	double m_Drive;

	// xml interface
	double m_Power;
	double m_DriveRate;
	double m_StartChance;
	double m_HeatRate;
	double m_CoolRate;
	double m_ShutdownTemp;
	double m_SpinupRate;
	double m_SpindownRate;
	double m_MaxFuelFlow;
};

} // namespace csp
