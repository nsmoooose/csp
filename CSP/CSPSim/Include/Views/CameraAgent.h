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

#include <SimData/Ref.h>
#include <SimData/Vector3.h>

#include "Views/CameraKinematics.h"
#include "Views/View.h"

class CameraCommand;
class DynamicObject;

class CameraAgent {
	simdata::Vector3 m_EyePoint, m_LookPoint, m_UpVector;
	CameraKinematics m_CameraKinematics;
	size_t m_ViewMode;
	ViewList m_ViewList;
	void validate(double dt);
	void deleteViews();
	void notifyCameraKinematicsToViews();

	struct DestroyView {
		void operator()(std::pair<const size_t, View*>& vm) const {
			delete vm.second;
		}
	};

	template <class T> class Accept {
		T m_t;
	public:
		Accept(const T t): m_t(t) {}
		void operator()(std::pair<const size_t, View*>& vm) const {
			vm.second->accept(m_t);
		}
	};
public:
	CameraAgent(const ViewFactory& vf);
	~CameraAgent();
	void attach(size_t mode, View* vm);
	void setViewMode(size_t vm);
	void setCameraCommand(CameraCommand* cc);
	void setObject(const simdata::Ref<DynamicObject> object);
	void updateCamera(double dt);
	const simdata::Vector3& getEyePoint() const { return m_EyePoint; }
	const simdata::Vector3& getLookPoint() const { return m_LookPoint; }
	const simdata::Vector3& getUpVector() const { return m_UpVector; }
};

#endif //__CAMERAAGENT_H__

