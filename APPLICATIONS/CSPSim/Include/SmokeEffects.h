// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file SmokeEffects.h
 *
 **/


#ifndef __SMOKEEFFECTS_H__
#define __SMOKEEFFECTS_H__

#include <osg/ref_ptr>
#include <osgParticle/Operator>
#include <osgParticle/SegmentPlacer>
#include <osgParticle/ModularEmitter>

#include <SimData/Matrix3.h>
#include <SimData/Vector3.h>

#include <vector>
#include <string>

namespace osgParticle {
	class ParticleSystem;
	class ParticleSystemUpdater;
	class Placer;
	class Counter;
	class Shooter;
}

namespace fx {
namespace smoke {


class SmokeSegments {
	typedef std::vector<osg::ref_ptr<osgParticle::SegmentPlacer> > PlacerArray;
	PlacerArray m_Placers;
	std::vector<simdata::Vector3> m_Sources;
	std::vector<simdata::Vector3> m_LastPlace;
public:
	SmokeSegments() {
	}
	~SmokeSegments() {
	}
	int addSource(simdata::Vector3 const &v);
	osgParticle::SegmentPlacer *getSegment(int i);
	void update(simdata::Vector3 const &motion, simdata::Matrix3 const &to_body, simdata::Matrix3 const &to_scene);
};

class VortexExpander: public osgParticle::Operator
{
public:
	META_Object(csp, VortexExpander);
	VortexExpander() {}
	VortexExpander(const VortexExpander &copy, const osg::CopyOp &copyop): Operator(copy, copyop) {}
	virtual void operate(osgParticle::Particle *p, double dt);
};


class BaseSystem
{
public:
	BaseSystem();
	virtual ~BaseSystem();
	void setDefault();
	void setTexture(const std::string & TextureFile);
	void setColorRange(const osg::Vec4 &colorMin, const osg::Vec4 &colorMax);
	void setAlphaRange(float alpha_0, float alpha_1);
	void setSizeRange(float size_0, float size_1);
	void setLifeTime(float t);
	void setShape(osgParticle::Particle::Shape const &shape);
	void setEmissive(bool emissive);
	void setLight(bool light);
	virtual void setCounter(osgParticle::Counter *counter);
	virtual osgParticle::Counter *getCounter();
	virtual void setShooter(osgParticle::Shooter *shooter);
	virtual osgParticle::Shooter *getShooter();
	virtual void setPlacer(osgParticle::Placer *placer);
	virtual osgParticle::Placer *getPlacer();
	virtual void setOperator(osgParticle::Operator *op);
	virtual osgParticle::Operator *getOperator();
	virtual osgParticle::ModularEmitter *create(osg::Transform *base, 
	                                            osgParticle::ParticleSystemUpdater *&psu);
						    
protected:
	osgParticle::Particle m_Prototype;
	bool m_Emissive;
	bool m_Light;
	std::string m_TextureFile;
	osg::ref_ptr<osgParticle::Operator> m_Operator;
	osg::ref_ptr<osgParticle::Operator> m_UserOperator;
	osg::ref_ptr<osgParticle::Placer> m_Placer;
	osg::ref_ptr<osgParticle::Placer> m_UserPlacer;
	osg::ref_ptr<osgParticle::Counter> m_Counter;
	osg::ref_ptr<osgParticle::Counter> m_UserCounter;
	osg::ref_ptr<osgParticle::Shooter> m_Shooter;
	osg::ref_ptr<osgParticle::Shooter> m_UserShooter;
};


class Thinner: public osgParticle::Operator
{
public:
	META_Object(csp, Thinner);
	Thinner() {}
	Thinner(const Thinner &copy, const osg::CopyOp &copyop): Operator(copy, copyop) {}
	virtual void operate(osgParticle::Particle *p, double dt);
	int m_X;
};

class Trail: public BaseSystem
{
public:
	Trail(): BaseSystem() { m_Segment = NULL; m_Speed = 0.0; }
	virtual osgParticle::Shooter *getShooter();
	virtual osgParticle::Counter *getCounter();
	void setExpansion(float speed);
protected:
	osgParticle::SegmentPlacer *m_Segment;
	float m_Speed;
};


} // smoke
} // fx 


#endif // __SMOKEEFFECTS_H__

