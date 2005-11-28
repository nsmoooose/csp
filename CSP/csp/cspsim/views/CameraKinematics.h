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
 * @file CameraKinematics.h
 *
 **/

#ifndef __CSPSIM_VIEWS_CAMERAKINEMATICS_H__
#define __CSPSIM_VIEWS_CAMERAKINEMATICS_H__

#include <csp/csplib/util/Ref.h>

CSP_NAMESPACE

class CameraCommand;

class CameraKinematics: public Referenced {
	// XXX: serialize
	const double m_BaseRate;
	const double m_DisplacementCoefficient;
	const double m_MinimumDistanceOffset;
	const double m_AbsoluteMaximumDistance;
	double m_MinimumDistance;
	double m_DistanceToObject;
	double m_ObjectSize;

	double m_ViewAngle;
	double m_NearPlane;
	double m_Aspect;

	double m_Phi, m_Theta;
	double m_FOVScale;
	double m_PanRatePhi, m_PanRateTheta, m_ZoomRate;
	double m_Accel;
	bool m_ExternalPan;
	void rotateTheta(double dt) { m_Theta += m_Accel * m_PanRateTheta * dt * m_FOVScale; }
	void rotatePhi(double dt) { m_Phi += m_Accel * m_PanRatePhi * dt * m_FOVScale; }
	void scale(double dt);
	double smooth(double value, double min_value,double max_value) const;

public:
	CameraKinematics();
	virtual ~CameraKinematics() {}
	void clampPhi(double min_phi,double max_phi, bool smooth_on = true);
	void clampTheta(double min_theta,double max_theta, bool smooth_on = true);
	void reset();
	void update(double dt);
	void panLeft();
	void panRight();
	void panLeftRightStop();
	void panUp();
	void panDown();
	void panUpDownStop();
	void zoomIn();
	void zoomOut();
	void zoomStop();
	void zoomStepIn();
	void zoomStepOut();
	void lookForward() { m_Theta = 0.0; m_Phi = 0.0; m_ExternalPan = true; }
	void lookBackward() { m_Theta = PI; m_Phi = 0.0; m_ExternalPan = true; }
	void lookRight() { m_Theta = -PI_2; m_Phi = 0.0; m_ExternalPan = true; }
	void lookLeft() { m_Theta = PI_2; m_Phi = 0.0; m_ExternalPan = true; }
	void displacement(int x, int y, int dx, int dy);
	inline void setPhi(double phi) { m_Phi = phi; }
	inline double getPhi() const { return m_Phi; }
	inline void setTheta(double theta) { m_Theta = theta; }
	inline double getTheta() const { return m_Theta; }
	inline void setDistance(double d) { m_DistanceToObject = d; }
	inline double getDistance() const { return m_DistanceToObject; }
	void accept(CameraCommand* cc);
	bool externalPan() const { return m_ExternalPan; }
	void resetExternalPan() { m_ExternalPan = false; }
	void setObjectSize(double size);
	void setCameraParameters(double view_angle, double near_plane, double aspect);
};

CSP_NAMESPACE_END

#endif // __CSPSIM_VIEWS_CAMERAKINEMATICS_H__

