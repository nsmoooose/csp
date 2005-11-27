// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file View.h
 *
 **/

#ifndef __CSPSIM_VIEW_H__
#define __CSPSIM_VIEW_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/data/Vector3.h>

class CameraAgent;
class CameraKinematics;
class DynamicObject;
class SimObject;


/** Base class for different viewing modes. */
class View: public simdata::Referenced {
private:
	const bool m_Internal;
	const size_t m_ViewMode;

protected:
	simdata::Ref<CameraKinematics> m_CameraKinematics;
	simdata::Ref<DynamicObject> m_ActiveObject;
	void updateBody(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up);
	void updateWorld(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up);
	virtual void updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt)=0;

	typedef simdata::Ref<SimObject> Contact;
	void findContacts(simdata::Vector3 const &dir, double cutoff_angle, std::vector<Contact> &result) const;

public:
	typedef simdata::Ref<View> Ref;

	View(size_t vm, bool internal);
	virtual void activate();
	virtual void reactivate() { activate(); }
	void update(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	void setActiveObject(const simdata::Ref<DynamicObject> object);

	virtual void recalculate(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt) {
		update(ep, lp, up, dt);
	}

	virtual void select() {}
	virtual void reselect() { select(); }
	virtual void deselect() {}

	virtual CameraKinematics *kinematics() { return m_CameraKinematics.get(); }

	void cull();
	virtual ~View();

	inline bool isInternal() const { return m_Internal; }
};


class ViewFactory {
	View* createView_1() const;
	View* createView_2() const;
	View* createView_3() const;
	View* createView_4() const;
	View* createView_5() const;
	View* createView_7() const;
	View* createView_8() const;
	View* createView_9() const;
public:
	void attachAllView(CameraAgent* ca) const;
};

#endif //__VIEW_H__

