// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004-2005 The Combat Simulator Project
// http: //csp.sourceforge.net
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
 * @file ExternalViews.h
 *
 **/

#ifndef __EXTERNAL_VIEWS_H__
#define __EXTERNAL_VIEWS_H__

#include <SimData/Vector3.h>
#include "Views/View.h"


class ExternalViewBody: public View {
public:
	ExternalViewBody(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	virtual ~ExternalViewBody() { }
};


class ExternalViewWorld: public View {
public:
	ExternalViewWorld(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	virtual ~ExternalViewWorld() { }
};


class FlybyView: public View {
protected:
	simdata::Vector3 m_FixedCameraPosition;
	virtual void newFixedCamPos(SimObject* target);
public:
	FlybyView(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	virtual void recalculate(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	virtual ~FlybyView() { }
};


class FixedFlybyView: public FlybyView {
	bool m_Initialized;
	virtual void newFixedCamPos(SimObject* target);
public:
	FixedFlybyView(size_t vm): FlybyView(vm), m_Initialized(false) {}
	virtual void activate();
	virtual ~FixedFlybyView() { }
};


class SatelliteView: public View {
public:
	SatelliteView(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	virtual ~SatelliteView() { }
};


#endif // __EXTERNAL_VIEWS_H__

