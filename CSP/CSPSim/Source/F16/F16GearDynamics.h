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
 * @file F16GearDynamics.h
 *
 **/


#ifndef __F16_GEARDYNAMICS_H__
#define __F16_GEARDYNAMICS_H__

#include <LandingGear.h>


class F16GearDynamics: public GearDynamics {
public:
	SIMDATA_OBJECT(F16GearDynamics, 0, 0)

	EXTEND_SIMDATA_XML_INTERFACE(F16GearDynamics, GearDynamics)
	END_SIMDATA_XML_INTERFACE

	// input event handlers
	virtual void GearUp();
	virtual void GearDown();
	virtual void GearToggle();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual void preSimulationStep(double dt);
	virtual void postCreate();
	virtual void setSteering(double x, double link_brakes=1.0);


protected:
	DataChannel<bool>::Ref b_WheelSpin;
	DataChannel<bool>::Ref b_GearHandleUp;
	simdata::Ref<LandingGear> m_NoseLandingGear;
	simdata::Ref<LandingGear> m_LeftMainLandingGear;
	simdata::Ref<LandingGear> m_RightMainLandingGear;
};


#endif // __F16_GEARDYNAMICS_H__

