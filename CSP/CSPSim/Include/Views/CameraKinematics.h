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
 * @file CameraKinematics.h
 *
 **/

#ifndef __CAMERAKINEMATICS_H__
#define __CAMERAKINEMATICS_H__

#include <deque>

class CameraCommand;

class CameraKinematics {
	// XXX: serialize
	const float m_BaseRate, m_DisplacementCoefficient;
	const float m_MinimumDistanceOffset, m_AbsoluteMaximumDistance;

	double m_Phi, m_Theta;
	float m_PanRatePhi, m_PanRateTheta, m_ZoomRate;
	double m_DistanceToObject, m_MinimumDistance;
	void rotateTheta(double dt) {m_Theta += m_PanRateTheta	* dt;}
	void rotatePhi(double dt) {m_Phi += m_PanRatePhi	* dt;}
	void scale(double dt);
	float smooth(double value, float min_value,float max_value) const;
public:
	CameraKinematics();
	virtual ~CameraKinematics(){}
	void clampPhi(double& phi,float min_phi,float max_phi, bool smooth_on = true);
	void clampTheta(double& theta,float min_theta,float max_theta, bool smooth_on = true);
	void reset();
	void resetDistance();
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
	void displacement(int x, int y, int dx, int dy);
	void setPhi(double phi) {m_Phi = phi;}
	double& getPhi() {return	m_Phi;}
	void setTheta(double theta) {m_Theta = theta;}
	double& getTheta() {return	m_Theta;}
	void setDistance(float d) {m_DistanceToObject = d;}
	double getDistance() const {return m_DistanceToObject;}
	void accept(CameraCommand* cc);
};

#endif //__CAMERAKINEMATICS_H__

