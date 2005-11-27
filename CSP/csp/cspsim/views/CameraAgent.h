// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file CameraAgent.h
 *
 **/

#ifndef __CAMERAAGENT_H__
#define __CAMERAAGENT_H__

#include <map>

#include <csp/lib/util/Ref.h>
#include <csp/lib/data/Vector3.h>

#include "Views/CameraKinematics.h"
#include "Views/View.h"

class CameraCommand;
class DynamicObject;

class CameraAgent {
	typedef size_t ViewMode;
	typedef std::map<ViewMode, View::Ref> ViewList;
	simdata::Vector3 m_EyePoint, m_LookPoint, m_UpVector;
	View::Ref m_ActiveView;
	ViewMode m_ViewMode;
	ViewList m_ViewList;
	simdata::Ref<DynamicObject> m_ActiveObject;

	void validate(double dt);
	void notifyCameraKinematicsToViews();

	template <class T> class Accept {
		T m_t;
	public:
		Accept(const T t): m_t(t) {}
		void operator()(std::pair<const size_t, CameraKinematics*>& vm) const {
			vm.second->accept(m_t);
		}
	};
public:
	CameraAgent(const ViewFactory& vf, ViewMode default_view = 0);
	~CameraAgent();
	void attach(ViewMode mode, View* vm);
	void setViewMode(ViewMode vm);
	void setCameraCommand(CameraCommand* cc);
	void setObject(const simdata::Ref<DynamicObject> object);
	void select();
	void reselect();
	void deselect();
	void updateCamera(double dt);
	const simdata::Vector3& getEyePoint() const { return m_EyePoint; }
	const simdata::Vector3& getLookPoint() const { return m_LookPoint; }
	const simdata::Vector3& getUpVector() const { return m_UpVector; }
};

#endif //__CAMERAAGENT_H__

