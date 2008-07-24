// Combat Simulator Project
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

#ifndef __CSPSIM_EXTERNAL_VIEWS_H__
#define __CSPSIM_EXTERNAL_VIEWS_H__

#include <csp/cspsim/views/View.h>
#include <csp/csplib/data/Vector3.h>

namespace csp {

class ExternalViewBody: public View {
public:
	ExternalViewBody(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(Vector3& ep, Vector3& lp, Vector3& up, double dt);
	virtual ~ExternalViewBody() { }
};


class ExternalViewWorld: public View {
public:
	ExternalViewWorld(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(Vector3& ep, Vector3& lp, Vector3& up, double dt);
	virtual ~ExternalViewWorld() { }
};


class FlybyView: public View {
protected:
	Vector3 m_FixedCameraPosition;
	virtual void newFixedCamPos(SimObject* target);
public:
	FlybyView(size_t vm): View(vm, false) { }
	virtual void activate();
	virtual void updateView(Vector3& ep, Vector3& lp, Vector3& up, double dt);
	virtual void recalculate(Vector3& ep, Vector3& lp, Vector3& up, double dt);
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
	virtual void updateView(Vector3& ep, Vector3& lp, Vector3& up, double dt);
	virtual ~SatelliteView() { }
};

} // namespace csp

#endif // __CSPSIM_EXTERNAL_VIEWS_H__

