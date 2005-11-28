// Combat Simulator Project
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

#ifndef __CSPSIM_VIEWS_VIEW_H__
#define __CSPSIM_VIEWS_VIEW_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE

class CameraAgent;
class CameraKinematics;
class DynamicObject;
class SimObject;


/** Base class for different viewing modes. */
class View: public Referenced {
private:
	const bool m_Internal;
	const size_t m_ViewMode;

protected:
	Ref<CameraKinematics> m_CameraKinematics;
	Ref<DynamicObject> m_ActiveObject;
	void updateBody(Vector3& ep, Vector3& lp, Vector3& up);
	void updateWorld(Vector3& ep, Vector3& lp, Vector3& up);
	virtual void updateView(Vector3& ep, Vector3& lp, Vector3& up, double dt)=0;

	typedef Ref<SimObject> Contact;
	void findContacts(Vector3 const &dir, double cutoff_angle, std::vector<Contact> &result) const;

public:
	View(size_t vm, bool internal);
	virtual void activate();
	virtual void reactivate() { activate(); }
	void update(Vector3& ep, Vector3& lp, Vector3& up, double dt);
	void setActiveObject(const Ref<DynamicObject> object);

	virtual void recalculate(Vector3& ep, Vector3& lp, Vector3& up, double dt) {
		update(ep, lp, up, dt);
	}

	virtual void select() {}
	virtual void reselect() { select(); }
	virtual void deselect() {}

	virtual CameraKinematics *kinematics() { return m_CameraKinematics.get(); }

	void cull();
	virtual ~View();

	inline bool isInternal() const { return m_Internal; }
	void setCameraParameters(double view_angle, double near_plane, double aspect);
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

CSP_NAMESPACE_END

#endif // __CSPSIM_VIEWS_VIEW_H__

