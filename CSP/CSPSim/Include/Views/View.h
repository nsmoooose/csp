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
 * @file View.h
 *
 **/

#ifndef __VIEW_H__
#define __VIEW_H__

#include <map>

#include <SimData/Quat.h>
#include <SimData/Ref.h>
#include <SimData/Vector3.h>

class CameraAgent;
class CameraKinematics;
class DynamicObject;
class SimObject;

class View {
	size_t m_ViewMode;
protected:
	bool m_Internal;
	simdata::Ref<DynamicObject> m_ActiveObject;
	CameraKinematics* m_CameraKinematics;
	void updateBody(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up);
	void updateWorld(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up);
	virtual void constrain(){}
public:
	View(size_t vm);
	virtual void activate(){}
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up, double dt) = 0;
	void accept(const simdata::Ref<DynamicObject> object);
	void accept(CameraKinematics* ck) {m_CameraKinematics = ck;}
	virtual void recalculate(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up, double dt){
		update(ep,lp,up,dt);
	}
	void cull();
	virtual ~View();
	bool isInternal() const {return m_Internal;}
};

class InternalView: public View {
public:
	InternalView(size_t vm):View(vm){m_Internal = true;}
	virtual void constrain();
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up, double dt);
	virtual void activate();
	virtual ~InternalView(){}
};

class InternalViewHist: public InternalView {
public:
	InternalViewHist(size_t vm):InternalView(vm){}
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up, double dt);
	virtual ~InternalViewHist(){}
};

class ExternalViewBody: public View {
public:
	ExternalViewBody(size_t vm):View(vm){}
	virtual void activate();
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up, double dt);
	virtual ~ExternalViewBody(){}
};

class ExternalViewWorld: public View {
public:
	ExternalViewWorld(size_t vm):View(vm){}
	virtual void activate();
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt);
	virtual ~ExternalViewWorld(){}
};


class FlybyView: public View {
	simdata::Vector3 m_FixedCameraPosition;
	void newFixedCamPos(SimObject* target);
public:
	FlybyView(size_t vm):View(vm){}
	virtual void activate();
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt);
	virtual void recalculate(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt);
	virtual ~FlybyView(){}
};

class SatelliteView: public View {
public:
	SatelliteView(size_t vm):View(vm){}
	virtual void activate();
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt);
	virtual ~SatelliteView(){}
};

class PadlockView: public View {
	// padlock testing
	simdata::Ref<DynamicObject> m_Padlock;
	float m_NeckPhi,m_NeckTheta,m_psi;
	bool m_NeckLimit;
	simdata::Quat m_Attitude;
	void constrain(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt);
public:
	PadlockView(size_t vm);
	virtual void activate();
	virtual void update(simdata::Vector3& ep,simdata::Vector3& lp,simdata::Vector3& up,double dt);
	virtual ~PadlockView(){}
};


typedef std::map<size_t,View*> ViewList;

class ViewFactory {
	View* createView_1() const;
	View* createView_2() const;
	View* createView_3() const;
	View* createView_4() const;
	View* createView_7() const;
	View* createView_8() const;
	View* createView_9() const;
public: 
	void attachAllView(CameraAgent* ca) const;
};	

#endif //__VIEW_H__

