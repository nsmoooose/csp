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
	typedef std::deque<CameraCommand*> CameraCommandList;
	CameraCommandList m_CameraCommandList;

	// XXX: serialize
	const float m_BaseRate, m_DisplacementCoefficient;
	const float m_MinimumDistanceOffset, m_AbsoluteMaximumDistance;

	double m_AngleRotX, m_AngleRotZ;
	float m_PanRateX, m_PanRateZ, m_ZoomRate;
	double m_DistanceToObject, m_MinimumDistance;
	void rotateAboutZ(double dt) {m_AngleRotZ += m_PanRateZ	* dt;}
	void rotateAboutX(double dt) {m_AngleRotX += m_PanRateX	* dt;}
	void scale(double dt);
	float smooth(double value, float min_value,float max_value) const;
public:
	CameraKinematics();
	virtual ~CameraKinematics(){}
	void clampX(double& value,float min_value,float max_value, bool smooth_on = true);
	void clampZ(double& value,float min_value,float max_value, bool smooth_on = true);
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
	void setAngleX(double angle_x) {m_AngleRotX = angle_x;}
	double& getAngleX() {return	m_AngleRotX;}
	void setAngleZ(double angle_z) {m_AngleRotZ = angle_z;}
	double& getAngleZ() {return	m_AngleRotZ;}
	void setDistance(float d) {m_DistanceToObject = d;}
	double getDistance() const {return m_DistanceToObject;}
	void accept(CameraCommand* cm);
};

#endif //__CAMERAKINEMATICS_H__

