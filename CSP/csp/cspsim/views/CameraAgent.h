// Combat Simulator Project
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

#ifndef __CSPSIM_VIEWS_CAMERAAGENT_H__
#define __CSPSIM_VIEWS_CAMERAAGENT_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>

#include <map>

CSP_NAMESPACE

class CameraCommand;
class DynamicObject;
class TerrainObject;
class View;
class ViewFactory;

/** Primary interface for view and camera manipulation.
 */
class CameraAgent {
public:
	typedef size_t ViewMode;
	CameraAgent(const ViewFactory& vf, ViewMode default_view = 0);
	~CameraAgent();
	void attach(ViewMode mode, View* vm);
	void setViewMode(ViewMode vm);
	void setCameraCommand(CameraCommand* cc);
	void setObject(const Ref<DynamicObject> object);
	void select();
	void reselect();
	void deselect();
	void updateCamera(double dt, const TerrainObject *terrain);
	void setCameraParameters(double view_angle, double near_plane, double aspect);
	const Vector3& getEyePoint() const { return m_EyePoint; }
	const Vector3& getLookPoint() const { return m_LookPoint; }
	const Vector3& getUpVector() const { return m_UpVector; }

private:
	typedef std::map<ViewMode, Ref<View> > ViewList;
	Vector3 m_EyePoint, m_LookPoint, m_UpVector;
	ViewMode m_ViewMode;
	ViewList m_ViewList;
	double m_ViewAngle;
	double m_Aspect;
	double m_NearPlane;
	Ref<View> m_ActiveView;
	Ref<DynamicObject> m_ActiveObject;

	void validate(double dt, const TerrainObject *terrain);
	void notifyCameraKinematicsToViews();
};

CSP_NAMESPACE_END

#endif //__CAMERAAGENT_H__

